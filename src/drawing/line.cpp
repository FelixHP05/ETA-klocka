
#include "drawing.h"


Line::Line(size_t startX, size_t startY, size_t endX, size_t endY) {
    this->start = Point(startX, startY);
    this->end   = Point(endX,   endY  );
}
Line::Line(Point start, Point end) {
    this->start = start;
    this->end   = end;
}
Line::Line(Rect* rect) {
    this->start = rect->pos;
    this->end   = rect->pos + rect->size;
}