#include "Effects.h"
#include "hsv2rgb.h"
#include "noise.h"

const CRGBPalette16 WoodFireColors_p = CRGBPalette16(CRGB::Black, CRGB::OrangeRed, CRGB::Orange, CRGB::Gold);            //* Orange
const CRGBPalette16 SodiumFireColors_p = CRGBPalette16(CRGB::Black, CRGB::Orange, CRGB::Gold, CRGB::Goldenrod);          //* Yellow
const CRGBPalette16 CopperFireColors_p = CRGBPalette16(CRGB::Black, CRGB::Green, CRGB::GreenYellow, CRGB::LimeGreen);    //* Green
const CRGBPalette16 AlcoholFireColors_p = CRGBPalette16(CRGB::Black, CRGB::Blue, CRGB::DeepSkyBlue, CRGB::LightSkyBlue); //* Blue
const CRGBPalette16 RubidiumFireColors_p = CRGBPalette16(CRGB::Black, CRGB::Indigo, CRGB::Indigo, CRGB::DarkBlue);       //* Indigo
const CRGBPalette16 PotassiumFireColors_p = CRGBPalette16(CRGB::Black, CRGB::Indigo, CRGB::MediumPurple, CRGB::DeepPink);//* Violet
const CRGBPalette16 LithiumFireColors_p = CRGBPalette16(CRGB::Black, CRGB::FireBrick, CRGB::Pink, CRGB::DeepPink);       //* Red

const CRGBPalette16 palettes[] = {
        RainbowColors_p,
        RainbowStripeColors_p,
        CloudColors_p,
        LavaColors_p,
        OceanColors_p,
        ForestColors_p,
        PartyColors_p,
        HeatColors_p
};

Effects::Effects(Matrix *matrix)
{
    this->matrix = matrix;
    leds = matrix->getBuffer();
    //setupIcePalette();//
    currentPalette = LavaColors_p;//OceanColors_p;//ForestColors_p;//HeatColors_p;// CloudColors_p;//setupGrayscalePalette();//RainbowColors_p;//RainbowStripeColors_p; //PartyColors_p;setupIcePalette

    //ShowFrame();
    //loadPalette(0);
    NoiseVariablesSetup();
}

void Effects::drawForegroundHLine(int16_t x0, int16_t x1, int16_t y) {
    // make sure line goes from x0 to x1
    if (x1 < x0)
        SWAPint(x1, x0);

    // check for completely out of bounds line
    if (x1 < 0 || x0 >= MATRIX_WIDTH || y < 0 || y >= MATRIX_HEIGHT)
        return;

    // truncate if partially out of bounds
    if (x0 < 0)
        x0 = 0;

    if (x1 >= MATRIX_WIDTH)
        x1 = MATRIX_WIDTH - 1;

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
    CRGB leds2[NUM_LEDS];

    DimAll(value);

    for (uint8_t offset = 0; offset < MATRIX_CENTER_X; offset++) {
        boolean hasprev = false;
        uint16_t prevxy = 0;

        for (uint8_t theta = 0; theta < 255; theta++) {
            uint8_t x = mapcos8(theta, offset, (MATRIX_WIDTH - 1) - offset);
            uint8_t y = mapsin8(theta, offset, (MATRIX_HEIGHT - 1) - offset);

            uint16_t xy = XY(x, y);

            if (hasprev) {
                leds2[prevxy] += leds[xy];
            }

            prevxy = xy;
            hasprev = true;
        }
    }

    for (uint8_t x = 0; x < MATRIX_WIDTH; x++) {
        for (uint8_t y = 0; y < MATRIX_HEIGHT; y++) {
            uint16_t xy = XY(x, y);
            leds[xy] = leds2[xy];
            leds[xy].nscale8(value);
            leds2[xy].nscale8(value);
        }
    }
}

void Effects::CyclePalette(int offset) {
    loadPalette(paletteIndex + offset);
}

void Effects::RandomPalette() {
    loadPalette(RandomPaletteIndex);
}

