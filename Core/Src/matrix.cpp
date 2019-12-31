#include "matrix.h"
#include "ws2812.h"
#include <math.h>

Matrix *matrix = 0;


Matrix::Matrix() : Adafruit_GFX(16, 16)
{

}

void Matrix::drawPixel(int x, int y, CRGB color)
{
    framebuffer[x][y] = color;
}

void Matrix::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    CRGB rgb_color;
    rgb_color.r = (color % 0xF800) >> 9;
    rgb_color.g = (color % 0x07E0) >> 9;
    rgb_color.b = (color % 0x1F) << 3 ;

    drawPixel((int)x, (int)y, rgb_color);
}

void  Matrix::paint()
{
    for (int row = 0; row < 16; ++row)
    {
        for (int col = 0; col < 16; ++col)
        {
            ws2812b_set_pixel(row, col, framebuffer[col][row].r, framebuffer[col][row].g, framebuffer[col][row].b);
        }
    }

    ws2812b_paint();
}

void Matrix::clear()
{
    ws2812b_clear();
}

//
//
//#define _swap_int16_t(a, b)                                                    \
//  {                                                                            \
//    int16_t t = a;                                                             \
//    a = b;                                                                     \
//    b = t;                                                                     \
//  }
//
//void Matrix::writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
//                             uint16_t color)
//{
//  int16_t steep = abs(y1 - y0) > abs(x1 - x0);
//  if (steep) {
//    _swap_int16_t(x0, y0);
//    _swap_int16_t(x1, y1);
//  }
//
//  if (x0 > x1) {
//    _swap_int16_t(x0, x1);
//    _swap_int16_t(y0, y1);
//  }
//
//  int16_t dx, dy;
//  dx = x1 - x0;
//  dy = abs(y1 - y0);
//
//  int16_t err = dx / 2;
//  int16_t ystep;
//
//  if (y0 < y1) {
//    ystep = 1;
//  } else {
//    ystep = -1;
//  }
//
//  for (; x0 <= x1; x0++) {
//    if (steep) {
//      drawPixel((int16_t)y0, (int16_t)x0, color);
//    } else {
//      drawPixel((int16_t)x0, (int16_t)y0, color);
//    }
//    err -= dy;
//    if (err < 0) {
//      y0 += ystep;
//      err += dx;
//    }
//  }
//}
//
//void Matrix::drawFastHLine(int16_t x, int16_t y, int16_t w,
//                                 uint16_t color) {
//    writeLine(x, y, x + w - 1, y, color);
//  }
//
//void Matrix::drawFastVLine(int16_t x, int16_t y, int16_t h,
//                                 uint16_t color) {
//  writeLine(x, y, x, y + h - 1, color);
//}
//
//void Matrix::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
//                            uint16_t color) {
//  // Update in subclasses if desired!
//  if (x0 == x1) {
//    if (y0 > y1)
//      _swap_int16_t(y0, y1);
//    drawFastVLine(x0, y0, y1 - y0 + 1, color);
//  } else if (y0 == y1) {
//    if (x0 > x1)
//      _swap_int16_t(x0, x1);
//    drawFastHLine(x0, y0, x1 - x0 + 1, color);
//  } else {
//    writeLine(x0, y0, x1, y1, color);
//  }
//}
