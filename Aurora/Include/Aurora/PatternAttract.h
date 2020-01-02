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
    Boid *boids[5];
    Attractor attractor;

public:
    PatternAttract() {
        name = (char *)"Attract";
        for (int i = 0; i < count; i++) {
            boids[i] = new Boid();
        }
    }

    virtual ~PatternAttract() {
        for (int i = 0; i < count; i++) {
            delete boids[i];
        }
    }

    void start(Matrix *matrix) {
        int direction = random() % 2;
        if (direction == 0)
            direction = -1;

        for (int i = 0; i < count; i++) {
            Boid *boid = boids[i];
            boid->location.x = matrix->MATRIX_CENTER_X - 1;
            boid->location.y = (matrix->MATRIX_HEIGHT - 1) - i;
            boid->mass = 1; // random(0.1, 2);
            // boid.velocity.x = ((float) random(40, 50)) / 100.0;
            boid->velocity.x = ((float) (random() % 60) + 20) / 100.0;
            boid->velocity.x *= direction;
            boid->velocity.y = 0;
            //boid.colorIndex = i * 32;
            boid->colorIndex = i * (240 / count);
            //dim = random(170, 250);
        }
    }

    unsigned int drawFrame(Effects *effects) {
        Matrix *matrix = effects->matrix;

        // dim all pixels on the display
        uint8_t dim = beatsin8(2, 170, 250);
        effects->DimAll(dim);

        for (int i = 0; i < count; i++) {
            Boid *boid = boids[i];

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
