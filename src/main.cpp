

#include <pico/stdlib.h>
#include <pico/time.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "test.h"
#include "image.h"
#include "driver/lcd.h"



int main() {

   const int image_w = LCD_WIDTH * 8;
   const int image_h = LCD_HEIGHT;
   uint8_t frameBuffer[LCD_WIDTH * LCD_HEIGHT] = {0};

   stdio_init_all();
   printf("Hello, Pico!\n");


   lcd_init((uint32_t*)frameBuffer, 3, pio0);
   Image screen = Image(frameBuffer, nullptr, image_w, image_h);

   while (true) {
      printf(".\n");
      
      testSetPixel(screen);
      testPolygon(screen);

      testDrawRect(screen);
      testDrawImage(screen);
   }

   // float angle = 0;
   // while (true) {

   //    Line line = Line(
   //       50,         image_h/2 + 100*cos(angle+PI*2/3),
   //       image_w-50, image_h/2 + 100*cos(angle       )
   //    );

   //    screen.fill(COLOR_BLACK);
   //    screen.drawLine(line, 2, COLOR_GREEN);
      
   //    angle = modulo(angle+0.05, 2*PI);
   //    sleep_us(10'000);
   // }
   // screen.drawLines(polygon, sizeof(polygon)/sizeof(Point), 2, COLOR_GREEN);
   



  
}

 