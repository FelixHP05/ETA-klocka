#pragma once

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <pico/stdlib.h>
#include <stdexcept>
#include <math.h>

#include "drawing.h"
#include "generic.h"

class Image {

    public:
    inline Image() = default;
    inline Image(uint8_t* greenBuffer, uint8_t* alphaBuffer, size_t width, size_t height) {
        this->greenBuffer = greenBuffer;
        this->alphaBuffer = alphaBuffer;
        this->width  = width;
        this->height = height;
    }
    inline Image(uint8_t* greenBuffer, uint8_t* alphaBuffer, Point size) :
        Image::Image(greenBuffer, alphaBuffer, size.x, size.y) {}
    inline Image(Point size, Color fill) { /* NOT IMPLEMENTED */ }

    /*
    * framebuffer bit order:
    *   |byte0   |byte1   |
    *   |76543210|76543210|...new line
    */

    // 1-bit color channels
    uint8_t* greenBuffer;
    uint8_t* alphaBuffer;

    // width and height of green and alpha channels, in PIXELS
    size_t width;
    size_t height;

    private:
    size_t inline getBufferSize() { return height * width / 8; }
    size_t inline getBufferIndex(size_t x, size_t y) {  }


    
    public:
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

                // printf("dy: %f, ", deltaY);
                // printf("dx: %f, ", deltaX);
                // printf("thickness: %f, ", thickness);
                // printf("k: %f, ", k);
                // printf("width: %f, ", width);
                // printf("\n");
            }
        } else {
            float k = deltaX / deltaY;
            float m = line.start.x - k * line.start.y;
            float width = thickness * sqrt(1+k*k)/abs(k); //positive cross-section along x

            for (int y = min(line.start.y, line.end.y); y <= max(line.start.y, line.end.y); y++)
            for (float offset = -width/2; offset < width/2; offset++) {
                int x = ceil(k*y + m + offset);
                setPixel(Point(x, y), color);
            }
        }
        
        return;
    }

    void inline drawLines(Point* points, size_t count, float thickness, Color color) {
        for (size_t p = 1; p < count; p++)
            drawLine(Line(points[p-1], points[p]), thickness, color);
        return;
    }

    void inline drawImage(Image img, Point pos) {

        //img should be right-shifted by these offsets
        int byteOffset = floorDiv(pos.x, 8);
        int bitOffset = modulo(pos.x, 8);

        // shift buffers 
        uint32_t greenShift = 0;
        uint32_t alphaShift = 0;

        for (size_t byteY = 0; byteY < img.height;    byteY++) {
            for (size_t byteX = 0; byteX < img.width / 8; byteX++) {

                size_t sourceIndex = byteX                + img.width   * byteY;
                size_t targetIndex = (byteX + byteOffset) + this->width * byteY;
                
                greenShift <<= bitOffset + 8;                       alphaShift <<= bitOffset + 8;
                greenShift  |= (img.greenBuffer[sourceIndex] << 8); alphaShift  |= (img.alphaBuffer[sourceIndex] << 8);
                greenShift >>= bitOffset;                           alphaShift >>= bitOffset;

                Color color = colorMask(
                    { .green = (greenShift >> 8)&0xFF,         .alpha = (alphaShift >> 8)&0xFF         },
                    { .green = this->greenBuffer[targetIndex], .alpha = this->alphaBuffer[targetIndex] }
                );
                this->greenBuffer[targetIndex] = color.green;
                this->alphaBuffer[targetIndex] = color.alpha; 
            
                
            }
        }



    }

    void inline fill(Color color) {
        for (size_t i = 0; i < getBufferSize(); i++) {
            if (this->greenBuffer != NULL) this->greenBuffer[i] = color.green;
            if (this->alphaBuffer != NULL) this->alphaBuffer[i] = color.alpha;
        }
    }
};


