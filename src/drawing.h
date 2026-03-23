#pragma once

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdexcept>

#include "generic.h"

typedef struct {
    const bool green;
    const bool alpha;
} Color;

const Color COLOR_BLACK       = {.green = false, .alpha = false};
const Color COLOR_GREEN       = {.green = true , .alpha = false};
const Color COLOR_TRANSPARENT = {.green = false, .alpha = true };
const Color COLOR_INVERT      = {.green = true , .alpha = true };


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





