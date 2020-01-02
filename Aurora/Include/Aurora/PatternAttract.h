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

#ifndef PatternAttract_H
#define PatternAttract_H

#include "Drawable.h"
#include "Attractor.h"

class PatternAttract : public Drawable {
private:
    const int count = 5;
    Boid *boids;
    Attractor attractor;

public:
    PatternAttract(Effects *effects, Boid *boids) : Drawable(effects) {
        name = (char *)"Attract";
        this->boids = boids;
    }

    virtual ~PatternAttract() {
    }

    void start() {
        int direction = rand() % 2;
        if (direction == 0)
            direction = -1;

        Matrix *matrix = effects->getMatrix();
        int MATRIX_CENTER_X = (matrix->frame_buffer->width / 2) - 1;

        for (int i = 0; i < count; i++) {
            Boid *boid = &boids[i];
            boid->location.x = MATRIX_CENTER_X - 1;
            boid->location.y = (matrix->frame_buffer->height - 1) - i;
            boid->mass = 1; // rand(0.1, 2);
            boid->velocity.x = ((float) (rand() % 50) + 20) / 100.0;
            boid->velocity.x *= direction;
            boid->velocity.y =  0;
            boid->maxspeed = 1.5;
            boid->maxforce = 0.05;
            boid->desiredseparation = 4;
            boid->neighbordist = 8;
            boid->colorIndex = i * (240 / count);
        }
    }

    unsigned int drawFrame() {
        Matrix *matrix = effects->getMatrix();

        // dim all pixels on the display
        uint8_t dim = beatsin8(2, 170, 250);
        effects->DimAll(dim);

        for (int i = 0; i < count; i++) {
            Boid *boid = &boids[i];

            PVector force = attractor.attract(boid);
            boid->applyForce(force);

            boid->update();

            CRGB color = effects->ColorFromCurrentPalette(boid->colorIndex);
            matrix->drawPixel(boid->location.x, boid->location.y, color);
        }

        matrix->paint();
        return 15;
    }
};

#endif