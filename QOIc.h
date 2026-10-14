#ifndef QOI_H_
#define QOI_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

#define QOI_OP_RGB 0b11111110
#define QOI_OP_RGBA 0b11111111
#define QOI_OP_INDEX 0b00
#define QOI_OP_DIFF 0b01
#define QOI_OP_LUMA 0b10
#define QOI_OP_RUN 0b11

struct rgba8_t {
    u_int8_t r, g, b, a;
};

struct QOI_header_info {
    uint32_t width;
    uint32_t height;
    uint8_t channels;
    uint8_t colorspace;
};

typedef struct rgba8_t rgba8_t;
typedef struct QOI_header_info QOI_header_info;

uint32_t read_uint32(FILE *file) {
    return fgetc(file) * 16777216 + fgetc(file) * 65536 + fgetc(file) * 256 + fgetc(file);
}

void write_uint32(FILE *file, unsigned int value) {
    fputc(value / 16777216 % 256, file);
    fputc(value / 65536 % 256, file);
    fputc(value / 256 % 256, file);
    fputc(value % 256, file);
}

int compare_pixels(rgba8_t p1, rgba8_t p2) {
    if (p1.r == p2.r && p1.g == p2.g && p1.b == p2.b && p1.a == p2.a) return 1;
    return 0;
}

void qoi_encoder(uint8_t *pixels, QOI_header_info info, char* file_name) {
    FILE *file;
    file = fopen(file_name, "w");

    fputs("qoif", file);
    write_uint32(file, info.width);
    write_uint32(file, info.height);

    fputc(info.channels, file);
    fputc(info.colorspace, file);

    rgba8_t run[64];
    
    rgba8_t ppixel;
    ppixel.r = 0;
    ppixel.g = 0;
    ppixel.b = 0;
    ppixel.a = 255;

    rgba8_t cpixel = ppixel;
    uint8_t rlen = 0;

    for (uint16_t i = 0; i < info.width * info.height * info.channels; i += info.channels) {
        cpixel.r = pixels[i];
        cpixel.g = pixels[i+1];
        cpixel.b = pixels[i+2];
        if (info.channels == 4) cpixel.a = pixels[i+3];

        int index_pos = (cpixel.r * 3 + cpixel.g * 5 + cpixel.b * 7 + cpixel.a * 11) % 64;

        if (compare_pixels(cpixel, ppixel)) {
            rlen++;

            rgba8_t npixel;
            npixel.r = pixels[i + 4];
            npixel.g = pixels[i + 5];
            npixel.b = pixels[i + 6];
            if (info.channels == 4) npixel.a = pixels[i + 7];
            if (!compare_pixels(cpixel, npixel) || rlen == 62 || i == info.width * info.height * info.channels-1) {
                fputc(rlen + 191, file);
                rlen = 0;
            }
        } else if (compare_pixels(cpixel, run[index_pos])) {
            fputc(index_pos, file);
        } else if (cpixel.a == ppixel.a) {

            int dr = cpixel.r - ppixel.r;
            int dg = cpixel.g - ppixel.g;
            int db = cpixel.b - ppixel.b;

            int dr_g = dr - dg;
            int db_g = db - dg;

            if (dr >= -2 && dr <= 1 && dg >= -2 && dg <= 1 && db >= -2 && db <= 1) {
                fputc(64 | (dr << 4) + (dg << 2) + db, file);
            } else if (dg >= -32 && dg <= 31 && dr_g >= -8 && dr_g <= 7 && db_g >= -8 && db_g <= 7) {
                fputc(128 | (dg + 32), file);
                fputc((dr_g + 8) << 4 | (db_g + 8), file);

            } else {
                fputc(254, file);
                fputc(cpixel.r, file);
                fputc(cpixel.g, file);
                fputc(cpixel.b, file);
            }
        } else {
            fputc(255, file);
            fputc(cpixel.r, file);
            fputc(cpixel.g, file);
            fputc(cpixel.b, file);
            fputc(cpixel.a, file);
        }

        ppixel = cpixel;


        run[index_pos] = cpixel;
    }

    for (int i = 0; i < 7; i++) fputc(0, file);
    fputc(1, file);

	fclose(file);
}

uint8_t* qoi_decoder(FILE *file, QOI_header_info *h_info) {
    char magic[4] = {'q', 'o', 'i', 'f'};
    char magic_get[4];
    for (uint8_t i = 0; i < 4; i++) magic_get[i] = fgetc(file);
    if (!strcmp(magic, magic_get)) return 0;

    h_info->width = read_uint32(file);
    h_info->height = read_uint32(file);

    h_info->channels = fgetc(file);     //3 = RGB, 4 = RGBA
    h_info->colorspace = fgetc(file);   //0 = sRGB with linear alpha, 1 = all channels linear

    uint32_t pixel_length = h_info->width * h_info->height * h_info->channels;
    uint8_t *color = (uint8_t*)malloc(pixel_length);
    rgba8_t run[64];
    
    rgba8_t pixel;
    pixel.r = 0;
    pixel.g = 0;
    pixel.b = 0;
    pixel.a = 255;

    uint8_t c;
    for (uint16_t index = 0; index < pixel_length; index += h_info->channels) {
        c = fgetc(file);

        if (c == QOI_OP_RGB) {
            pixel.r = fgetc(file);
            pixel.g = fgetc(file);
            pixel.b = fgetc(file);
        } else if (c == QOI_OP_RGBA) {
            pixel.r = fgetc(file);
            pixel.g = fgetc(file);
            pixel.b = fgetc(file);
            pixel.a = fgetc(file);
        }  else if (c >> 6 == QOI_OP_INDEX) {
            pixel.r = run[c].r;
            pixel.g = run[c].g;
            pixel.b = run[c].b;
        } else if (c >> 6 == QOI_OP_DIFF) {
            pixel.r += (c >> 4 & 3) - 2;
            pixel.g += (c >> 2 & 3) - 2;
            pixel.b += (c & 3) - 2;
        } else if (c >> 6 == QOI_OP_LUMA) {
			int vg = (c & 0x3f) - 32;
            c = fgetc(file);
			pixel.r += vg - 8 + (c >> 4);
			pixel.g += vg;
			pixel.b += vg - 8 + (c & 15);
        } else if (c >> 6 == QOI_OP_RUN) {
            uint8_t length = (c & 63) + 1;
            for (int i = 0; i < length; i++) {
                color[index++] = pixel.r;
                color[index++] = pixel.g;
                color[index++] = pixel.b;
                color[index++] = pixel.a;
            }
            index-=4;
            continue;
        }

        color[index] = pixel.r;
        color[index+1] = pixel.g;
        color[index+2] = pixel.b;
        if (h_info->channels == 4) color[index+3] = pixel.a;

        int r_index = (pixel.r * 3 + pixel.g * 5 + pixel.b * 7 + pixel.a * 11) % 64;

        run[r_index].r = pixel.r;
        run[r_index].g = pixel.g;
        run[r_index].b = pixel.b;
        run[r_index].a = pixel.a;
    }

    return color;
}

#endif
