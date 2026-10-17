# QOI Image Encoder / Decoder (QOIc.h)
A lightweight, header-only C implementation of the QOI (Quite OK Image Format) encoder and decoder.
QOI is an image format that achieves compression ratios comparable to PNG while offering dramatically faster encoding and decoding speeds (typically 20x–50x faster encoding and 3x–4x faster decoding than standard PNG implementations).
## Features
- Header-Only: Easy to integrate into any existing C or C++ project—just drop QOIc.h into your codebase.
- High Performance: Designed with speed and simplicity in mind.
- Full Support: Handles both RGB (24-bit) and RGBA (32-bit) image channels.
- Minimal Dependencies: Built on standard C libraries.
## Getting Started
 -Installation
Since this is a header-only library, simply copy QOIc.h into your project directory and include it where needed.

## Code Exemples

### Encoder

```text
QOI_header_info info = {
    width = 800,
    height = 600,
    channels = 4, // 3 for RGB, 4 for RGBA
    colorspace = QOI_SRGB
};

// pixel_data is a pointer to raw RGBA/RGB bytes
qoi_encoder(uint8_t *pixel_buffer, QOI_header_info info, "image.qoi");
```

### Decoder

```text
FILE *file = open("image.qoi");

if (file == NULL) {
  //Handle error 
}

QOI_header_info info;

void *pixel_data = qoi_decoder(file, &info);

if (pixel_data == NULL) {
    // Handle error
}

// Use pixel_data...

// Free the pixel data when done
free(pixel_data);
```

## API Reference

- ```void qoi_encoder(uint8_t *pixels, QOI_header_info info, char* file_name): Encodes raw data into memory.```
- ```uint8_t *qoi_decoder(FILE *file, QOI_header_info *info): Decodes QOI bytes from memory.```

## Reference

Official QOI Specification [https://qoiformat.org/qoi-specification.pdf]: The official specification document detailing the file format, chunk structures, and encoding/decoding rules.
