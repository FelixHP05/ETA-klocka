#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdexcept>

#include "../generic.h"
#include "drawing.h"

// color masking
uint8_t greenMask(Color source, Color target) { return source.green ^ (source.alpha & target.green); }
uint8_t alphaMask(Color source, Color target) { return source.alpha & target.alpha; }
uint8_t alphaMask(uint8_t sourceAlpha, uint8_t targetAlpha) { return sourceAlpha & targetAlpha; }
Color colorMask(Color source, Color target) {
    return (Color){
        greenMask(source, target),
        alphaMask(source, target)
    };
}