void Effects::loadPalette(int index) {
    paletteIndex = index;

    if (paletteIndex >= paletteCount)
        paletteIndex = 0;
    else if (paletteIndex < 0)
        paletteIndex = paletteCount - 1;

    switch (paletteIndex) {
    case 0:
        targetPalette = RainbowColors_p;
        currentPaletteName = (char *)"Rainbow";
        break;
        //case 1:
        //  targetPalette = RainbowStripeColors_p;
        //  currentPaletteName = (char *)"RainbowStripe";
        //  break;
    case 1:
        targetPalette = OceanColors_p;
        currentPaletteName = (char *)"Ocean";
        break;
    case 2:
        targetPalette = CloudColors_p;
        currentPaletteName = (char *)"Cloud";
        break;
    case 3:
        targetPalette = ForestColors_p;
        currentPaletteName = (char *)"Forest";
        break;
    case 4:
        targetPalette = PartyColors_p;
        currentPaletteName = (char *)"Party";
        break;
    case 5:
        setupGrayscalePalette();
        currentPaletteName = (char *)"Grey";
        break;
    case HeatColorsPaletteIndex:
        targetPalette = HeatColors_p;
        currentPaletteName = (char *)"Heat";
        break;
    case 7:
        targetPalette = LavaColors_p;
        currentPaletteName = (char *)"Lava";
        break;
    case 8:
        setupIcePalette();
        currentPaletteName = (char *)"Ice";
        break;
    case RandomPaletteIndex:
        loadPalette(random() % 8);
        paletteIndex = RandomPaletteIndex;
        currentPaletteName = (char *)"Random";
        break;
    }
}

void Effects::setPalette(/*String paletteName*/) {

    //TODO wtf is String
    //    if (paletteName == "Rainbow")
    //      loadPalette(0);
    //    //else if (paletteName == "RainbowStripe")
    //    //  loadPalette(1);
    //    else if (paletteName == "Ocean")
    //      loadPalette(1);
    //    else if (paletteName == "Cloud")
    //      loadPalette(2);
    //    else if (paletteName == "Forest")
    //      loadPalette(3);
    //    else if (paletteName == "Party")
    //      loadPalette(4);
    //    else if (paletteName == "Grayscale")
    //      loadPalette(5);
    //    else if (paletteName == "Heat")
    //      loadPalette(6);
    //    else if (paletteName == "Lava")
    //      loadPalette(7);
    //    else if (paletteName == "Ice")
    //      loadPalette(8);
    //    else if (paletteName == "Random")
    //      RandomPalette();
    //  }
    //
    //  void listPalettes() {
    //    Serial.println(F("{"));
    //    Serial.print(F("  \"count\": "));
    //    Serial.print(paletteCount);
    //    Serial.println(",");
    //    Serial.println(F("  \"results\": ["));
    //
    //    String paletteNames [] = {
    //      "Rainbow",
    //      // "RainbowStripe",
    //      "Ocean",
    //      "Cloud",
    //      "Forest",
    //      "Party",
    //      "Grayscale",
    //      "Heat",
    //      "Lava",
    //      "Ice",
    //      "Random"
    //    };
    //
    //    for (int i = 0; i < paletteCount; i++) {
    //      Serial.print(F("    \""));
    //      Serial.print(paletteNames[i]);
    //      if (i == paletteCount - 1)
    //        Serial.println(F("\""));
    //      else
    //        Serial.println(F("\","));
    //    }
    //
    //    Serial.println("  ]");
    //    Serial.println("}");
}

void Effects::setupGrayscalePalette() {
    targetPalette = CRGBPalette16(CRGB::Black, CRGB::White);
}

void Effects::setupIcePalette() {
    targetPalette = CRGBPalette16(CRGB::Black, CRGB::Blue, CRGB::Aqua, CRGB::White);
}

// set the speeds (and by that ratios) of the oscillators here
void Effects::MoveOscillators() {
    osci[0] = osci[0] + 5;
    osci[1] = osci[1] + 2;
    osci[2] = osci[2] + 3;
    osci[3] = osci[3] + 4;
    osci[4] = osci[4] + 1;
    if (osci[4] % 2 == 0)
        osci[5] = osci[5] + 1; // .5
    for (int i = 0; i < 4; i++) {
        p[i] = map8(sin8(osci[i]), 0, MATRIX_WIDTH - 1); //why? to keep the result in the range of 0-MATRIX_WIDTH (matrix size)
    }
}

void Effects::ShowFrame() {

    currentPalette = targetPalette;

}

// scale the brightness of the screenbuffer down
void Effects::DimAll(byte value)
{
    fadeToBlackBy( leds, NUMMATRIX, 255-value);
}

