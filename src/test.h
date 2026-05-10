
#include <pico/stdlib.h>

#include "image.h"

inline void testSetPixel(Image screen) {

    for (size_t x = 25; x < 50; x++) for (size_t y = 25; y < 50; y++) {
         
        screen.setPixel(Point(x, y), COLOR_GREEN);
        sleep_us(5'000);

    }
}

inline void testPolygon(Image screen) {

    Point polygon[] = {
        Point(100,               25          ),
        Point(200,               55  ),
        Point(180,               80),
        Point(180,               60),
        Point(screen.width - 25, 30          ),
        Point(screen.width - 25, screen.height - 25),
        Point(100,               screen.height - 25),
        Point(100,               50  ),
        Point(120,               140 )
    };
    int len = sizeof(polygon)/sizeof(Point);

    screen.drawLines(polygon, len, 5, COLOR_GREEN);
}

inline void testDrawRect(Image screen) {
    
    Rect rect = Rect(Point(25, 75), Point(25, 25));
    screen.drawRect(rect, COLOR_GREEN);
}

inline void testDrawImage(Image screen) {

    Image fillImg = Image(Point(25, 25), COLOR_INVERT);
    screen.applyImage(fillImg, Point(25, 100));
}