#include "Aurora/Effects.h"
#include "Aurora/hsv2rgb.h"
#include "Aurora/noise.h"
#include "Aurora/colorpalettes.h"

#include "Aurora/animate_disco.h"
#include "Aurora/PatternBounce.h"
#include "Aurora/PatternFlock.h"
#include "Aurora/PatternSwirl.h"
#include "Aurora/PatternSpiral.h"
#include "Aurora/PatternSpiro.h"
#include "Aurora/PatternWave.h"
#include "Aurora/PatternRadar.h"
#include "Aurora/PatternPendulumWave.h"
#include "Aurora/PatternIncrementalDrift.h"
#include "Aurora/PatternFlowField.h"
#include "Aurora/PatternAttract.h"
#include "Aurora/PatternCube.h"
#include "Aurora/Effects.h"

struct pallette_s
{
    const char *name;
    const CRGBPalette16 *palette;
};

const pallette_s palettes[] = {
        {"Rainbow", &RainbowColors_p},
        {"RainStripe", &RainbowStripeColors_p},
        {"Cloud", &CloudColors_p},
        {"Lava", &LavaColors_p},
        {"Ocean", &OceanColors_p},
        {"Forest", &ForestColors_p},
        {"Party", &PartyColors_p},
        {"Heat", &HeatColors_p},
        {"Wood", &WoodFireColors_p},
        {"Sodium", &SodiumFireColors_p},
        {"Copper", &CopperFireColors_p},
        {"Alcohol", &AlcoholFireColors_p},
        {"Rubidium", &RubidiumFireColors_p},
        {"Potassium", &PotassiumFireColors_p},
        {"Lithium", &LithiumFireColors_p},
        {"Ice", &IceColors_p},
        {0, 0}
};

Boid boids[16];

AnimateDisco disco;
PatternBounce bounce(boids);
PatternFlock flock(boids);
PatternSwirl swirl;
PatternSpiral spiral;
PatternSpiro spiro;
PatternWave wave;
PatternRadar radar;
PatternPendulumWave p_wave;
PatternIncrementalDrift drift;
PatternFlowField flow(boids);
PatternAttract attract(boids);
PatternCube cube;

Drawable *patterns[] = {
        &bounce,
        &flock,
        &flow,
        &swirl,
        &spiral,
        &spiro,
        &wave,
        &radar,
        &cube,
        //&p_wave,
        &drift,
        &attract,
        &disco,
        0
};

Effects::Effects(Matrix *matrix)
{
    curr_pattern = 0;
    paletteIndex = 0;
    this->matrix = matrix;
    leds = matrix->framebuffer;// &matrix->framebuffer[0][0];
    currentPalette = &RainbowColors_p;

    patterns[curr_pattern]->start(matrix);
}


void Effects::setNextPattern()
{
    curr_pattern++;
    if(!patterns[curr_pattern])
        curr_pattern = 0;

    printf("Starting pattern: %s\n", patterns[curr_pattern]->name);
    patterns[curr_pattern]->start(matrix);
}

void Effects::run()
{
    patterns[curr_pattern]->drawFrame(this);
}

void Effects::drawForegroundHLine(int16_t x0, int16_t x1, int16_t y) {
    // make sure line goes from x0 to x1
    if (x1 < x0)
        SWAPint(x1, x0);

    // check for completely out of bounds line
    if (x1 < 0 || x0 >= matrix->MATRIX_WIDTH || y < 0 || y >= matrix->MATRIX_HEIGHT)
        return;

    // truncate if partially out of bounds
    if (x0 < 0)
        x0 = 0;

    if (x1 >= matrix->MATRIX_WIDTH)
        x1 = matrix->MATRIX_WIDTH - 1;

    int i;

    for (i = x0; i <= x1; i++) {
        matrix->drawPixel((int16_t)i, (int16_t)y, (uint16_t)1);
    }
}

void Effects::fillForegroundRectangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
    int i;
    // Loop only works if y1 > y0
    if (y0 > y1) {
        SWAPint(y0, y1);
    };
    // Putting the x coordinates in order saves multiple swaps in drawFastHLine
    if (x0 > x1) {
        SWAPint(x0, x1);
    };

    for (i = y0; i <= y1; i++) {
        drawForegroundHLine(x0, x1, i);
    }
}


uint16_t Effects::XY( uint8_t x, uint8_t y)
{
    return matrix->XY(x,y);
}


