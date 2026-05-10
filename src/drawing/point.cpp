


#include "drawing.h"
#include "../generic.h"

Point::Point(size_t x, size_t y) {
    this->x = x; this->y = y;
}

Point Point::operator + (Point that) { return Point(this->x + that.x, this->y + that.y); }
Point Point::operator - (Point that) { return Point(this->x - that.x, this->y - that.y); }

Point Point::minPoint(Point a, Point b) { return Point(min(a.x, b.x), min(a.y, b.y)); }
Point Point::maxPoint(Point a, Point b) { return Point(max(a.x, b.x), max(a.y, b.y)); }

