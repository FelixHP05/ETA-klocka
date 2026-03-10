#ifndef LCD_H
#define LCD_H

#include <hardware/pio.h>

#include "stdint.h"

/* LCD size, height in pixels, width in bytes */
#define LCD_WIDTH (320 / 8)
#define LCD_HEIGHT (240)

void lcd_init(uint32_t *buf, char pin, PIO pio);

#endif