void Effects::CircleStream(uint8_t value) {

    // Sigh, 3rd array taking memory, convert to malloc?
    CRGB leds2[matrix->NUM_LEDS];

    DimAll(value);

    for (uint8_t offset = 0; offset < matrix->MATRIX_CENTER_X; offset++) {
        boolean hasprev = false;
        uint16_t prevxy = 0;

        for (uint8_t theta = 0; theta < 255; theta++) {
            uint8_t x = mapcos8(theta, offset, (matrix->MATRIX_WIDTH - 1) - offset);
            uint8_t y = mapsin8(theta, offset, (matrix->MATRIX_HEIGHT - 1) - offset);

            uint16_t xy = XY(x, y);

            if (hasprev) {
                leds2[prevxy] += leds[xy];
            }

            prevxy = xy;
            hasprev = true;
        }
    }

    for (uint8_t x = 0; x < matrix->MATRIX_WIDTH; x++) {
        for (uint8_t y = 0; y < matrix->MATRIX_HEIGHT; y++) {
            uint16_t xy = XY(x, y);
            leds[xy] = leds2[xy];
            leds[xy].nscale8(value);
            leds2[xy].nscale8(value);
        }
    }
}

void Effects::CyclePalette() {

    paletteIndex++;
    if(!palettes[paletteIndex].palette)
        paletteIndex = 0;

    printf("Palatte[%d] %s\n", paletteIndex, palettes[paletteIndex].name);

    currentPalette = palettes[paletteIndex].palette;
}

// scale the brightness of the screenbuffer down
void Effects::DimAll(byte value)
{
    fadeToBlackBy( leds, matrix->NUMMATRIX, 255 - value);
}

void Effects::Caleidoscope1() {
    for (int x = 0; x < matrix->MATRIX_CENTER_X; x++) {
        for (int y = 0; y < matrix->MATRIX_CENTER_Y; y++) {
            leds[XY(matrix->MATRIX_WIDTH - 1 - x, y)] = leds[XY(x, y)];
            leds[XY(matrix->MATRIX_WIDTH - 1 - x, matrix->MATRIX_HEIGHT - 1 - y)] = leds[XY(x, y)];
            leds[XY(x, matrix->MATRIX_HEIGHT - 1 - y)] = leds[XY(x, y)];
        }
    }
}

void Effects::Caleidoscope2() {
    for (int x = 0; x < matrix->MATRIX_CENTER_X; x++) {
        for (int y = 0; y < matrix->MATRIX_CENTER_Y; y++) {
            leds[XY(matrix->MATRIX_WIDTH - 1 - x, y)] = leds[XY(y, x)];
            leds[XY(x, matrix->MATRIX_HEIGHT - 1 - y)] = leds[XY(y, x)];
            leds[XY(matrix->MATRIX_WIDTH - 1 - x, matrix->MATRIX_HEIGHT - 1 - y)] = leds[XY(x, y)];
        }
    }
}

// copy one diagonal triangle into the other one within a 16x16
void Effects::Caleidoscope3() {
    for (int x = 0; x <= matrix->MATRIX_CENTRE_X; x++) {
        for (int y = 0; y <= x; y++) {
            leds[XY(x, y)] = leds[XY(y, x)];
        }
    }
}

void Effects::Caleidoscope4() {
    for (int x = 0; x <= matrix->MATRIX_CENTRE_X; x++) {
        for (int y = 0; y <= matrix->MATRIX_CENTRE_Y - x; y++) {
            leds[XY(matrix->MATRIX_CENTRE_Y - y, matrix->MATRIX_CENTRE_X - x)] = leds[XY(x, y)];
        }
    }
}

void Effects::Caleidoscope5() {
    for (int x = 0; x < matrix->MATRIX_WIDTH / 4; x++) {
        for (int y = 0; y <= x; y++) {
            leds[XY(x, y)] = leds[XY(y, x)];
        }
    }

    for (int x = matrix->MATRIX_WIDTH / 4; x < matrix->MATRIX_WIDTH / 2; x++) {
        for (int y = matrix->MATRIX_HEIGHT / 4; y >= 0; y--) {
            leds[XY(x, y)] = leds[XY(y, x)];
        }
    }
}

