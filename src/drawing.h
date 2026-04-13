#pragma once

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdexcept>

#include "generic.h"

// Can describe color of a pixel (if one of color constants), or color distribution of a byte (always)

typedef struct {
    const uint8_t green;
    const uint8_t alpha;
} Color;

Color inline colorMask(Color source, Color target) {
    return {
        .green = source.green ^ (source.alpha & target.green),
        .alpha = source.alpha & target.alpha
    };
}

// color constants
const Color COLOR_BLACK       = {.green = 0x00,  .alpha = 0x00 };
const Color COLOR_GREEN       = {.green = 0xFF , .alpha = 0x00 };
const Color COLOR_TRANSPARENT = {.green = 0x00,  .alpha = 0xFF };
const Color COLOR_INVERT      = {.green = 0xFF , .alpha = 0xFF };



class Point {
    public:
    inline Point() = default;
    inline Point(size_t x, size_t y) {
        this->x = x; this->y = y;
    }
    size_t x;
    size_t y;
};

class Line {
    public:
    inline Line() = default;
    inline Line(size_t startX, size_t startY, size_t endX, size_t endY) {
        this->start = Point(startX, startY);
        this->end   = Point(endX,   endY  );
    }
    inline Line(Point start, Point end) {
        this->start = start;
        this->end   = end;
    }

    Point start;
    Point end;
};





