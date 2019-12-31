// By Marc MERLIN <marc_soft@merlins.org>
// License: Apache v2.0
//

#ifndef matrix_h
#define matrix_h

#include "Adafruit_GFX.h"
#include "Drawable.h"
#include "pixeltypes.h"

#define MATRIX_WIDTH 16
#define MATRIX_HEIGHT 16
#define NUM_LEDS 16
#define NUMMATRIX 256

typedef bool boolean;
typedef uint8_t byte;

const int MATRIX_CENTER_X = MATRIX_WIDTH / 2;
const int MATRIX_CENTER_Y = MATRIX_HEIGHT / 2;

const byte MATRIX_CENTRE_X = MATRIX_CENTER_X - 1;
const byte MATRIX_CENTRE_Y = MATRIX_CENTER_Y - 1;

#define mmin(a,b) (a<b)?(a):(b)
#define mmax(a,b) (a>b)?(a):(b)


class Matrix : public Adafruit_GFX
{
    CRGB framebuffer[16][16];

//    void writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
//    void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
//    void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);

public:
    Matrix();
    virtual ~Matrix(){}

    void drawPixel(int x, int y, CRGB color);
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    void clear();
    void paint();

    CRGB *getBuffer(){ return (CRGB*)&framebuffer[0][0]; }

    //void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
    void setPassThruColor(uint16_t){}
    void setPassThruColor(){}
    uint16_t XY(int x, int y){ return x + y * MATRIX_WIDTH; }
};

extern Matrix *matrix;

#endif