void Effects::Caleidoscope6() {
    for (int x = 1; x < matrix->MATRIX_CENTER_X; x++) {
        leds[XY(7 - x, 7)] = leds[XY(x, 0)];
    } //a
    for (int x = 2; x < matrix->MATRIX_CENTER_X; x++) {
        leds[XY(7 - x, 6)] = leds[XY(x, 1)];
    } //b
    for (int x = 3; x < matrix->MATRIX_CENTER_X; x++) {
        leds[XY(7 - x, 5)] = leds[XY(x, 2)];
    } //c
    for (int x = 4; x < matrix->MATRIX_CENTER_X; x++) {
        leds[XY(7 - x, 4)] = leds[XY(x, 3)];
    } //d
    for (int x = 5; x < matrix->MATRIX_CENTER_X; x++) {
        leds[XY(7 - x, 3)] = leds[XY(x, 4)];
    } //e
    for (int x = 6; x < matrix->MATRIX_CENTER_X; x++) {
        leds[XY(7 - x, 2)] = leds[XY(x, 5)];
    } //f
    for (int x = 7; x < matrix->MATRIX_CENTER_X; x++) {
        leds[XY(7 - x, 1)] = leds[XY(x, 6)];
    } //g
}


void Effects::SpiralStream(int x, int y, int r, byte dimm) {
    for (int d = r; d >= 0; d--) { // from the outside to the inside
        for (int i = x - d; i <= x + d; i++) {
            leds[XY(i, y - d)] += leds[XY(i + 1, y - d)]; // lowest row to the right
            leds[XY(i, y - d)].nscale8(dimm);
        }
        for (int i = y - d; i <= y + d; i++) {
            leds[XY(x + d, i)] += leds[XY(x + d, i + 1)]; // right colum up
            leds[XY(x + d, i)].nscale8(dimm);
        }
        for (int i = x + d; i >= x - d; i--) {
            leds[XY(i, y + d)] += leds[XY(i - 1, y + d)]; // upper row to the left
            leds[XY(i, y + d)].nscale8(dimm);
        }
        for (int i = y + d; i >= y - d; i--) {
            leds[XY(x - d, i)] += leds[XY(x - d, i - 1)]; // left colum down
            leds[XY(x - d, i)].nscale8(dimm);
        }
    }
}

void Effects::Expand(int centerX, int centerY, int radius, byte dimm) {
    if (radius == 0)
        return;

    int currentRadius = radius;

    while (currentRadius > 0) {
        int a = radius, b = 0;
        int radiusError = 1 - a;

        int nextRadius = currentRadius - 1;
        int nextA = nextRadius - 1, nextB = 0;
        int nextRadiusError = 1 - nextA;

        while (a >= b)
        {
            // move them out one pixel on the radius
            leds[XY(a + centerX, b + centerY)] = leds[XY(nextA + centerX, nextB + centerY)];
            leds[XY(b + centerX, a + centerY)] = leds[XY(nextB + centerX, nextA + centerY)];
            leds[XY(-a + centerX, b + centerY)] = leds[XY(-nextA + centerX, nextB + centerY)];
            leds[XY(-b + centerX, a + centerY)] = leds[XY(-nextB + centerX, nextA + centerY)];
            leds[XY(-a + centerX, -b + centerY)] = leds[XY(-nextA + centerX, -nextB + centerY)];
            leds[XY(-b + centerX, -a + centerY)] = leds[XY(-nextB + centerX, -nextA + centerY)];
            leds[XY(a + centerX, -b + centerY)] = leds[XY(nextA + centerX, -nextB + centerY)];
            leds[XY(b + centerX, -a + centerY)] = leds[XY(nextB + centerX, -nextA + centerY)];

            // dim them
            leds[XY(a + centerX, b + centerY)].nscale8(dimm);
            leds[XY(b + centerX, a + centerY)].nscale8(dimm);
            leds[XY(-a + centerX, b + centerY)].nscale8(dimm);
            leds[XY(-b + centerX, a + centerY)].nscale8(dimm);
            leds[XY(-a + centerX, -b + centerY)].nscale8(dimm);
            leds[XY(-b + centerX, -a + centerY)].nscale8(dimm);
            leds[XY(a + centerX, -b + centerY)].nscale8(dimm);
            leds[XY(b + centerX, -a + centerY)].nscale8(dimm);

            b++;
            if (radiusError < 0)
                radiusError += 2 * b + 1;
            else
            {
                a--;
                radiusError += 2 * (b - a + 1);
            }

            nextB++;
            if (nextRadiusError < 0)
                nextRadiusError += 2 * nextB + 1;
            else
            {
                nextA--;
                nextRadiusError += 2 * (nextB - nextA + 1);
            }
        }

        currentRadius--;
    }
}

