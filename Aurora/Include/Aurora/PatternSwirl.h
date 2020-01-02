/*
 * Aurora: https://github.com/pixelmatix/aurora
 * Copyright (c) 2014 Jason Coon
 *
 * Portions of this code are adapted from SmartMatrixSwirl by Mark Kriegsman: https://gist.github.com/kriegsman/5adca44e14ad025e6d3b
 * https://www.youtube.com/watch?v=bsGBT-50cts
 * Copyright (c) 2014 Mark Kriegsman
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
#ifndef PatternSwirl_H
#define PatternSwirl_H
#include "Drawable.h"

class PatternSwirl : public Drawable {
private:
    const uint8_t borderWidth = 0;

public:
    PatternSwirl(Effects *effects) : Drawable(effects) {
        name = (char *)"Swirl";
    }

    void start() {
    }

    unsigned int drawFrame() {
        // Apply some blurring to whatever's already on the matrix
        // Note that we never actually clear the matrix, we just constantly
        // blur it repeatedly.  Since the blurring is 'lossy', there's
        // an automatic trend toward black -- by design.
        uint8_t blurAmount = beatsin8(2, 128, 255);
        Matrix *matrix = effects->getMatrix();

        effects->DimAll(blurAmount);

        // Use two out-of-sync sine waves
        uint8_t  i = beatsin8(27, borderWidth, matrix->frame_buffer->height - borderWidth);
        uint8_t  j = beatsin8(41, borderWidth, matrix->frame_buffer->width - borderWidth);
        // Also calculate some reflections
        uint8_t ni = (matrix->frame_buffer->width - 1) - i;
        uint8_t nj = (matrix->frame_buffer->width - 1) - j;

        int NUMMATRIX = matrix->frame_buffer->size;

        // The color of each point shifts over time, each at a different speed.
        uint16_t ms = HAL_GetTick();
        CRGB *leds = matrix->frame_buffer->buffer;
        leds[effects->XY(i, j)  ] += effects->XY(i, j)  == NUMMATRIX-1 ? CRGB::Black : effects->ColorFromCurrentPalette(ms / 11);
        leds[effects->XY(j, i)  ] += effects->XY(j, i)  == NUMMATRIX-1 ? CRGB::Black : effects->ColorFromCurrentPalette(ms / 13);
        leds[effects->XY(ni, nj)] += effects->XY(ni, nj)== NUMMATRIX-1 ? CRGB::Black : effects->ColorFromCurrentPalette(ms / 17);
        leds[effects->XY(nj, ni)] += effects->XY(nj, ni)== NUMMATRIX-1 ? CRGB::Black : effects->ColorFromCurrentPalette(ms / 29);
        leds[effects->XY(i, nj) ] += effects->XY(i, nj) == NUMMATRIX-1 ? CRGB::Black : effects->ColorFromCurrentPalette(ms / 37);
        leds[effects->XY(ni, j) ] += effects->XY(ni, j) == NUMMATRIX-1 ? CRGB::Black : effects->ColorFromCurrentPalette(ms / 41);

        matrix->paint();
        return 0;
    }
};

#endif