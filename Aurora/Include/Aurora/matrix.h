#ifndef matrix_h
#define matrix_h
#include "pixeltypes.h"

typedef bool boolean;
typedef uint8_t byte;

#define mmin(a,b) (a<b)?(a):(b)
#define mmax(a,b) (a>b)?(a):(b)


class Matrix
{
protected:

public:
    CRGB *framebuffer;
//    CRGB framebuffer[16][16];
    int MATRIX_WIDTH;
    int MATRIX_HEIGHT;
    int NUM_LEDS;
    int NUMMATRIX;
    int MATRIX_CENTER_Y;
    int MATRIX_CENTER_X;
    byte MATRIX_CENTRE_X;
    byte MATRIX_CENTRE_Y;

    Matrix(int width, int height);
    virtual ~Matrix(){}

    virtual void drawPixel(int x, int y, CRGB color) = 0;
    virtual void drawPixel(int16_t x, int16_t y, uint16_t color) = 0;
    virtual void clear() = 0;
    virtual void paint() = 0;


    uint16_t XY(int x, int y){ return x + y * MATRIX_WIDTH; }
};

#endif

