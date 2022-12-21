#include <stdio.h>
#include <string.h>
#include <inttypes.h>

uint8_t rgb_pixels[] = {165,102,81,165,102,81,146,90,72,134,111,105,187,109,88,187,109,88,187,109,88,165,102,81,165,102,81,165,102,81,146,90,72,157,134,126,187,109,88,165,102,81,165,102,81,165,102,81,146,90,72,146,90,72,134,83,67,157,134,126,117,72,57,134,83,67,146,90,72,146,90,72,146,90,72,146,90,72,134,83,67,157,134,126,117,72,57,146,90,72,146,90,72,146,90,72,117,72,57,108,66,52,108,66,52,165,150,143,108,66,52,108,66,52,134,83,67,134,83,67,117,72,57,108,66,52,108,66,52,157,134,126,108,66,52,134,83,67,117,72,57,117,72,57,134,111,105,134,111,105,157,134,126,165,150,143,157,134,126,134,111,105,134,111,105,134,111,105,134,111,105,157,134,126,157,134,126,165,150,143,157,134,126,134,111,105,134,111,105,134,111,105,187,109,88,187,109,88,165,102,81,165,102,81,146,90,72,165,102,81,146,90,72,157,134,126,187,109,88,165,102,81,165,102,81,165,102,81,146,90,72,165,102,81,146,90,72,134,111,105,117,72,57,146,90,72,146,90,72,146,90,72,146,90,72,134,83,67,117,72,57,157,134,126,117,72,57,134,83,67,134,83,67,134,83,67,134,83,67,134,83,67,117,72,57,157,134,126,117,72,57,134,83,67,146,90,72,117,72,57,117,72,57,117,72,57,108,66,52,165,150,143,108,66,52,108,66,52,117,72,57,134,83,67,134,83,67,117,72,57,108,66,52,157,134,126,157,134,126,134,111,105,134,111,105,134,111,105,134,111,105,157,134,126,157,134,126,165,150,143,157,134,126,157,134,126,134,111,105,134,111,105,134,111,105,134,111,105,157,134,126,165,150,143,165,102,81,165,102,81,146,90,72,157,134,126,187,109,88,187,109,88,187,109,88,165,102,81,146,90,72,165,102,81,146,90,72,157,134,126,187,109,88,187,109,88,165,102,81,165,102,81,146,90,72,146,90,72,134,83,67,157,134,126,134,83,67,146,90,72,146,90,72,146,90,72,146,90,72,146,90,72,117,72,57,157,134,126,134,83,67,146,90,72,146,90,72,146,90,72,117,72,57,117,72,57,108,66,52,157,134,126,117,72,57,117,72,57,134,83,67,134,83,67,117,72,57,117,72,57,108,66,52,165,150,143,117,72,57,134,83,67,134,83,67,134,83,67,134,111,105,157,134,126,157,134,126,165,150,143,157,134,126,134,111,105,134,111,105,134,111,105,134,111,105,134,111,105,157,134,126,165,150,143,157,134,126,134,111,105,134,111,105,134,111,105,146,90,72,165,102,81,165,102,81,165,102,81,146,90,72,165,102,81,146,90,72,134,111,105,187,109,88,165,102,81,165,102,81,165,102,81,146,90,72,165,102,81,146,90,72,157,134,126,117,72,57,146,90,72,146,90,72,146,90,72,146,90,72,146,90,72,134,83,67,157,134,126,134,83,67,146,90,72,146,90,72,146,90,72,146,90,72,146,90,72,117,72,57,157,134,126,108,66,52,117,72,57,117,72,57,117,72,57,117,72,57,117,72,57,108,66,52,165,150,143,108,66,52,117,72,57,117,72,57,117,72,57,117,72,57,117,72,57,108,66,52,165,150,143,157,134,126,134,111,105,134,111,105,134,111,105,134,111,105,134,111,105,157,134,126,165,150,143,157,134,126,157,134,126,134,111,105,134,111,105,134,111,105,157,134,126,165,150,143,165,150,143};

#define get_bit(n, x) ((n & ( 1 << x )) >> x)

