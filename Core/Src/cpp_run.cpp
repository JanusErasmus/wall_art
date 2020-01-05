#include "ws2812.h"

#include "main.h"
#include "led_matrix.h"
#include "Aurora/Effects.h"

LEDmatrix matrix;
Effects effects(&matrix);

extern "C" {
void cpp_init()
{
}

uint32_t tick = 0;

void cpp_run()
{
    //disco.run();

    if(tick < HAL_GetTick())
    {
        tick = HAL_GetTick() + 20;
        effects.run();
    }

    if(!HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin))
    {
        effects.setNextPattern();

        while(!HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin))
        {
            HAL_Delay(500);
        }
    }
}

void pattern(uint8_t argc, char **argv)
{
    effects.setNextPattern();
}

void palette(uint8_t argc, char **argv)
{
    effects.CyclePalette();
}
}
