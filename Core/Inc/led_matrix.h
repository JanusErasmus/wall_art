#ifndef led_matrix_h
#define led_matrix_h

#include "Aurora/matrix.h"

class LEDmatrix : public Matrix
{

public:
    LEDmatrix();
    virtual ~LEDmatrix(){}

    void drawPixel(int x, int y, CRGB color);
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    void clear();
    void paint();
};

#endif

