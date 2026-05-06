

#include <pico/stdlib.h>
#include <pico/time.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "image.h"
#include "driver/lcd.h"

const int image_w = LCD_WIDTH * 8;
const int image_h = LCD_HEIGHT;
uint8_t frameBuffer[LCD_WIDTH * LCD_HEIGHT] = {0};

int main() {

   stdio_init_all();
   printf("Hello, Pico!\n");


   lcd_init((uint32_t*)frameBuffer, 3, pio0);
   Image screen = Image(frameBuffer, nullptr, image_w, image_h);

   Point polygon[] = {
      Point(25,           25          ),
      Point(image_w - 25, 25          ),
      Point(image_w - 25, image_h - 25),
      Point(25,           image_h - 25),
      Point(25,   50),
      Point(100,  40),
      Point(120, 140)

   };

   float angle = 0;
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
   
   screen.drawLine( Line(polygon[1],polygon[2]), 2, COLOR_GREEN );

   
   Image square = Image(Point(25,25), COLOR_INVERT);
   screen.drawImage(square, Point(25,25));

   while (true) {
      
      sleep_us(10'000);
   }
  
}

 