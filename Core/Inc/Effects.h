/*
 * Aurora: https://github.com/pixelmatix/aurora
 * Copyright (c) 2014 Jason Coon
 *
 * Portions of this code are adapted from "Funky Clouds" by Stefan Petrick: https://gist.github.com/anonymous/876f908333cd95315c35
 * Portions of this code are adapted from "NoiseSmearing" by Stefan Petrick: https://gist.github.com/StefanPetrick/9ee2f677dbff64e3ba7a
 * Copyright (c) 2014 Stefan Petrick
 * http://www.stefan-petrick.de/wordpress_beta
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef Effects_H
#define Effects_H
#include <String>
#include <stdint.h>
#include <Utils/utils.h>

#include "matrix.h"
#include "colorutils.h"


#define MAX_COLOR_VALUE 255

#define SWAPint(X,Y) { \
        int temp = X ; \
        X = Y ; \
        Y = temp ; \
}


class Effects {
    Matrix *matrix;
    static const int paletteCount = 10;
    TBlendType currentBlendType = LINEARBLEND;
    int paletteIndex;
    const CRGBPalette16 *currentPalette;
    char* currentPaletteName;


    uint32_t noise_x;
    uint32_t noise_y;
    uint32_t noise_z;
    uint32_t noise_scale_x;
    uint32_t noise_scale_y;
    uint8_t noise[MATRIX_WIDTH][MATRIX_HEIGHT];
    uint8_t noisesmoothing;

    // the oscillators: linear ramps 0-255
    byte osci[6];

    // sin8(osci) swinging between 0 to MATRIX_WIDTH - 1
    byte p[6];

public:
    CRGB *leds;

    Effects(Matrix *matrix);

    Matrix *getMatrix(){ return matrix; }

    void CircleStream(uint8_t value);

    // palettes
    static const int HeatColorsPaletteIndex = 6;
    static const int RandomPaletteIndex = 9;

    void CyclePalette();
    void RandomPalette();
    void loadPalette(int index);
    void setupGrayscalePalette();
    void setupIcePalette();

    void drawForegroundHLine(int16_t x0, int16_t x1, int16_t y);
    void fillForegroundRectangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1);
    uint16_t XY( uint8_t x, uint8_t y);


    // Oscillators and Emitters
    // set the speeds (and by that ratios) of the oscillators here
    void MoveOscillators();

    // scale the brightness of the frame buffer down
    void DimAll(byte value);

    // All the caleidoscope functions work directly within the frame buffer (LED's array).
    // Draw whatever you like in the area x(0-15) and y (0-15) and then copy it around.
    // rotates the first 16x16 quadrant 3 times onto a 32x32 (+90 degrees rotation for each one)
    void Caleidoscope1();

    // mirror the first 16x16 quadrant 3 times onto a 32x32
    void Caleidoscope2();

    // copy one diagonal triangle into the other one within a 16x16
    void Caleidoscope3();

    // copy one diagonal triangle into the other one within a 16x16 (90 degrees rotated compared to Caleidoscope3)
    void Caleidoscope4();

    // copy one diagonal triangle into the other one within a 8x8
    void Caleidoscope5();
    void Caleidoscope6();

    // create a square twister to the left or counter-clockwise
    // x and y for center, r for radius
    void SpiralStream(int x, int y, int r, byte dimm);

    // expand everything within a circle
    void Expand(int centerX, int centerY, int radius, byte dimm);

    // give it a linear tail to the right
    void StreamRight(byte scale, int fromX = 0, int toX = MATRIX_WIDTH, int fromY = 0, int toY = MATRIX_HEIGHT);

    // give it a linear tail to the left
    void StreamLeft(byte scale, int fromX = MATRIX_WIDTH, int toX = 0, int fromY = 0, int toY = MATRIX_HEIGHT);

    // give it a linear tail downwards
    void StreamDown(byte scale);

    // give it a linear tail upwards
    void StreamUp(byte scale);

    // give it a linear tail up and to the left
    void StreamUpAndLeft(byte scale);

    // give it a linear tail up and to the right
    void StreamUpAndRight(byte scale);

    // just move everything one line down
    void MoveDown();

    // just move everything one line down
    void VerticalMoveFrom(int start, int end);

    // copy the rectangle defined with 2 points x0, y0, x1, y1
    // to the rectangle beginning at x2, x3
    void Copy(byte x0, byte y0, byte x1, byte y1, byte x2, byte y2);

    // rotate + copy triangle (MATRIX_CENTER_X*MATRIX_CENTER_X)
    void RotateTriangle();

    // mirror + copy triangle (MATRIX_CENTER_X*MATRIX_CENTER_X)
    void MirrorTriangle();

    // draw static rainbow triangle pattern (MATRIX_CENTER_XxWIDTH / 2)
    // (just for debugging)
    void RainbowTriangle();

    void BresenhamLine(int x0, int y0, int x1, int y1, CRGB color);
    void BresenhamLine(int x0, int y0, int x1, int y1, byte colorIndex);

    // write one pixel with the specified color from the current palate to coordinates
    void Pixel(int x, int y, uint8_t colorIndex);

    CRGB ColorFromCurrentPalette(uint8_t index = 0, uint8_t brightness = 255, TBlendType blendType = LINEARBLEND);
    CRGB HsvToRgb(uint8_t h, uint8_t s, uint8_t v);

    void NoiseVariablesSetup();
    void FillNoise();
    void standardNoiseSmearing();

    void MoveX(byte delta);
    void MoveY(byte delta);

    void MoveFractionalNoiseX(byte amt = 16);
    void MoveFractionalNoiseY(byte amt = 16);

};

#endif
