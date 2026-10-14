#ifndef QOI_H_
#define QOI_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>

#define QOI_OP_RGB 0b11111110
#define QOI_OP_RGBA 0b11111111
#define QOI_OP_INDEX 0x00
#define QOI_OP_DIFF  0x40
#define QOI_OP_LUMA  0x80
#define QOI_OP_RUN   0xc0
#define QOIF_HEADER 0x716F6966

struct rgba8_t {
    uint8_t r, g, b, a;
};

struct QOI_header_info {
    uint32_t width;
    uint32_t height;
    uint8_t channels;
    uint8_t colorspace;
};

uint32_t read_uint32(FILE *file) {
    uint8_t bytes[4];
    if (fread(bytes, 1, 4, file) != 4) {
        return 0;
    }
    return ((uint32_t)bytes[0] << 24) | 
           ((uint32_t)bytes[1] << 16) | 
           ((uint32_t)bytes[2] << 8)  | 
             (uint32_t)bytes[3];
}

typedef struct rgba8_t rgba8_t;
typedef struct QOI_header_info QOI_header_info;

bool pixel_compare(rgba8_t *a, rgba8_t *b) {
    return (a->r == b->r && a->g == b->g && a->b == b->b && a->a == b->a);
}

void write_uint32(FILE *file, uint32_t value) {
    uint8_t bytes[4] = {
        (value >> 24) & 0xFF,
        (value >> 16) & 0xFF,
        (value >> 8)  & 0xFF,
        value         & 0xFF
    };
    fwrite(bytes, 1, 4, file);
}

void qoi_encoder(uint8_t *pixels, QOI_header_info info, char* file_name) {
    FILE *file = fopen(file_name, "wb");

    if (file == NULL) {
        perror("failed to open file.");
        return;
    }

    //header (14 bytes)
    write_uint32(file, QOIF_HEADER);
    write_uint32(file, info.width);
    write_uint32(file, info.height);
    fputc(info.channels, file);
    fputc(info.colorspace, file);

    rgba8_t run[64] = {0};
    uint8_t run_length = 0;

    rgba8_t ppixel = {0, 0, 0, 255};
    rgba8_t cpixel = ppixel;
    
    uint64_t size = info.width*info.height*info.channels;

    for (uint64_t i = 0; i < size; i+=info.channels) {
        //get current pixel
        cpixel.r = pixels[i];
        cpixel.g = pixels[i+1];
        cpixel.b = pixels[i+2];
        if (info.channels == 4) cpixel.a = pixels[i+3];

        if (pixel_compare(&cpixel, &ppixel)) {
            run_length++;
            if (run_length == 62 || (i + info.channels) >= size) {
                fputc(QOI_OP_RUN | (run_length - 1), file);
                run_length = 0;
            }
        } else {
            if (run_length > 0) {
                fputc(QOI_OP_RUN | (run_length - 1), file);
                run_length = 0;
            }
            
            int index_pos = (cpixel.r * 3 + cpixel.g * 5 + cpixel.b * 7 + cpixel.a * 11) % 64;

            if (pixel_compare(&cpixel, run + index_pos)) {
                fputc(QOI_OP_INDEX | index_pos, file);
            } else if (cpixel.a == ppixel.a) {
                int dr = cpixel.r - ppixel.r;
                int dg = cpixel.g - ppixel.g;
                int db = cpixel.b - ppixel.b;

                int dr_g = dr - dg;
                int db_g = db - dg;

                if (dr >= -2 && dr <= 1 && dg >= -2 && dg <= 1 && db >= -2 && db <= 1) {
                    fputc(QOI_OP_DIFF | ((dr + 2) << 4) | ((dg + 2) << 2) | (db + 2), file);
                } else if (dg >= -32 && dg <= 31 && dr_g >= -8 && dr_g <= 7 && db_g >= -8 && db_g <= 7) {
                    fputc(QOI_OP_LUMA | (dg + 32), file);
                    fputc(((dr_g + 8) << 4) | (db_g + 8), file);
                } else {
                    fputc(QOI_OP_RGB, file);
                    fputc(cpixel.r, file);
                    fputc(cpixel.g, file);
                    fputc(cpixel.b, file);
                }
            } else {
                fputc(QOI_OP_RGBA, file);
                fputc(cpixel.r, file);
                fputc(cpixel.g, file);
                fputc(cpixel.b, file);
                fputc(cpixel.a, file);
            }

            run[index_pos] = cpixel;
        }
        ppixel = cpixel;
    }

    // Official 8-byte stream end marker
    for (int i = 0; i < 7; i++) fputc(0, file);
    fputc(1, file);

    fclose(file);
}

uint8_t *qoi_decoder(FILE *file, QOI_header_info *info) {

    //header
    if (read_uint32(file) != QOIF_HEADER) return NULL; //not qoi file
    info->width = read_uint32(file);
    info->height = read_uint32(file);
    info->channels = fgetc(file);
    info->colorspace = fgetc(file);

    uint64_t size = info->width*info->height*info->channels;

    uint8_t *buffer = (uint8_t*)(size*sizeof(uint8_t));

    if (buffer == NULL) {
        perror("Failed to allocate buffer.");
        return NULL;
    }

    uint64_t i = 0;

    rgba8_t run[64] = {0};

    rgba8_t pixel = {0, 0, 0, 255};

    while (i < size) {
        int c = fgetc(file);
        bool skip_fallback_write = false;

        if (c == EOF) break;

        if (c == QOI_OP_RGB) {
            pixel.r = fgetc(file);
            pixel.g = fgetc(file);
            pixel.b = fgetc(file);

        } else if (c == QOI_OP_RGBA) {
            pixel.r = fgetc(file);
            pixel.g = fgetc(file);
            pixel.b = fgetc(file);
            pixel.a = fgetc(file);

        } else {
            switch (c & 0xc0) {
                case QOI_OP_INDEX:
                    pixel = run[c & 0x3f];
                    break;
                case QOI_OP_DIFF:
                    pixel.r += ((c >> 4) & 0x03) - 2;
                    pixel.g += ((c >> 2) & 0x03) - 2;
                    pixel.b += (c & 0x03) - 2;
                    break;
                case QOI_OP_LUMA:
                    ; //null statement to be able to declare var
                    int vg = (c & 0x3f) - 32;
                    int c2 = fgetc(file);
                    pixel.r += vg - 8 + ((c2 >> 4) & 0x0f);
                    pixel.g += vg;
                    pixel.b += vg - 8 + (c2 & 0x0f);
                    break;
                case QOI_OP_RUN:
                ;
                uint8_t run_len = (c & 0x3f) + 1;
                for (uint8_t r = 0; r < run_len && i < size; r++) {
                    buffer[i] = pixel.r;
                    buffer[i+1] = pixel.g;
                    buffer[i+2] = pixel.b;
                if (info->channels == 4) {
                    buffer[i+3] = pixel.a;
                }
                i += info->channels;
            }
                skip_fallback_write = true;
                break;
            }
        }

        if (!skip_fallback_write && i < size) {
            buffer[i] = pixel.r;
            buffer[i+1] = pixel.g;
            buffer[i+2] = pixel.b;
            if (info->channels == 4) buffer[i+3] = pixel.a;

            int r_index = (pixel.r * 3 + pixel.g * 5 + pixel.b * 7 + pixel.a * 11) % 64;
            run[r_index] = pixel;   
            i += info->channels;
        }
    }

    return buffer;

}

#endif
