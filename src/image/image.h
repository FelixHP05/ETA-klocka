#pragma once

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>

#include "drawing/drawing.h"
#include "generic.h"

/*
    Defines underlying source of alpha data in images
*/
enum AlphaMode {
    BUFFER,         // defined by buffer
    OPAQUE,         // alpha channel always opaque (0)
    XOR_MASK,       // alpha channel always transparent (1)
};

/*
    Wraps two optional framebuffers (green and alpha),
    along with metadata.

    Images may own framebuffers, or they may not, depending on whether they were passed to the constructor or not.
    Owning images yeild non-owning images when copied.
    Owning images will free framebuffers upon being destroyed.
*/
class Image {

    
    public:
    // constructors
    Image(uint8_t* greenBuffer, uint8_t* alphaBuffer, size_t width, size_t height);
    Image(uint8_t* greenBuffer, uint8_t* alphaBuffer, Point size);
    Image(Point size, Color fill);

    // copy/move/destruct behaviour
    Image(const Image& other);
    Image(Image&& other);      
    ~Image();
    Image& operator=(const Image& other);
    Image& operator=(Image&& other); 

    /*
    * framebuffer bit order:
    *   |byte0   |byte1   |
    *   |76543210|76543210|...new line
    *   line break MUST be on byte border
    */
    
    private: //internal data
    uint8_t* greenBuffer;       // 1-bit color channel
    uint8_t* alphaBuffer;       // 1-bit color channel, may be NULL
    size_t width;               // width of image, in PIXELS (will ALWAYS be a multiple of 8, make sure u call constructor like so)
    size_t height;              // height of image, in PIXELS
    bool ownsBuffers = false;   // destructor will free buffers
    AlphaMode alphaMode;        // determines behaviour of setBufferByte/getBufferByte

    
    public: // publicly exposed copies
    inline size_t getWidth() { return width; }
    inline size_t getHeight() { return height; }

    private: // utility functions
    size_t inline getBufferSize() { return height * width / 8; }
    size_t inline getByteIndex(size_t x, size_t y) { /* NOT IMPLEMENTED */} //Should return floor
    size_t inline getBitOffset(size_t x) { /* NOT IMPLEMENTED */}
    void inline setBufferByte(size_t index, Color color, uint8_t mask = 0xFF) {
        if (this->greenBuffer != NULL) {
            this->greenBuffer[index] |= (  mask  & color.green);
            this->greenBuffer[index] &= ((~mask) | color.green);
        } 
        if (this->alphaBuffer != NULL) {
            this->alphaBuffer[index] |= (  mask  & color.alpha);
            this->alphaBuffer[index] &= ((~mask) | color.alpha);
        } 
    }
    Color getBufferByte(size_t index);


    public: // exposed functions
    bool inline setPixel(Point pos, Color color) {

        //check if in image
        if ( pos.x < this->width & pos.y < this->height) {
            
            // calculate where pixel is
            uint8_t bitMask = 0x01 << 7-(pos.x % 8);
            size_t  byteIndex = pos.y * this->width/8 + pos.x/8;

            //apply to each channel IFF it exists
            if (this->greenBuffer != NULL) this->greenBuffer[byteIndex] =
                (this->greenBuffer[byteIndex] & ~bitMask) | (color.green & bitMask);
            if (this->alphaBuffer != NULL) this->alphaBuffer[byteIndex] =
                (this->alphaBuffer[byteIndex] & ~bitMask) | (color.alpha & bitMask);
            
            //success
            return true;
        }
        //failure
        else return false;
    }

    void inline drawLine(Line line, float thickness, Color color ) {

        float deltaY = (float)line.end.y - (float)line.start.y;
        float deltaX = (float)line.end.x - (float)line.start.x;

        //choose wether to iterate over x or y, depending on slope
        if (abs(deltaX) >= abs(deltaY)) {
            float k = deltaY / deltaX;
            float m = line.start.y - k * line.start.x;
            float width = thickness * sqrt(1+k*k); //positive cross-section along y         

            for (float x = min(line.start.x, line.end.x); x <= max(line.start.x, line.end.x); x++)
            for (float offset = -width/2; offset < width/2; offset++) {
                float y = ceil(k*x + m + offset);
                setPixel(Point(x, y), color);

            }
        } else {
            float k = deltaX / deltaY;
            float m = line.start.x - k * line.start.y;
            float width = thickness * sqrt(1+k*k); //positive cross-section along x

            for (int y = min(line.start.y, line.end.y); y <= max(line.start.y, line.end.y); y++)
            for (float offset = -width/2; offset < width/2; offset++) {
                int x = ceil(k*y + m + offset);
                setPixel(Point(x, y), color);

                // printf("dy: %f, ", deltaY);
                // printf("dx: %f, ", deltaX);
                // printf("thickness: %f, ", thickness);
                // printf("k: %f, ", k);
                // printf("width: %f, ", width);
                // printf("\n");
            }
        }
        
        return;
    }

