#pragma once

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <pico/stdlib.h>
#include <stdexcept>

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

    // 1-bit color channels
    uint8_t* greenBuffer;
    uint8_t* alphaBuffer;

    // width and height of green and alpha channels, in PIXELS
    size_t width;
    size_t height;

    bool inline setPixel(Point pos, Color color) {

        //check if in image
        if ( pos.x < this->width & pos.y < this->height) {
            
            // calculate where pixel is
            uint8_t bit = 7-(pos.x % 8);
            size_t  byteIndex = pos.y * this->width/8 + pos.x/8;

            //apply to each channel IFF it exists
            if (this->greenBuffer != NULL) this->greenBuffer[byteIndex] =
                (this->greenBuffer[byteIndex] &~ true << bit) | (color.green << bit) ;
            if (this->alphaBuffer != NULL) this->alphaBuffer[byteIndex] =
                (this->alphaBuffer[byteIndex] &~ true << bit) | (color.alpha << bit);
            
            //success
            return true;
        }
        //failure
        else return false;
    }

    bool inline drawLine(Line line, Color color) {

        float k = (line.end.x - line.start.x) / (float)(line.end.y - line.start.y);
        float m = line.start.y - k * line.start.x;

        for (int x = min(line.start.x, line.end.x); x <= max(line.start.x, line.end.x); x++) {
            int y = k * x + m;
            setPixel(Point(x, y), color);
        }

        return true;
    }

};


