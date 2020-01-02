#include "ws2812.h"
#include "led_matrix.h"
#include "main.h"

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

LEDmatrix matrix;
Effects effects(&matrix);
Boid boids[16];

AnimateDisco disco(&effects);
PatternBounce bounce(&effects, boids);
PatternFlock flock(&effects, boids);
PatternSwirl swirl(&effects);
PatternSpiral spiral(&effects);
PatternSpiro spiro(&effects);
PatternWave wave(&effects);
PatternRadar radar(&effects);
PatternPendulumWave p_wave(&effects);
PatternIncrementalDrift drift(&effects);
PatternFlowField flow(&effects, boids);
PatternAttract attract(&effects, boids);
PatternCube cube(&effects);

Drawable *patterns[] = {
        &cube,
        &attract,
        &flock,
        &bounce,
        &flow,
        &spiro,
        &spiral,
        &swirl,
        &wave,
        &radar,
        //&p_wave,
        &drift,
        &disco,
        0
};

int curr_pattern = 0;
void setNextPattern()
{
    curr_pattern++;
    if(!patterns[curr_pattern])
        curr_pattern = 0;

    printf("Starting pattern: %s\n", patterns[curr_pattern]->name);
    patterns[curr_pattern]->start();
}

extern "C" {
void cpp_init()
{
    patterns[curr_pattern]->start();
}

uint32_t tick = 0;

void cpp_run()
{
    //disco.run();

    if(tick < HAL_GetTick())
    {
        tick = HAL_GetTick() + 20;
        patterns[curr_pattern]->drawFrame();
    }

    if(!HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin))
    {
        setNextPattern();

        while(!HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin))
        {
            HAL_Delay(500);
        }
    }
}

void pattern(uint8_t argc, char **argv)
{
    setNextPattern();
}

void palette(uint8_t argc, char **argv)
{
    effects.CyclePalette();
}
}
