#include <stdio.h>

#include "animate_disco.h"
#include "ws2812.h"
#include "main.h"

AnimateDisco::AnimateDisco()
{
    tick = 0;
    radius = 150;
    red_row = 15;
    red_col = 0;
    red_delta = 1;
    green_row = 0;
    green_col = 7;
    green_delta = 1;
    green_c_delta = 1;
    blue_row = 0;
    blue_col = 15;
    blue_delta = 1;
    scale = 1;//0.9;
}

AnimateDisco::~AnimateDisco()
{
}

void AnimateDisco::run()
{
    if(tick < HAL_GetTick())
    {
        tick = HAL_GetTick() + 100;

        ws2812b_clear();
        {
            static int flag = 0;
            flag++;
            if(flag > 1)
            {
                flag = 0;
                red_row += red_delta;
                if(red_row > 15)
                {
                    red_row = 14;
                    red_delta *= -1;
                }
                if(red_row < 0)
                {
                    red_row = 1;
                    red_delta *= -1;
                }
            }
        }

        green_row += green_delta;
        if(green_row > 15)
        {
            green_row = 14;
            green_delta *= -1;
        }
        if(green_row < 0)
        {
            green_row = 1;
            green_delta *= -1;
        }

        green_col += green_c_delta;
        if(green_col > 15)
        {
            green_col = 14;
            green_c_delta *= -1;
        }
        if(green_col < 0)
        {
            green_col = 1;
            green_c_delta *= -1;
        }

        {
            static int flag = 0;
            flag++;
            if(flag > 2)
            {
                flag = 0;
                blue_row += blue_delta;
                if(blue_row > 15)
                {
                    blue_row = 14;
                    blue_delta *= -1;
                }
                if(blue_row < 0)
                {
                    blue_row = 1;
                    blue_delta *= -1;
                }
            }
        }

        for (int row = 0; row < 16; ++row)
        {
            for (int col = 0; col < 16; ++col)
            {
                int r = 0, g = 0,b = 0;
                get_scale(row, col, &r, &g, &b);
                ws2812b_set_pixel(row, col, r, g, b);
                //printf("(%d, %d) = (%d, %d, %d)\n", row, col, r, g, b);
            }
        }
        //        ws2812b_set_pixel(red_row, red_col, 255, 0, 0);
        //        ws2812b_set_pixel(green_row, green_col, 0, 255, 0);
        //        ws2812b_set_pixel(blue_row, blue_col, 0, 0, 255);
        ws2812b_paint();
    }
}


void AnimateDisco::get_scale(int row, int col, int *r, int *g, int *b)
{
    //red distance (15, 0)
    int delta_r = red_row - row;
    int delta_c = red_col - col;
    int distance = (delta_r *delta_r) + (delta_c * delta_c);
    double factor = 1.0 - ((double)distance / radius);
    if(factor < 0)
        factor = 0;
    //printf("R (%d, %d), dr: %d, dc %d, d %d, f %f\n", row, col, delta_r, delta_c, distance, factor);
    *r = factor * 255.0 * scale;

    //green distance (0, 7)
    delta_r = green_row - row;
    delta_c = green_col - col;
    distance = (delta_r *delta_r) + (delta_c * delta_c);
    factor = 1.0 - ((double)distance / radius);
    if(factor < 0)
        factor = 0;
    //printf("G (%d, %d), dr: %d, dc %d, d %d, f %f\n", row, col, delta_r, delta_c, distance, factor);
    *g = factor * 255.0 * scale;

    //blue distance (15, 15)
    delta_r = blue_row - row;
    delta_c = blue_col - col;
    distance = (delta_r *delta_r) + (delta_c * delta_c);
    factor = 1.0 - ((double)distance / radius);
    if(factor < 0)
        factor = 0;
    //printf("B (%d, %d), dr: %d, dc %d, d %d, f %f\n", row, col, delta_r, delta_c, distance, factor);
    *b = factor * 255.0 * scale;
}