void Effects::StreamRight(byte scale, int fromX, int toX, int fromY, int toY)
{
    if(toY == -1)
        toY = matrix->MATRIX_HEIGHT;

    if(toX == -1)
        toX = matrix->MATRIX_WIDTH;

    for (int x = fromX + 1; x < toX; x++) {
        for (int y = fromY; y < toY; y++) {
            leds[XY(x, y)] += leds[XY(x - 1, y)];
            leds[XY(x, y)].nscale8(scale);
        }
    }
    for (int y = fromY; y < toY; y++)
        leds[XY(0, y)].nscale8(scale);
}

void Effects::StreamLeft(byte scale, int fromX, int toX, int fromY, int toY)
{
    if(toY == -1)
        toY = matrix->MATRIX_HEIGHT;

    if(toX == -1)
        toX = matrix->MATRIX_WIDTH;

    for (int x = toX; x < fromX; x++) {
        for (int y = fromY; y < toY; y++) {
            leds[XY(x, y)] += leds[XY(x + 1, y)];
            leds[XY(x, y)].nscale8(scale);
        }
    }
    for (int y = fromY; y < toY; y++)
        leds[XY(0, y)].nscale8(scale);
}

void Effects::StreamDown(byte scale)
{
    for (int x = 0; x < matrix->MATRIX_WIDTH; x++) {
        for (int y = 1; y < matrix->MATRIX_HEIGHT; y++) {
            leds[XY(x, y)] += leds[XY(x, y - 1)];
            leds[XY(x, y)].nscale8(scale);
        }
    }
    for (int x = 0; x < matrix->MATRIX_WIDTH; x++)
        leds[XY(x, 0)].nscale8(scale);
}

void Effects::StreamUp(byte scale)
{
    for (int x = 0; x < matrix->MATRIX_WIDTH; x++) {
        for (int y = matrix->MATRIX_HEIGHT - 2; y >= 0; y--) {
            leds[XY(x, y)] += leds[XY(x, y + 1)];
            leds[XY(x, y)].nscale8(scale);
        }
    }
    for (int x = 0; x < matrix->MATRIX_WIDTH; x++)
        leds[XY(x, matrix->MATRIX_HEIGHT - 1)].nscale8(scale);
}

void Effects::StreamUpAndLeft(byte scale)
{
    for (int x = 0; x < matrix->MATRIX_WIDTH - 1; x++) {
        for (int y = matrix->MATRIX_HEIGHT - 2; y >= 0; y--) {
            leds[XY(x, y)] += leds[XY(x + 1, y + 1)];
            leds[XY(x, y)].nscale8(scale);
        }
    }
    for (int x = 0; x < matrix->MATRIX_WIDTH; x++)
        leds[XY(x, matrix->MATRIX_HEIGHT - 1)].nscale8(scale);
    for (int y = 0; y < matrix->MATRIX_HEIGHT; y++)
        leds[matrix->XY(matrix->MATRIX_WIDTH - 1, y)].nscale8(scale);
}

void Effects::StreamUpAndRight(byte scale)
{
    for (int x = 0; x < matrix->MATRIX_WIDTH - 1; x++) {
        for (int y = matrix->MATRIX_HEIGHT - 2; y >= 0; y--) {
            leds[XY(x + 1, y)] += leds[XY(x, y + 1)];
            leds[XY(x, y)].nscale8(scale);
        }
    }
    // fade the bottom row
    for (int x = 0; x < matrix->MATRIX_WIDTH; x++)
        leds[XY(x, matrix->MATRIX_HEIGHT - 1)].nscale8(scale);

    // fade the right column
    for (int y = 0; y < matrix->MATRIX_HEIGHT; y++)
        leds[XY(matrix->MATRIX_WIDTH - 1, y)].nscale8(scale);
}

void Effects::MoveDown() {
    for (int y = matrix->MATRIX_HEIGHT - 1; y > 0; y--) {
        for (int x = 0; x < matrix->MATRIX_WIDTH; x++) {
            leds[XY(x, y)] = leds[XY(x, y - 1)];
        }
    }
}

void Effects::VerticalMoveFrom(int start, int end) {
    for (int y = end; y > start; y--) {
        for (int x = 0; x < matrix->MATRIX_WIDTH; x++) {
            leds[XY(x, y)] = leds[XY(x, y - 1)];
        }
    }
}

