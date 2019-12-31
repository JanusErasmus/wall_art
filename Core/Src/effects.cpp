
#include "Effects.h"
Effects *effects;

void drawForegroundHLine(int16_t x0, int16_t x1, int16_t y) {
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

void fillForegroundRectangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
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

uint8_t beatcos8(accum88 beats_per_minute, uint8_t lowest, uint8_t highest, uint32_t timebase, uint8_t phase_offset)
{
  uint8_t beat = beat8(beats_per_minute, timebase);
  uint8_t beatcos = cos8(beat + phase_offset);
  uint8_t rangewidth = highest - lowest;
  uint8_t scaledbeat = scale8(beatcos, rangewidth);
  uint8_t result = lowest + scaledbeat;
  return result;
}

uint8_t beattriwave8(accum88 beats_per_minute, uint8_t lowest, uint8_t highest, uint32_t timebase, uint8_t phase_offset)
{
  uint8_t beat = beat8(beats_per_minute, timebase);
  uint8_t beatcos = triwave8(beat + phase_offset);
  uint8_t rangewidth = highest - lowest;
  uint8_t scaledbeat = scale8(beatcos, rangewidth);
  uint8_t result = lowest + scaledbeat;
  return result;
}

uint8_t mapsin8(uint8_t theta, uint8_t lowest, uint8_t highest) {
  uint8_t beatsin = sin8(theta);
  uint8_t rangewidth = highest - lowest;
  uint8_t scaledbeat = scale8(beatsin, rangewidth);
  uint8_t result = lowest + scaledbeat;
  return result;
}

uint8_t mapcos8(uint8_t theta, uint8_t lowest, uint8_t highest) {
  uint8_t beatcos = cos8(theta);
  uint8_t rangewidth = highest - lowest;
  uint8_t scaledbeat = scale8(beatcos, rangewidth);
  uint8_t result = lowest + scaledbeat;
  return result;
}

// Array of temperature readings at each simulation cell
// convert to malloc?
byte heat[NUM_LEDS];

uint32_t noise_x;
uint32_t noise_y;
uint32_t noise_z;
uint32_t noise_scale_x;
uint32_t noise_scale_y;

// second array, convert to malloc?
uint8_t noise[MATRIX_WIDTH][MATRIX_HEIGHT];

uint8_t noisesmoothing;

// Like XY, but for a mirror image from the top (used by misconfigured code)
int XY2( int x, int y, bool wrap) {
    wrap = wrap; // squelch compiler warning
    return matrix->XY(x,MATRIX_HEIGHT-1-y);
}

uint16_t XY( uint8_t x, uint8_t y) {
    return matrix->XY(x,y);
}

int wrapX(int x) {
    if (x < 0 ) return 0;
    if (x >= MATRIX_WIDTH) return (MATRIX_WIDTH-1);
    return x;
}

Effects::Effects()
{
}
