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
#ifndef PatternFlowField_H
#define PatternFlowField_H
#include "Drawable.h"
#include "noise.h"

class PatternFlowField : public Drawable {
public:
    PatternFlowField(Effects *effects, Boid *boids) : Drawable(effects) {
        name = (char *)"FlowField";
        this->boids = boids;
    }

    uint16_t x;
    uint16_t y;
    uint16_t z;

    uint16_t speed = 1;
    uint16_t scale = 26;

    static const int count = 5;
    Boid *boids;

    byte hue = 0;

    void start() {
        Matrix *matrix = effects->getMatrix();
        x = random16();
        y = random16();
        z = random16();

        for (int i = 0; i < count; i++) {
            boids[i].location.x = rand() % (matrix->frame_buffer->width);
            boids[i].location.y = rand() % (matrix->frame_buffer->height);
        }
    }

    unsigned int drawFrame() {
        Matrix *matrix = effects->getMatrix();

        effects->DimAll(240);
        // CRGB color = effects.ColorFromCurrentPalette(hue);

        for (int i = 0; i < count; i++) {
            Boid *boid = &boids[i];

            //int ioffset = scale * boid->location.x;
            //int joffset = scale * boid->location.y;

            byte angle = rand() % 255;// inoise8(x + ioffset, y + joffset, z);

            boid->velocity.x = (float) sin8(angle) * 0.0078125 - 1.0;
            boid->velocity.y = -((float)cos8(angle) * 0.0078125 - 1.0);
            boid->update();

            if (
                    boid->location.x < 0 ||
                    boid->location.x >= (matrix->frame_buffer->width) ||
                    boid->location.y < 0 ||
                    boid->location.y >= (matrix->frame_buffer->height)
            ) {
                boid->location.x = rand() % (matrix->frame_buffer->width);
                boid->location.y = rand() % (matrix->frame_buffer->height);
            }

            CRGB color = effects->ColorFromCurrentPalette(effects->ColorFromCurrentPalette(angle + hue));
            matrix->drawPixel(boid->location.x, boid->location.y, color);
        }

        EVERY_N_MILLIS(200) {
            hue++;
        }

        x += speed;
        y += speed;
        z += speed;

        matrix->paint();

        return 50;
    }
};

#endif