#define QOI_OP_RGB 0b11111110
#define QOI_OP_RGBA 0b11111111

/*
//RGB
8-bit tag b11111110
8-bit   red channel value
8-bit green channel value
8-bit  blue channel value

alpha value is same as previous pixel
*/

/*
//RGBA
8-bit tag b11111111
8-bit   red channel value
8-bit green channel value
8-bit  blue channel value
8-bit alpha channel value
*/


struct qoi_header {
    char magic[4];
    int32_t width;
    uint32_t height;
    uint8_t channels;
    uint8_t colorspace;
};

void encoder(uint8_t *pixel_data) {

}

uint32_t get_uint32(FILE *file) {
    return fgetc(file) * 16777216 + fgetc(file) * 65536 + fgetc(file) * 256 + fgetc(file);
}

int decoder(FILE *file) {
    uint8_t c;

    char magic[4] = "qoif";
    char magic_get[4];
    for (uint8_t i = 0; i < 4; i++) magic_get[i] = fgetc(file);
    if (!strcmp(magic, magic_get)) return 0;

    uint32_t width = get_uint32(file);
    uint32_t height = get_uint32(file);
    uint8_t channels = fgetc(file);     //3 = RGB, 4 = RGBA
    uint8_t colorspace = fgetc(file);   //0 = sRGB with linear alpha, 1 = all channels linear

    //printf("width: %d, height: %d", width, height);

    uint8_t color[width * height * 4];
    uint16_t index = 0;
    uint16_t temp_index = index;

    uint8_t pred = 0;
    uint8_t pgreen = 0;
    uint8_t pblue = 0;
    uint8_t palpha = 255;

    uint8_t cred = 0;
    uint8_t cgreen = 0;
    uint8_t cblue = 0;
    uint8_t calpha = 255;

    while (1) {
        c = fgetc(file);
        //printf("%d, ", c);

        if (c == QOI_OP_RGB) {
            color[index++] = fgetc(file);
            color[index++] = fgetc(file);
            color[index++] = fgetc(file);
            color[index++] = palpha;
        } else if (c == QOI_OP_RGBA) {
            color[index++] = fgetc(file);
            color[index++] = fgetc(file);
            color[index++] = fgetc(file);
            color[index++] = fgetc(file);
        } else if (c >> 6 == 0b01) { //QOI_OP_LUMA
            color[index++] = ((c >> 4) & 0x03) - 2;
            color[index++] = ((c >> 2) & 0x03) - 2;
            color[index++] = (c        & 0x03) - 2;
            temp_index = index;
            color[index++] = color[temp_index-4];
        } else if (c >> 6 == 0b11) {
            uint8_t green_dif = c - 0b11000000;
            c = fgetc(file);
            uint8_t red_dif = (c >> 4) + green_dif;
            uint8_t blue_dif = c - (c >> 4) + green_dif;
            red_dif = -(red_dif >> 3) * 16 + red_dif; //from unsigned to signed
            blue_dif = -(blue_dif >> 3) * 16 + blue_dif;

            temp_index = index;
            color[index++] = red_dif + color[temp_index-3];
            temp_index = index;
            color[index++] = green_dif + color[temp_index-3];
            temp_index = index;
            color[index++] = blue_dif + color[temp_index-3];
            temp_index = index;
            color[index++] = color[temp_index-4];
        } else if (c >> 6 == 0b11) {
            uint8_t length = c & 0b00111111;

            uint8_t pr = color[index-3];
            uint8_t pg = color[index-2];
            uint8_t pb = color[index-1];
            uint8_t pa = color[index-0];
            for (int i = 0; i < length; i++) {
                color[index++] = pr;
                color[index++] = pg;
                color[index++] = pb;
                color[index++] = pa;
            }
        } else if (c >> 6 == 0b00) {
            
        }

        if ((char)c == EOF) break;
        index++;
    }

    for (int i = 0; i < width * height; i++) {
        printf("%d, %d, %d, %d\n", color[i], color[i+1], color[i+2], color[i+3]);
    }

    return 1;
}

int main() {
    FILE *file;
    file = fopen("minecraft_bricks.qoi", "r");

    decoder(file);
    
	return 0;	
}
