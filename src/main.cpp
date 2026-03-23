

#include <pico/stdlib.h>
#include <pico/time.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "image.h"
#include "driver/lcd.h"

const int image_w = 320;
const int image_h = 240;
uint8_t frameBuffer[LCD_WIDTH * LCD_HEIGHT] = {0};

int main() {

   stdio_init_all();
   printf("Hello, Pico!\n");


   lcd_init((uint32_t*)frameBuffer, 3, pio0);
   Image screen = Image(frameBuffer, nullptr, LCD_WIDTH * 8, LCD_HEIGHT);


   while (true) {
      
      // for (int i = 0; i < LCD_HEIGHT; i++) {
      //    frameBuffer[LCD_WIDTH * i + 12] ^= 0x01;
      // }
      for (int i = 0; i < LCD_HEIGHT; i++) {
         for (int offset=0; offset < 10; offset++) {
            screen.setPixel(Point(i + offset, i), COLOR_GREEN);
         }
      }
      Line line = Line(25, 25, 200, 100);
      screen.drawLine(line, COLOR_GREEN);
      // printf("%u\n", LCD_WIDTH * 30 + 30/8);
      // printf("0x%02X at x: %u y: %u\n", frameBuffer[LCD_WIDTH * 30 + 30/8], 30/8, 30);
      sleep_us(1000000);
   }
  
}

 