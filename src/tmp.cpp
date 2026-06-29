
#include <stdint.h>
#include "image.h"

static uint8_t green[] = {
    0xF1,
    0xF0,
    0x80,
    0x40,
    0x20,
    0x10,
    0x08,
    0x04,
};

Image a = Image(green, NULL, 8, 8);