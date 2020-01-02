/*
 * Aurora: https://github.com/pixelmatix/aurora
 * Copyright (c) 2014 Jason Coon
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
#ifndef PatternRadar_H
#define PatternRadar_H
#include "Drawable.h"

class PatternRadar : public Drawable {
private:
    byte theta = 0;
    byte hueoffset = 0;

public:
    PatternRadar(Effects *effects) : Drawable(effects) {
        name = (char *)"Radar";
    }

    unsigned int drawFrame() {
        Matrix *matrix = effects->getMatrix();
        if (matrix->frame_buffer->height < 25) {
            effects->DimAll(252);
        } else {
            effects->DimAll(245);
        }

        int MATRIX_CENTER_X = (matrix->frame_buffer->width / 2) - 1;

        for (int offset = 0; offset < MATRIX_CENTER_X; offset++) {

            byte hue = 255 - (offset * (256 / MATRIX_CENTER_X) + hueoffset);
            CRGB color = effects->ColorFromCurrentPalette(hue);
            uint8_t x = mapcos8(theta, offset, (matrix->frame_buffer->width - 1) - offset);
            uint8_t y = mapsin8(theta, offset, (matrix->frame_buffer->height - 1) - offset);
            uint16_t xy = effects->XY(x, y);

            CRGB *leds = matrix->frame_buffer->buffer;
            leds[xy] = color;

            EVERY_N_MILLIS(25) {
                theta += 2;
                hueoffset += 1;
            }
        }

        matrix->paint();
        return 0;
    }
};

#endif
