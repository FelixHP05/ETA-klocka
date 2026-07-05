

#include <pico/stdlib.h>
#include <pico/time.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "test.h"
#include "image/image.h"
#include "driver/lcd.h"

uint8_t frameBuffer[LCD_WIDTH * LCD_HEIGHT] = {0};
int main() {

    // init uart
    stdio_init_all();
    sleep_ms(2000);
    printf("Hello, Pico!\n");

    // init screen
    Image screen = Image(AlphaMode::OPAQUE, frameBuffer, nullptr, 8*LCD_WIDTH, LCD_HEIGHT);

    // init lcd
    lcd_init((uint32_t*)frameBuffer, 3, pio0);



    while (true) {
        printf("hi :)\n");
        sleep_ms(1000);
        
        runTests(screen);
    }






}

 