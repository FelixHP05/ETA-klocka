
#include <pico/stdlib.h>

#include "image/image.h"
#include "images.h"


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
        Point(screen.getHeight() - 25, 30          ),
        Point(screen.getWidth() - 25, screen.getHeight() - 25),
        Point(100,               screen.getHeight() - 25),
        Point(100,               50  ),
        Point(120,               140 )
    };
    int len = sizeof(polygon)/sizeof(Point);

    screen.drawLines(polygon, len, 5, COLOR_GREEN);
}

inline void testDrawRect(Image screen) {
    
    Rect rect1 = Rect(Point(25,51), Point(25, 2));
    screen.drawRect(rect1, COLOR_GREEN);

    Rect rect = Rect(Point(25, 75), Point(25, 25));
    screen.drawRect(rect, COLOR_GREEN);
}

inline void testDrawImage(Image screen) {


    Image fillImg = Image(Point(8,2), COLOR_GREEN);

    screen.applyImage(fillImg, Point(33, 125));

    screen.applyImage(fillImg, Point(32, 130));


}

inline void runTests(Image screen) {
    
    
    //testSetPixel(screen);
    //testPolygon(screen);

    //testDrawRect(screen);
    //testDrawImage(screen);

    screen.applyImage(fontUpperÖ, Point(20, 20));
}