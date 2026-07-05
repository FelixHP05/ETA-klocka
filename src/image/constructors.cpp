
#include "image.h"

 
    Image::Image(AlphaMode alphaMode, uint8_t* greenBuffer, uint8_t* alphaBuffer, size_t width, size_t height) :
        alphaMode(alphaMode),
        greenBuffer(greenBuffer),
        alphaBuffer(alphaBuffer),
        width(ceilDiv(width, 8) * 8), // round up to multiple of 8
        height(height)
    {};

    Image::Image(AlphaMode alphaMode, uint8_t* greenBuffer, uint8_t* alphaBuffer, Point size) :
        Image::Image(alphaMode, greenBuffer, alphaBuffer, size.x, size.y)
    {};

    Image::Image(Point size, Color fill) { 
        size_t bufWidth = ceilDiv(size.x, 8) * 8;   // round up to multiple of 8

        this->height = size.y;
        this->width = bufWidth;
        this->ownsBuffers = true;
        this->alphaMode = AlphaMode::BUFFER;

        this->greenBuffer = (uint8_t*) malloc(bufWidth/8 * height);
        this->alphaBuffer = (uint8_t*) malloc(bufWidth/8 * height);

        drawRect(Rect(0,      0, size.x,   size.y), fill              );
        drawRect(Rect(size.x, 0, bufWidth, size.y), COLOR_TRANSPARENT );
    };

    Image::Image(const Image& other) : 
        greenBuffer(other.greenBuffer),
        alphaBuffer(other.alphaBuffer),
        width(other.width),
        height(other.height),
        alphaMode(other.alphaMode),
        ownsBuffers(false)
    {}; 

    Image& Image::operator=(const Image& other) {

        if (this == &other) return *this;
        if (this->ownsBuffers ) { 
            free(alphaBuffer); 
            free(greenBuffer); 
        }

        this->greenBuffer = other.greenBuffer;
        this->alphaBuffer = other.alphaBuffer;
        this->width = other.width;
        this->height = other.height;
        this->alphaMode = other.alphaMode;
        this->ownsBuffers = false;
        return *this;
    };

    Image::Image(Image&& other) : 
        greenBuffer(other.greenBuffer),
        alphaBuffer(other.alphaBuffer),
        width(other.width),
        height(other.height),
        alphaMode(other.alphaMode),
        ownsBuffers(other.ownsBuffers)
    {other.ownsBuffers = false;};      

    Image& Image::operator=(Image&& other) {

        if (this == &other) return *this;

        // free your old buffers
        if (this->ownsBuffers ) {
            free(alphaBuffer);
            free(greenBuffer);
        }
        
        this->greenBuffer = other.greenBuffer;
        this->alphaBuffer = other.alphaBuffer;
        this->width = other.width;
        this->height = other.height;
        this->alphaMode = other.alphaMode;

        // steal ownership
        this->ownsBuffers = other.ownsBuffers;
        other.ownsBuffers = false;
        return *this;
    }; 
    Image::~Image() {
        if (ownsBuffers) { 
            free(alphaBuffer); 
            free(greenBuffer); 
        }
    };