void Effects::Caleidoscope1() {
    for (int x = 0; x < MATRIX_CENTER_X; x++) {
        for (int y = 0; y < MATRIX_CENTER_Y; y++) {
            leds[XY(MATRIX_WIDTH - 1 - x, y)] = leds[XY(x, y)];
            leds[XY(MATRIX_WIDTH - 1 - x, MATRIX_HEIGHT - 1 - y)] = leds[XY(x, y)];
            leds[XY(x, MATRIX_HEIGHT - 1 - y)] = leds[XY(x, y)];
        }
    }
}

void Effects::Caleidoscope2() {
    for (int x = 0; x < MATRIX_CENTER_X; x++) {
        for (int y = 0; y < MATRIX_CENTER_Y; y++) {
            leds[XY(MATRIX_WIDTH - 1 - x, y)] = leds[XY(y, x)];
            leds[XY(x, MATRIX_HEIGHT - 1 - y)] = leds[XY(y, x)];
            leds[XY(MATRIX_WIDTH - 1 - x, MATRIX_HEIGHT - 1 - y)] = leds[XY(x, y)];
        }
    }
}

// copy one diagonal triangle into the other one within a 16x16
void Effects::Caleidoscope3() {
    for (int x = 0; x <= MATRIX_CENTRE_X; x++) {
        for (int y = 0; y <= x; y++) {
            leds[XY(x, y)] = leds[XY(y, x)];
        }
    }
}

void Effects::Caleidoscope4() {
    for (int x = 0; x <= MATRIX_CENTRE_X; x++) {
        for (int y = 0; y <= MATRIX_CENTRE_Y - x; y++) {
            leds[XY(MATRIX_CENTRE_Y - y, MATRIX_CENTRE_X - x)] = leds[XY(x, y)];
        }
    }
}

void Effects::Caleidoscope5() {
    for (int x = 0; x < MATRIX_WIDTH / 4; x++) {
        for (int y = 0; y <= x; y++) {
            leds[XY(x, y)] = leds[XY(y, x)];
        }
    }

    for (int x = MATRIX_WIDTH / 4; x < MATRIX_WIDTH / 2; x++) {
        for (int y = MATRIX_HEIGHT / 4; y >= 0; y--) {
            leds[XY(x, y)] = leds[XY(y, x)];
        }
    }
}

