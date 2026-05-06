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
    Point operator + (Point that) { return Point(this->x + that.x, this->y + that.y); }
    Point operator - (Point that) { return Point(this->x - that.x, this->y - that.y); }
    static inline Point minPoint(Point a, Point b) { return Point(min(a.x, b.x), min(a.y, b.y)); }
    static inline Point maxPoint(Point a, Point b) { return Point(max(a.x, b.x), max(a.y, b.y)); }

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
    inline Line(Rect rect) {
        this->start = rect.pos;
        this->end   = rect.pos + rect.size;
    }

    Point start;
    Point end;
};

class Rect {
    public:
    inline Rect() = default;
    inline Rect(size_t xPos, size_t yPos, size_t width, size_t height) {
        this->pos  = Point(xPos,  yPos  );
        this->size = Point(width, height);
    }
    inline Rect(Point pos, Point size) {
        this->pos  = pos;
        this->size = size;
    }
    inline Rect(Line line) {
        this->pos = Point(
            min(line.start.x, line.end.x),
            min(line.start.y, line.end.y)
        );
        this->size = Point(
            max(line.start.x, line.end.x) - this->pos.x,
            max(line.start.y, line.end.y) - this->pos.y
        );
    }

    // cut rectangles
    inline Rect operator && (Rect that) {
        Point near = Point::maxPoint(this->pos, that.pos);
        Point far  = Point::minPoint(this->pos + this->size, that.pos + that.size);
        return Rect(near, far - near);
    }
    // bounding rectangle
    inline Rect operator || (Rect that) {
        Point near = Point::minPoint(this->pos, that.pos);
        Point far  = Point::maxPoint(this->pos + this->size, that.pos + that.size);
        return Rect(near, far - near);
    }

    inline size_t left()   { return pos.x;          }
    inline size_t right()  { return pos.x + size.x; }
    inline size_t top()    { return pos.y;          }
    inline size_t bottom() { return pos.y + size.y; }

    Point pos;
    Point size;
};







