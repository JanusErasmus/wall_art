#ifndef INC_LDE_MATRIX_H_
#define INC_LDE_MATRIX_H_
#include "Aurora/matrix.h"

class LEDmatrix : public Matrix
{
public:
    LEDmatrix();


    void drawPixel(int x, int y, CRGB color);
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    void clear();
    void paint();
};


#endif /* INC_LDE_MATRIX_H_ */
