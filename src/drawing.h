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

// color constants
Color COLOR_BLACK       = {0x00, 0x00};
Color COLOR_GREEN       = {0xFF, 0x00};
Color COLOR_TRANSPARENT = {0x00, 0xFF};
Color COLOR_INVERT      = {0xFF, 0xFF}; 

// color masking
uint8_t greenMask(Color source, Color target) { return source.green ^ (source.alpha & target.green); }
uint8_t alphaMask(Color source, Color target) { return source.alpha & target.alpha; }
uint8_t alphaMask(uint8_t sourceAlpha, uint8_t targetAlpha) { return sourceAlpha & targetAlpha; }
Color inline colorMask(Color source, Color target) {
    return (Color){
        greenMask(source, target),
        alphaMask(source, target)
    };
}







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





