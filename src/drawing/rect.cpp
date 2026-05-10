
#include "../generic.h"
#include "drawing.h"

Rect::Rect(size_t xPos, size_t yPos, size_t width, size_t height) {
    this->pos  = Point(xPos,  yPos  );
    this->size = Point(width, height);
}
Rect::Rect(Point pos, Point size) {
    this->pos  = pos;
    this->size = size;
}
Rect::Rect(Line* line) {
    this->pos = Point(
        min(line->start.x, line->end.x),
        min(line->start.y, line->end.y)
    );
    this->size = Point(
        max(line->start.x, line->end.x) - this->pos.x,
        max(line->start.y, line->end.y) - this->pos.y
    );
}

// cut rectangles
Rect Rect::operator && (Rect that) {
    Point near = Point::maxPoint(this->pos, that.pos);
    Point far  = Point::minPoint(this->pos + this->size, that.pos + that.size);
    return Rect(near, far - near);
}
// bounding rectangle
Rect Rect::operator || (Rect that) {
    Point near = Point::minPoint(this->pos, that.pos);
    Point far  = Point::maxPoint(this->pos + this->size, that.pos + that.size);
    return Rect(near, far - near);
}