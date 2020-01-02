#ifndef matrix_h
#define matrix_h
#include <stdlib.h>

#include "pixeltypes.h"

#define NUM_LEDS 16

typedef bool boolean;
typedef uint8_t byte;

#define mmin(a,b) (a<b)?(a):(b)
#define mmax(a,b) (a>b)?(a):(b)

class FrameBuffer
{
public:
    CRGB *buffer;
    int width;
    int height;
    int size;
    FrameBuffer(int width, int height)
    {
        this->width = width;
        this->height = height;
        this->size = width * height;
        buffer = (CRGB*)malloc(sizeof(CRGB) * width * height);
    }


};

class Matrix
{
    int MATRIX_WITDH;
    int MATRIX_HEIGHT;
    int MATRIX_CENTER_X;
    int MATRIX_CENTER_Y;

protected:
    CRGB *getPixel(int x, int  y);

public:
    FrameBuffer *frame_buffer;
    Matrix(FrameBuffer *frame_buffer);
    virtual ~Matrix(){}

    virtual void drawPixel(int x, int y, CRGB color) = 0;
    virtual void drawPixel(int16_t x, int16_t y, uint16_t color) = 0;
    virtual void clear(){};
    virtual void paint(){};

    virtual void setPassThruColor(uint16_t){}
    virtual void setPassThruColor(){}
    uint16_t XY(int x, int y){ return x + y * frame_buffer->width; }
};

#endif

