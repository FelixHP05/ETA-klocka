#pragma once

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdexcept>


// Can describe color of a pixel (if one of color constants), or color distribution of a byte (always)
typedef struct {
    const uint8_t green;
    const uint8_t alpha;
} Color;

// color constants
inline Color COLOR_BLACK       = {0x00, 0x00};
inline Color COLOR_GREEN       = {0xFF, 0x00};
inline Color COLOR_TRANSPARENT = {0x00, 0xFF};
inline Color COLOR_INVERT      = {0xFF, 0xFF}; 

// color masking
uint8_t greenMask(Color source, Color target);
uint8_t alphaMask(Color source, Color target);
uint8_t alphaMask(uint8_t sourceAlpha, uint8_t targetAlpha);
Color colorMask(Color source, Color target);

class Point {
    public:
    inline Point() = default;
    Point(size_t x, size_t y);

    //point operations
    Point operator + (Point that);
    Point operator - (Point that);
    static Point minPoint(Point a, Point b);
    static Point maxPoint(Point a, Point b);

    //fields
    size_t x;
    size_t y;
};

class Rect;
class Line {
    public:
    inline Line() = default;
    Line(size_t startX, size_t startY, size_t endX, size_t endY);
    Line(Point start, Point end);
    Line(Rect* rect);

    Point start;
    Point end;
};

// Rectangle, represented by half-open integer interval between two corners.
class Rect {
    public:
    inline Rect() = default;
    Rect(size_t xPos, size_t yPos, size_t width, size_t height);
    Rect(Point pos, Point size);
    Rect(Line* line);

    // cut rectangles
    Rect operator && (Rect that);

    // bounding rectangle
    Rect operator || (Rect that);

    // shorthand properties
    inline size_t left()   { return pos.x;          }
    inline size_t right()  { return pos.x + size.x; }
    inline size_t top()    { return pos.y;          }
    inline size_t bottom() { return pos.y + size.y; }

    Point pos;
    Point size;
};







