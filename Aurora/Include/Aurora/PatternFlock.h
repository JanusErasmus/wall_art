/*
 * Aurora: https://github.com/pixelmatix/aurora
 * Copyright (c) 2014 Jason Coon
 *
 * Portions of this code are adapted from "Flocking" in "The Nature of Code" by Daniel Shiffman: http://natureofcode.com/
 * Copyright (c) 2014 Daniel Shiffman
 * http://www.shiffman.net
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

// Flocking
// Daniel Shiffman <http://www.shiffman.net>
// The Nature of Code, Spring 2009

// Demonstration of Craig Reynolds' "Flocking" behavior
// See: http://www.red3d.com/cwr/
// Rules: Cohesion, Separation, Alignment

#ifndef PatternFlock_H
#define PatternFlock_H
#include "Drawable.h"

class PatternFlock : public Drawable {
public:
    PatternFlock(Boid *boids) {
        name = (char *)"Flock";
        this->boids = boids;
    }

    const int boidCount = 7;
    Boid *boids;
    Boid predator;

    PVector wind;
    byte hue = 0;
    bool predatorPresent = true;

    void start(Matrix *matrix) {
        for (int i = 0; i < boidCount; i++) {
            Boid *boid = &boids[i];
            boid->location.x = matrix->MATRIX_CENTER_X;
            boid->location.y = matrix->MATRIX_CENTRE_Y;
            boid->maxspeed = 0.380;
            boid->maxforce = 0.015;
        }


        predator = Boid(matrix->MATRIX_CENTER_X / 2, matrix->MATRIX_CENTER_Y / 2);
        predatorPresent = true;
        predator.maxspeed = 0.385;
        predator.maxforce = 0.020;
        predator.neighbordist = 16.0;
        predator.desiredseparation = 0.0;
    }

    unsigned int drawFrame(Effects *effects) {
        Matrix *matrix = effects->matrix;
        effects->DimAll(230);

        bool applyWind = (random() % 255) > 128;
        if (applyWind) {
            wind.x = Boid::randomf() * .015;
            wind.y = Boid::randomf() * .015;
        }

        CRGB color = effects->ColorFromCurrentPalette(hue);

        for (int i = 0; i < boidCount; i++)
        {
            Boid * boid = &boids[i];

            if (predatorPresent) {
                // flee from predator
                boid->repelForce(predator.location, 10);
            }

            boid->run(boids, boidCount);
            boid->wrapAroundBorders(matrix->MATRIX_WIDTH, matrix->MATRIX_HEIGHT);
            //boid->avoidBorders();
            PVector location = boid->location;
            // PVector velocity = boid->velocity;
            // backgroundLayer.drawLine(location.x, location.y, location.x - velocity.x, location.y - velocity.y, color);
            // effects.leds[XY(location.x, location.y)] += color;
            //backgroundLayer.drawPixel(location.x, location.y, color);
            matrix->drawPixel(location.x, location.y, color);

            if (applyWind) {
                boid->applyForce(wind);
                applyWind = false;
            }
        }

        if (predatorPresent) {
            predator.run(boids, boidCount);
            predator.wrapAroundBorders(matrix->MATRIX_WIDTH, matrix->MATRIX_HEIGHT);
            //predator.avoidBorders();
            color = effects->ColorFromCurrentPalette(hue + 128);
            PVector location = predator.location;

            // PVector velocity = predator.velocity;
            // backgroundLayer.drawLine(location.x, location.y, location.x - velocity.x, location.y - velocity.y, color);
            // effects.leds[XY(location.x, location.y)] += color;
            //backgroundLayer.drawPixel(location.x, location.y, color);
            matrix->drawPixel(location.x, location.y, color);
        }

        static int tick = 0;
        if(tick++ > 20)
        {
            tick = 0;
            hue++;
        }

        matrix->paint();
        return 0;
    }
};

#endif