    void inline drawLines(Point* points, size_t count, float thickness, Color color) {
        for (size_t p = 1; p < count; p++)
            drawLine(Line(points[p-1], points[p]), thickness, color);
        return;
    }

    void inline applyImage(Image img, Point pos) {

        //img should be right-shifted by these offsets
        int byteOffset = floorDiv(pos.x, 8);
        int bitOffset = modulo(pos.x, 8);

        // iterate over source bytes
        for (size_t byteY = 0; byteY < img.height;    byteY++) {

            // shift buffers (contains source color data, but bitshifted)
            uint32_t greenShift = 0x00000000; 
            uint32_t alphaShift = 0x00000000;

            for (size_t byteX = 0; byteX < img.width / 8; byteX++) {
                
                // indexes
                size_t sourceIndex = byteX                + img.width  /8 *  byteY;
                size_t targetIndex = (byteX + byteOffset) + this->width/8 * (byteY + pos.y);
                
                // offset
                greenShift <<= bitOffset + 8;                       // align old data at byte #2 (counting from LSB)
                greenShift  |= (img.greenBuffer[sourceIndex] << 8); // add new data at byte #1 (counting from LSB)
                greenShift >>= bitOffset;                           // shift aside (underflow partially into byte #0)

                alphaShift <<= bitOffset + 8;
                alphaShift  |= (img.alphaBuffer[sourceIndex] << 8);
                alphaShift >>= bitOffset;

                // apply
                Color color = colorMask(
                    { .green = (uint8_t)(greenShift >> 8),      .alpha = (uint8_t)(alphaShift >> 8)     },
                    { .green = this->greenBuffer[targetIndex],  .alpha = this->alphaBuffer[targetIndex] }
                );
                
                setBufferByte(
                    targetIndex,
                    color, 
                    byteX==0 ? (uint8_t)0xFF >> bitOffset : (uint8_t)0xFF
                ); 
                

            }
            // (byteX + byteOffset) + this->width/8 * (byteY + pos.y)
            // apply last byte
            size_t lastTargetIndex = (img.width / 8 + byteOffset) + this->width/8 * (byteY + pos.y); 
            Color lastColor = colorMask(
                { .green = (uint8_t)(greenShift >> 0),   .alpha = (uint8_t)(alphaShift >> 0)   },
                { .green = this->greenBuffer[lastTargetIndex], .alpha = this->alphaBuffer[lastTargetIndex] }
            );

            setBufferByte(
                lastTargetIndex,
                lastColor,
                (uint8_t)(0xFF << 8-bitOffset)
            ); //TODO, make sure ok when end of image aligns to byte border
        
        }
    }

    // fill rectangle, half-open bounds
    void inline drawRect(Rect rect, Color color) {
        
        // handle overflowed rectangle (trim to bounds)
        rect = rect && Rect(0, 0, width, height);

        size_t startByte = floorDiv(rect.pos.x, 8);                             // first byte index along width
        uint8_t startMask = 0xFF >> modulo(rect.pos.x, 8);                      // bitmask (LSB aligned or full)
        size_t stopByte = floorDiv(rect.pos.x + rect.size.x, 8);                // last byte index along width
        uint8_t stopMask = 0xFF << (8 - modulo(rect.pos.x + rect.size.x, 8));   // bitmask (MSB aligned or empty)

        // check for narrow rectangles (merge masks)
        if (startByte == stopByte) {
            startMask &= stopMask; stopMask = 0x00;
        }

        for (size_t y = rect.pos.y; y < rect.pos.y + rect.size.y; y++) {
            size_t baseIndex = y * this->width / 8;   // index of first byte on row

            setBufferByte(baseIndex + startByte, color, startMask); //apply start
            for (size_t x = startByte + 1; x < stopByte; x++)
                setBufferByte(baseIndex + x, color);                //apply full bytes
            setBufferByte(baseIndex + stopByte, color, stopMask);   //apply stop
        }
    }
};


