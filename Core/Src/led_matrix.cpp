#include "led_matrix.h"
#include "ws2812.h"

LEDmatrix::LEDmatrix()
{

}

void LEDmatrix::drawPixel(int x, int y, CRGB color)
{
    if(x < 0)
        x = 0;
    if(x > 15)
        x = 15;
    if(y < 0)
        y = 0;
    if(y > 15)
        y = 15;

    framebuffer[x][y] = color;
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
            ws2812b_set_pixel(row, col, framebuffer[col][row].r, framebuffer[col][row].g, framebuffer[col][row].b);
        }
    }

    ws2812b_paint();
}

void LEDmatrix::clear()
{
    ws2812b_clear();
}