void Effects::Copy(byte x0, byte y0, byte x1, byte y1, byte x2, byte y2) {
    for (int y = y0; y < y1 + 1; y++) {
        for (int x = x0; x < x1 + 1; x++) {
            leds[XY(x + x2 - x0, y + y2 - y0)] = leds[XY(x, y)];
        }
    }
}

void Effects::RotateTriangle() {
    for (int x = 1; x < matrix->MATRIX_CENTER_X; x++) {
        for (int y = 0; y < x; y++) {
            leds[XY(x, 7 - y)] = leds[XY(7 - x, y)];
        }
    }
}
void Effects::MirrorTriangle() {
    for (int x = 1; x < matrix->MATRIX_CENTER_X; x++) {
        for (int y = 0; y < x; y++) {
            leds[XY(7 - y, x)] = leds[XY(7 - x, y)];
        }
    }
}

void Effects::RainbowTriangle() {
    for (int i = 0; i < matrix->MATRIX_CENTER_X; i++) {
        for (int j = 0; j <= i; j++) {
            Pixel(7 - i, j, i * j * 4);
        }
    }
}


void Effects::BresenhamLine(int x0, int y0, int x1, int y1, CRGB color)
{
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;
    for (;;) {
        leds[XY(x0, y0)] += color;
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 > dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void Effects::BresenhamLine(int x0, int y0, int x1, int y1, byte colorIndex)
{
    BresenhamLine(x0, y0, x1, y1, ColorFromCurrentPalette(colorIndex));
}


void Effects::Pixel(int x, int y, uint8_t colorIndex) {
    leds[XY(x, y)] = ColorFromCurrentPalette(colorIndex);
}

CRGB Effects::ColorFromCurrentPalette(uint8_t index, uint8_t brightness, TBlendType blendType) {

    return ColorFromPalette(currentPalette, index, brightness, blendType);
}

CRGB Effects::HsvToRgb(uint8_t h, uint8_t s, uint8_t v) {
    CHSV hsv = CHSV(h, s, v);
    CRGB rgb;
    hsv2rgb_spectrum(hsv, rgb);
    return rgb;
}

void Effects::MoveX(byte delta) {

    //TODO do we need another buffer???
    CRGB leds2[matrix->NUM_LEDS];

    for (int y = 0; y < matrix->MATRIX_HEIGHT; y++) {
        for (int x = 0; x < matrix->MATRIX_WIDTH - delta; x++) {
            leds2[XY(x, y)] = leds[XY(x + delta, y)];
        }
        for (int x = matrix->MATRIX_WIDTH - delta; x < matrix->MATRIX_WIDTH; x++) {
            leds2[XY(x, y)] = leds[XY(x + delta - matrix->MATRIX_WIDTH, y)];
        }
    }

    // write back to leds
    for (uint8_t y = 0; y < matrix->MATRIX_HEIGHT; y++) {
        for (uint8_t x = 0; x < matrix->MATRIX_WIDTH; x++) {
            leds[XY(x, y)] = leds2[XY(x, y)];
        }
    }
}

void Effects::MoveY(byte delta) {

    //TODO do we need another buffer???
    CRGB leds2[matrix->NUM_LEDS];

    for (int x = 0; x < matrix->MATRIX_WIDTH; x++) {
        for (int y = 0; y < matrix->MATRIX_HEIGHT - delta; y++) {
            leds2[XY(x, y)] = leds[XY(x, y + delta)];
        }
        for (int y = matrix->MATRIX_HEIGHT - delta; y < matrix->MATRIX_HEIGHT; y++) {
            leds2[XY(x, y)] = leds[XY(x, y + delta - matrix->MATRIX_HEIGHT)];
        }
    }

    // write back to leds
    for (uint8_t y = 0; y < matrix->MATRIX_HEIGHT; y++) {
        for (uint8_t x = 0; x < matrix->MATRIX_WIDTH; x++) {
            leds[XY(x, y)] = leds2[XY(x, y)];
        }
    }
}


//void Effects::NoiseVariablesSetup() {
//    noisesmoothing = 200;
//
//    noise_x = random16();
//    noise_y = random16();
//    noise_z = random16();
//    noise_scale_x = 6000;
//    noise_scale_y = 6000;
//}
//
//void Effects::FillNoise() {
//    for (uint8_t i = 0; i < MATRIX_WIDTH; i++) {
//        uint32_t ioffset = noise_scale_x * (i - MATRIX_CENTRE_Y);
//
//        for (uint8_t j = 0; j < MATRIX_HEIGHT; j++) {
//            uint32_t joffset = noise_scale_y * (j - MATRIX_CENTRE_Y);
//
//            byte data = inoise16(noise_x + ioffset, noise_y + joffset, noise_z) >> 8;
//
//            uint8_t olddata = noise[i][j];
//            uint8_t newdata = scale8(olddata, noisesmoothing) + scale8(data, 256 - noisesmoothing);
//            data = newdata;
//
//            noise[i][j] = data;
//        }
//    }
//}
//
//void Effects::standardNoiseSmearing() {
//    noise_x += 1000;
//    noise_y += 1000;
//    noise_scale_x = 4000;
//    noise_scale_y = 4000;
//    FillNoise();
//
//    MoveX(3);
//    MoveFractionalNoiseY(4);
//
//    MoveY(3);
//    MoveFractionalNoiseX(4);
//}

//void Effects::MoveFractionalNoiseX(byte amt) {
//
//    //TODO do we need another buffer???
//    CRGB leds2[NUM_LEDS];
//
//    // move delta pixelwise
//    for (int y = 0; y < MATRIX_HEIGHT; y++) {
//        uint16_t amount = noise[0][y] * amt;
//        byte delta = 31 - (amount / 256);
//
//        for (int x = 0; x < MATRIX_WIDTH - delta; x++) {
//            leds2[XY(x, y)] = leds[XY(x + delta, y)];
//        }
//        for (int x = MATRIX_WIDTH - delta; x < MATRIX_WIDTH; x++) {
//            leds2[XY(x, y)] = leds[XY(x + delta - MATRIX_WIDTH, y)];
//        }
//    }
//
//    //move fractions
//    CRGB PixelA;
//    CRGB PixelB;
//
//    for (uint8_t y = 0; y < MATRIX_HEIGHT; y++) {
//        uint16_t amount = noise[0][y] * amt;
//        byte delta = 31 - (amount / 256);
//        byte fractions = amount - (delta * 256);
//
//        for (uint8_t x = 1; x < MATRIX_WIDTH; x++) {
//            PixelA = leds2[XY(x, y)];
//            PixelB = leds2[XY(x - 1, y)];
//
//            PixelA %= 255 - fractions;
//            PixelB %= fractions;
//
//            leds[XY(x, y)] = PixelA + PixelB;
//        }
//
//        PixelA = leds2[XY(0, y)];
//        PixelB = leds2[XY(MATRIX_WIDTH - 1, y)];
//
//        PixelA %= 255 - fractions;
//        PixelB %= fractions;
//
//        leds[XY(0, y)] = PixelA + PixelB;
//    }
//}
//
//void Effects::MoveFractionalNoiseY(byte amt) {
//
//    //TODO do we need another buffer???
//    CRGB leds2[NUM_LEDS];
//
//    // move delta pixelwise
//    for (int x = 0; x < MATRIX_WIDTH; x++) {
//        uint16_t amount = noise[x][0] * amt;
//        byte delta = 31 - (amount / 256);
//
//        for (int y = 0; y < MATRIX_WIDTH - delta; y++) {
//            leds2[XY(x, y)] = leds[XY(x, y + delta)];
//        }
//        for (int y = MATRIX_WIDTH - delta; y < MATRIX_WIDTH; y++) {
//            leds2[XY(x, y)] = leds[XY(x, y + delta - MATRIX_WIDTH)];
//        }
//    }//move fractions
//    CRGB PixelA;
//    CRGB PixelB;
//
//    for (uint8_t x = 0; x < MATRIX_HEIGHT; x++) {
//        uint16_t amount = noise[x][0] * amt;
//        byte delta = 31 - (amount / 256);
//        byte fractions = amount - (delta * 256);
//
//        for (uint8_t y = 1; y < MATRIX_WIDTH; y++) {
//            PixelA = leds2[XY(x, y)];
//            PixelB = leds2[XY(x, y - 1)];
//
//            PixelA %= 255 - fractions;
//            PixelB %= fractions;
//
//            leds[XY(x, y)] = PixelA + PixelB;
//        }
//
//        PixelA = leds2[XY(x, 0)];
//        PixelB = leds2[XY(x, MATRIX_WIDTH - 1)];
//
//        PixelA %= 255 - fractions;
//        PixelB %= fractions;
//
//        leds[XY(x, 0)] = PixelA + PixelB;
//    }
//}
