

#include <pico/stdlib.h>
#include <pico/time.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "test.h"
#include "image.h"
#include "driver/lcd.h"
#include "tmp.h"


int main() {

    // init uart
    stdio_init_all();
    sleep_ms(2000);
    printf("Hello, Pico!\n");

    // init screen
    const int image_w = LCD_WIDTH * 8;
    const int image_h = LCD_HEIGHT;
    uint8_t frameBuffer[LCD_WIDTH * LCD_HEIGHT] = {0};
    Image screen = Image(frameBuffer, nullptr, image_w, image_h);

    // init lcd
    lcd_init((uint32_t*)frameBuffer, 3, pio0);



    while (true) {
        printf("hi :)\n");
        sleep_ms(1000);
        screen.applyImage(a, Point(100, 100));
        
        runTests(screen);
    }






}

 