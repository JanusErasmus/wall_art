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
#ifndef PatternSpiro_H
#define PatternSpiro_H
#include "Drawable.h"

class PatternSpiro : public Drawable {
private:
    byte theta1 = 0;
    byte theta2 = 0;
    byte hueoffset = 0;

    uint8_t radiusx;
    uint8_t radiusy;
    uint8_t minx;
    uint8_t maxx;
    uint8_t miny;
    uint8_t maxy;

    uint8_t spirocount = 2;
    uint8_t spirooffset = 256 / spirocount;
    boolean spiroincrement = false;

    boolean handledChange = false;

public:
    PatternSpiro(Effects *effects) : Drawable(effects) {
        name = (char *)"Spiro";

        Matrix *matrix = effects->getMatrix();

        int MATRIX_CENTER_X = (matrix->frame_buffer->width / 2) - 1;
        int MATRIX_CENTER_Y = (matrix->frame_buffer->height / 2) - 1;
        radiusx = matrix->frame_buffer->width / 4;
        radiusy = matrix->frame_buffer->height / 4;
        minx = MATRIX_CENTER_X - radiusx;
        maxx = MATRIX_CENTER_X + radiusx + 1;
        miny = MATRIX_CENTER_Y - radiusy;
        maxy = MATRIX_CENTER_Y + radiusy + 1;
    }

    unsigned int drawFrame() {
        Matrix *matrix = effects->getMatrix();
        effects->DimAll(254);

        int MATRIX_CENTER_X = (matrix->frame_buffer->width / 2) - 1;
        int MATRIX_CENTER_Y = (matrix->frame_buffer->height / 2) - 1;
        boolean change = false;

        for (int i = 0; i < spirocount; i++) {
            uint8_t x = mapsin8(theta1 + i * spirooffset, minx, maxx);
            uint8_t y = mapcos8(theta1 + i * spirooffset, miny, maxy);

            uint8_t x2 = mapsin8(theta2 + i * spirooffset, x - radiusx, x + radiusx);
            uint8_t y2 = mapcos8(theta2 + i * spirooffset, y - radiusy, y + radiusy);

            // Calling Colorfromcurrentpalette breaks the pattern in that it stops half way into a ball
            CRGB color = effects->ColorFromCurrentPalette(hueoffset + i * spirooffset, 255);
            //uint32_t color = Wheel(hueoffset + i * spirooffset);
            //effects.leds[XY(x2, y2)] += color;
            // Some coordinates are off screen, using drawpixel avoids the artifact of overwriting pixel0
            matrix->drawPixel(x2, y2, color);
            // On bigger displays, make the dot faster
            if (matrix->frame_buffer->width > 32) {
                matrix->drawPixel(x2-1, y2-0, color);
                matrix->drawPixel(x2-1, y2-1, color);
                matrix->drawPixel(x2-0, y2-1, color);

                matrix->drawPixel(x2-1, y2+1, color);
                matrix->drawPixel(x2+1, y2-1, color);

                matrix->drawPixel(x2+1, y2+0, color);
                matrix->drawPixel(x2+1, y2+1, color);
                matrix->drawPixel(x2+0, y2+1, color);
            }

            if((x2 == MATRIX_CENTER_X && y2 == MATRIX_CENTER_Y) ||
                    (x2 == MATRIX_CENTER_X && y2 == MATRIX_CENTER_Y)) change = true;
        }

        theta2 += 1;

        EVERY_N_MILLIS(25) {
            theta1 += 1;
        }

        EVERY_N_MILLIS(100) {
            if (change && !handledChange) {
                handledChange = true;

                if (spirocount >= matrix->frame_buffer->width || spirocount == 1) spiroincrement = !spiroincrement;

                if (spiroincrement) {
                    if(spirocount >= 4)
                        spirocount *= 2;
                    else
                        spirocount += 1;
                }
                else {
                    if(spirocount > 4)
                        spirocount /= 2;
                    else
                        spirocount -= 1;
                }

                spirooffset = 256 / spirocount;
            }

            if(!change) handledChange = false;
        }

        EVERY_N_MILLIS(33) {
            hueoffset += 1;
        }

        matrix->paint();


        return 0;
    }
};

#endif