void Effects::Caleidoscope6() {
    for (int x = 1; x < MATRIX_CENTER_X; x++) {
        leds[XY(7 - x, 7)] = leds[XY(x, 0)];
    } //a
    for (int x = 2; x < MATRIX_CENTER_X; x++) {
        leds[XY(7 - x, 6)] = leds[XY(x, 1)];
    } //b
    for (int x = 3; x < MATRIX_CENTER_X; x++) {
        leds[XY(7 - x, 5)] = leds[XY(x, 2)];
    } //c
    for (int x = 4; x < MATRIX_CENTER_X; x++) {
        leds[XY(7 - x, 4)] = leds[XY(x, 3)];
    } //d
    for (int x = 5; x < MATRIX_CENTER_X; x++) {
        leds[XY(7 - x, 3)] = leds[XY(x, 4)];
    } //e
    for (int x = 6; x < MATRIX_CENTER_X; x++) {
        leds[XY(7 - x, 2)] = leds[XY(x, 5)];
    } //f
    for (int x = 7; x < MATRIX_CENTER_X; x++) {
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
    for (int x = 0; x < MATRIX_WIDTH; x++) {
        for (int y = 1; y < MATRIX_HEIGHT; y++) {
            leds[XY(x, y)] += leds[XY(x, y - 1)];
            leds[XY(x, y)].nscale8(scale);
        }
    }
    for (int x = 0; x < MATRIX_WIDTH; x++)
        leds[XY(x, 0)].nscale8(scale);
}

void Effects::StreamUp(byte scale)
{
    for (int x = 0; x < MATRIX_WIDTH; x++) {
        for (int y = MATRIX_HEIGHT - 2; y >= 0; y--) {
            leds[XY(x, y)] += leds[XY(x, y + 1)];
            leds[XY(x, y)].nscale8(scale);
        }
    }
    for (int x = 0; x < MATRIX_WIDTH; x++)
        leds[XY(x, MATRIX_HEIGHT - 1)].nscale8(scale);
}

void Effects::StreamUpAndLeft(byte scale)
{
    for (int x = 0; x < MATRIX_WIDTH - 1; x++) {
        for (int y = MATRIX_HEIGHT - 2; y >= 0; y--) {
            leds[XY(x, y)] += leds[XY(x + 1, y + 1)];
            leds[XY(x, y)].nscale8(scale);
        }
    }
    for (int x = 0; x < MATRIX_WIDTH; x++)
        leds[XY(x, MATRIX_HEIGHT - 1)].nscale8(scale);
    for (int y = 0; y < MATRIX_HEIGHT; y++)
        leds[XY(MATRIX_WIDTH - 1, y)].nscale8(scale);
}

void Effects::StreamUpAndRight(byte scale)
{
    for (int x = 0; x < MATRIX_WIDTH - 1; x++) {
        for (int y = MATRIX_HEIGHT - 2; y >= 0; y--) {
            leds[XY(x + 1, y)] += leds[XY(x, y + 1)];
            leds[XY(x, y)].nscale8(scale);
        }
    }
    // fade the bottom row
    for (int x = 0; x < MATRIX_WIDTH; x++)
        leds[XY(x, MATRIX_HEIGHT - 1)].nscale8(scale);

    // fade the right column
    for (int y = 0; y < MATRIX_HEIGHT; y++)
        leds[XY(MATRIX_WIDTH - 1, y)].nscale8(scale);
}

void Effects::MoveDown() {
    for (int y = MATRIX_HEIGHT - 1; y > 0; y--) {
        for (int x = 0; x < MATRIX_WIDTH; x++) {
            leds[XY(x, y)] = leds[XY(x, y - 1)];
        }
    }
}

void Effects::VerticalMoveFrom(int start, int end) {
    for (int y = end; y > start; y--) {
        for (int x = 0; x < MATRIX_WIDTH; x++) {
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
    for (int x = 1; x < MATRIX_CENTER_X; x++) {
        for (int y = 0; y < x; y++) {
            leds[XY(x, 7 - y)] = leds[XY(7 - x, y)];
        }
    }
}
void Effects::MirrorTriangle() {
    for (int x = 1; x < MATRIX_CENTER_X; x++) {
        for (int y = 0; y < x; y++) {
            leds[XY(7 - y, x)] = leds[XY(7 - x, y)];
        }
    }
}

void Effects::RainbowTriangle() {
    for (int i = 0; i < MATRIX_CENTER_X; i++) {
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

void Effects::NoiseVariablesSetup() {
    noisesmoothing = 200;

    noise_x = random16();
    noise_y = random16();
    noise_z = random16();
    noise_scale_x = 6000;
    noise_scale_y = 6000;
}

void Effects::FillNoise() {
    for (uint8_t i = 0; i < MATRIX_WIDTH; i++) {
        uint32_t ioffset = noise_scale_x * (i - MATRIX_CENTRE_Y);

        for (uint8_t j = 0; j < MATRIX_HEIGHT; j++) {
            uint32_t joffset = noise_scale_y * (j - MATRIX_CENTRE_Y);

            byte data = inoise16(noise_x + ioffset, noise_y + joffset, noise_z) >> 8;

            uint8_t olddata = noise[i][j];
            uint8_t newdata = scale8(olddata, noisesmoothing) + scale8(data, 256 - noisesmoothing);
            data = newdata;

            noise[i][j] = data;
        }
    }
}

void Effects::standardNoiseSmearing() {
    noise_x += 1000;
    noise_y += 1000;
    noise_scale_x = 4000;
    noise_scale_y = 4000;
    FillNoise();

    MoveX(3);
    MoveFractionalNoiseY(4);

    MoveY(3);
    MoveFractionalNoiseX(4);
}

void Effects::MoveX(byte delta) {

    //TODO do we need another buffer???
    CRGB leds2[NUM_LEDS];

    for (int y = 0; y < MATRIX_HEIGHT; y++) {
        for (int x = 0; x < MATRIX_WIDTH - delta; x++) {
            leds2[XY(x, y)] = leds[XY(x + delta, y)];
        }
        for (int x = MATRIX_WIDTH - delta; x < MATRIX_WIDTH; x++) {
            leds2[XY(x, y)] = leds[XY(x + delta - MATRIX_WIDTH, y)];
        }
    }

    // write back to leds
    for (uint8_t y = 0; y < MATRIX_HEIGHT; y++) {
        for (uint8_t x = 0; x < MATRIX_WIDTH; x++) {
            leds[XY(x, y)] = leds2[XY(x, y)];
        }
    }
}

void Effects::MoveY(byte delta) {

    //TODO do we need another buffer???
    CRGB leds2[NUM_LEDS];

    for (int x = 0; x < MATRIX_WIDTH; x++) {
        for (int y = 0; y < MATRIX_HEIGHT - delta; y++) {
            leds2[XY(x, y)] = leds[XY(x, y + delta)];
        }
        for (int y = MATRIX_HEIGHT - delta; y < MATRIX_HEIGHT; y++) {
            leds2[XY(x, y)] = leds[XY(x, y + delta - MATRIX_HEIGHT)];
        }
    }

    // write back to leds
    for (uint8_t y = 0; y < MATRIX_HEIGHT; y++) {
        for (uint8_t x = 0; x < MATRIX_WIDTH; x++) {
            leds[XY(x, y)] = leds2[XY(x, y)];
        }
    }
}

void Effects::MoveFractionalNoiseX(byte amt) {

    //TODO do we need another buffer???
    CRGB leds2[NUM_LEDS];

    // move delta pixelwise
    for (int y = 0; y < MATRIX_HEIGHT; y++) {
        uint16_t amount = noise[0][y] * amt;
        byte delta = 31 - (amount / 256);

        for (int x = 0; x < MATRIX_WIDTH - delta; x++) {
            leds2[XY(x, y)] = leds[XY(x + delta, y)];
        }
        for (int x = MATRIX_WIDTH - delta; x < MATRIX_WIDTH; x++) {
            leds2[XY(x, y)] = leds[XY(x + delta - MATRIX_WIDTH, y)];
        }
    }

    //move fractions
    CRGB PixelA;
    CRGB PixelB;

    for (uint8_t y = 0; y < MATRIX_HEIGHT; y++) {
        uint16_t amount = noise[0][y] * amt;
        byte delta = 31 - (amount / 256);
        byte fractions = amount - (delta * 256);

        for (uint8_t x = 1; x < MATRIX_WIDTH; x++) {
            PixelA = leds2[XY(x, y)];
            PixelB = leds2[XY(x - 1, y)];

            PixelA %= 255 - fractions;
            PixelB %= fractions;

            leds[XY(x, y)] = PixelA + PixelB;
        }

        PixelA = leds2[XY(0, y)];
        PixelB = leds2[XY(MATRIX_WIDTH - 1, y)];

        PixelA %= 255 - fractions;
        PixelB %= fractions;

        leds[XY(0, y)] = PixelA + PixelB;
    }
}

void Effects::MoveFractionalNoiseY(byte amt) {

    //TODO do we need another buffer???
    CRGB leds2[NUM_LEDS];

    // move delta pixelwise
    for (int x = 0; x < MATRIX_WIDTH; x++) {
        uint16_t amount = noise[x][0] * amt;
        byte delta = 31 - (amount / 256);

        for (int y = 0; y < MATRIX_WIDTH - delta; y++) {
            leds2[XY(x, y)] = leds[XY(x, y + delta)];
        }
        for (int y = MATRIX_WIDTH - delta; y < MATRIX_WIDTH; y++) {
            leds2[XY(x, y)] = leds[XY(x, y + delta - MATRIX_WIDTH)];
        }
    }//move fractions
    CRGB PixelA;
    CRGB PixelB;

    for (uint8_t x = 0; x < MATRIX_HEIGHT; x++) {
        uint16_t amount = noise[x][0] * amt;
        byte delta = 31 - (amount / 256);
        byte fractions = amount - (delta * 256);

        for (uint8_t y = 1; y < MATRIX_WIDTH; y++) {
            PixelA = leds2[XY(x, y)];
            PixelB = leds2[XY(x, y - 1)];

            PixelA %= 255 - fractions;
            PixelB %= fractions;

            leds[XY(x, y)] = PixelA + PixelB;
        }

        PixelA = leds2[XY(x, 0)];
        PixelB = leds2[XY(x, MATRIX_WIDTH - 1)];

        PixelA %= 255 - fractions;
        PixelB %= fractions;

        leds[XY(x, 0)] = PixelA + PixelB;
    }
}
