
#include "image.h"
#include "drawing/drawing.h"


size_t Image::getBufferSize() { return height * width / 8; }
size_t Image::getByteIndex(size_t x, size_t y) { /* NOT IMPLEMENTED */} //Should return floor
size_t Image::getBitOffset(size_t x) { /* NOT IMPLEMENTED */}
void   Image::setBufferByte(size_t index, Color color, uint8_t mask = 0xFF) {

    //set green
    this->greenBuffer[index] |= (  mask  & color.green);
    this->greenBuffer[index] &= ((~mask) | color.green);

    // set alpha (only if alpha channel is set as alpha source)
    if (alphaMode == AlphaMode::BUFFER) {
        this->alphaBuffer[index] |= (  mask  & color.alpha);
        this->alphaBuffer[index] &= ((~mask) | color.alpha);
    }
};
Color Image::getBufferByte(size_t index) {

    uint8_t green = greenBuffer[index];
    uint8_t alpha;

    // select alpha source
    switch (this->alphaMode)
    {
        case AlphaMode::BUFFER:   alpha = alphaBuffer[index]; break;
        case AlphaMode::OPAQUE:   alpha = 0x00; break;
        case AlphaMode::XOR_MASK: alpha = 0xFF; break;
    }

    return (Color){.green = green, .alpha = alpha};
};