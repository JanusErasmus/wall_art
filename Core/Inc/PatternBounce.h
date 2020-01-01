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

#ifndef PatternBounce_H
#define PatternBounce_H

#include "Drawable.h"
#include "Boid.h"
#include "colorutils.h"

class PatternBounce : public Drawable {
private:
    static const int count = 16;
    Boid *boids[16];
    PVector gravity = PVector(0, 0.0125);

public:
    PatternBounce(Effects *effects) : Drawable(effects) {
        name = (char *)"Bounce";
        for (int i = 0; i < count; i++) {
            boids[i] = new Boid();
        }
    }

    virtual ~PatternBounce() {
        for (int i = 0; i < count; i++) {
            delete boids[i];
        }
    }

    void start() {
        unsigned int colorWidth = 256 / count;
        for (int i = 0; i < count; i++) {
            Boid *boid = boids[i];
            boid->location.x = i;
            boid->location.y = 0;
            boid->velocity.x = 0;// 0.02;
            boid->velocity.y = i * -0.01;
            boid->colorIndex = colorWidth * i;
            boid->maxforce = 10;
            boid->maxspeed = 10;
        }
    }

    unsigned int drawFrame()
    {
        if(!effects)
            return 0;

        Matrix *matrix = effects->getMatrix();

        // dim all pixels on the display
        effects->DimAll(230);

        for (int i = 0; i < count; i++)
        {
            Boid *boid = boids[i];

            boid->applyForce(gravity);

            float oldY = boid->location.y;
            boid->update();

            CRGB color = effects->ColorFromCurrentPalette(boid->colorIndex);
            if(boid->location.y > oldY)
            {
                for(float y = oldY; y < boid->location.y; y++)
                {
                    matrix->drawPixel(boid->location.x, y, color);
                }
            }
            else
            {
                for(float y = boid->location.y; y < oldY; y++)
                {
                    matrix->drawPixel(boid->location.x, y, color);
                }
            }

            // drawPixel takes care of it
            matrix->drawPixel(boid->location.x, boid->location.y, color);

            if (boid->location.y >= MATRIX_HEIGHT - 1)
            {
                boid->location.y = MATRIX_HEIGHT - 1;
                boid->velocity.y *= -1.0;
            }
        }

        matrix->paint();
        return 15;
    }
};

#endif
