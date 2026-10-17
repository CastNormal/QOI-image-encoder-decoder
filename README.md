# QOI Image Encoder / Decoder (QOIc.h)
A lightweight, header-only C implementation of the QOI (Quite OK Image Format) encoder and decoder.
QOI is an image format that achieves compression ratios comparable to PNG while offering dramatically faster encoding and decoding speeds.
## Features
- Header-Only: Easy to integrate into any existing C or C++ project—just drop QOIc.h into your codebase.
- High Performance: Designed with speed and simplicity in mind.
- Full Support: Handles both RGB (24-bit) and RGBA (32-bit) image channels.
- Minimal Dependencies: Built on standard C libraries (```<stdio.h>```, ```<string.h>```, ```<stdlib.h>```, etc.)
## Getting Started
 -Installation
Since this is a header-only library, simply copy QOIc.h into your project directory and include it where needed.

## Data Structures
```text
typedef struct {
    uint8_t r, g, b, a;
} rgba8_t;

typedef struct {
    uint32_t width;
    uint32_t height;
    uint8_t channels;
    uint8_t colorspace;
} QOI_header_info;
```

## Code Exemples

### Encoder

```text
#include <stdio.h>
#include <stdlib.h>
#include "QOIc.h"

int main() {
    int width = 800;
    int height = 600;
    int channels = 4; // 3 for RGB, 4 for RGBA
    
    // Allocate or load your raw pixel buffer
    uint32_t size = width * height * channels;
    uint8_t *pixels = (uint8_t *)malloc(size);
    // Fill pixel data...

    QOI_header_info info = {
        .width = width,
        .height = height,
        .channels = channels,
        .colorspace = 0 // e.g., 0 for sRGB with linear alpha, 1 for all linear
    };

    qoi_encoder(pixels, info, "output.qoi");

    free(pixels);
    return 0;
}
```

### Decoder

```text
#include <stdio.h>
#include <stdlib.h>
#include "QOIc.h"

int main() {
    FILE *file = fopen("image.qoi", "rb");
    if (!file) {
        perror("Failed to open file");
        return 1;
    }

    QOI_header_info info;
    uint8_t *pixels = qoi_decoder(file, &info);
    fclose(file);

    if (pixels == NULL) {
        fprintf(stderr, "Failed to decode QOI image.\n");
        return 1;
    }

    printf("Decoded image: %ux%u with %u channels\n", info.width, info.height, info.channels);

    // Use pixel buffer...

    free(pixels);
    return 0;
}
```

## API Reference

### qoi_encoder

```text
void qoi_encoder(
    uint8_t *pixels,
    QOI_header_info info,
    char *file_name
);
```
Encodes raw RGB or RGBA pixel data into a QOI image file.

#### Parameters:

pixels: Pointer to the raw input pixel buffer.

info: Image dimensions, channel count, and colour-space information.

file_name: Destination filename for the encoded image.

Returns: void.

### qoi_decoder

```text
uint8_t *qoi_decoder(
    FILE *file,
    QOI_header_info *info
);
```
Decodes a QOI image from an open file stream.

#### Parameters:

file: Pointer to an open QOI file in binary-read mode.

info: Pointer to a structure populated with the decoded image metadata.

Returns: A pointer to the dynamically allocated decoded pixel buffer, or NULL if decoding fails.

The caller is responsible for closing the file stream and freeing the returned pixel buffer.



## Reference

Official QOI Specification [https://qoiformat.org/qoi-specification.pdf]: The official specification document detailing the file format, chunk structures, and encoding/decoding rules.
