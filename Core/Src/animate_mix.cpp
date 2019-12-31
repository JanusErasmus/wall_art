#include "animate_mix.h"
#include "ws2812.h"

#include "main.h"

AnimateMix::AnimateMix()
{
    tick = 0;
    r = 0;
    g = 0;
    b = 0;
    row_delta = 1;
    mix_state = 0;
    mix_delta = 8;
    main_delta = 8;
}

AnimateMix::~AnimateMix()
{
}


void AnimateMix::run()
{
    if(tick < HAL_GetTick())
    {
        tick = HAL_GetTick() + 100;
        switch(mix_state)
        {
        //mix green
        case 0:
            g += mix_delta;
            if((g > 255) || (g <= 0))
            {
                if(g <= 0)
                    g = 0;
                if(g > 255)
                    g = 255;

                mix_delta *= -1;

                r = r + main_delta;
                if((r > 255) || (r <= 0))
                {
                    if(r <= 0)
                        r = 0;
                    if(r > 255)
                        r = 255;

                    main_delta *= -1;
                    mix_state = 1;
                }
            }
            break;
            //mix blue
        case 1:
            b += mix_delta;
            if((b > 255) || (b <= 0))
            {
                if(b <= 0)
                    b = 0;
                if(b > 255)
                    b = 255;

                mix_delta *= -1;

                r = r + main_delta;
                if((r > 255) || (r <= 0))
                {
                    if(r <= 0)
                        r = 0;
                    if(r > 255)
                        r = 255;

                    main_delta *= -1;
                    mix_state = 2;
                }
            }
            break;
            //mix red and blue
        case 2:
            g += mix_delta;
            b += mix_delta;
            if((g > 255) || (g <= 0))
            {
                if(b <= 0)
                    b = 0;
                if(b > 255)
                    b = 255;

                if(g <= 0)
                    g = 0;
                if(g > 255)
                    g = 255;

                mix_delta *= -1;

                r = r + main_delta;
                if((r > 255) || (r < 0))
                {
                    if(r <= 0)
                        r = 0;
                    if(r > 255)
                        r = 255;

                    main_delta *= -1;
                    mix_state = 0;
                }
            }
            break;
        }


        //printf("(%3d, %3d, %3d)\n", r, g, b);

        for (int row = 0; row < 16; ++row)
        {
            for (int col = 0; col < 16; ++col)
            {
                ws2812b_set_pixel(col, row, r, g, b);
            }
        }
    }
}

void AnimateMix::column_run(int *row)
{
    *row += row_delta;

    if(*row > 15)
    {
        row_delta *= -1;
        *row = 14;
    }
    if(*row < 0)
    {
        row_delta *= -1;
        *row = 1;
    }
}
