#include "led_matrix.h"
#include "ws2812.h"

FrameBuffer framebuffer(16, 16);

LEDmatrix::LEDmatrix() : Matrix(&framebuffer)
{

}

void LEDmatrix::drawPixel(int x, int y, CRGB color)
{
    if(0 > x)
        x=0;
    if(15 < x)
        x = 15;
    if(0 > y)
        y=0;
    if(15 < y)
        y = 15;

    CRGB *pixel = getPixel(x, y);
    *pixel = color;
}

void LEDmatrix::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    CRGB rgb_color;
    rgb_color.r = (color % 0xF800) >> 9;
    rgb_color.g = (color % 0x07E0) >> 9;
    rgb_color.b = (color % 0x1F) << 3 ;

    drawPixel((int)x, (int)y, rgb_color);
}

void  LEDmatrix::paint()
{
    for (int row = 0; row < 16; ++row)
    {
        for (int col = 0; col < 16; ++col)
        {
            CRGB *pixel = getPixel(row, col);
            ws2812b_set_pixel(row, col, pixel->r, pixel->g, pixel->b);
        }
    }

    ws2812b_paint();
}

void LEDmatrix::clear()
{
    ws2812b_clear();
}
