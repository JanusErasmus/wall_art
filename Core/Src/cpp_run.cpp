#include "animate_disco.h"
#include "Adafruit_GFX.h"
#include "ws2812.h"

#include "main.h"

#include "PatternBounce.h"
#include "PatternFlock.h"
#include "PatternSwirl.h"
#include "PatternSpiral.h"
#include "PatternSpiro.h"
#include "PatternWave.h"
#include "PatternRadar.h"
#include "PatternPendulumWave.h"
#include "PatternIncrementalDrift.h"
#include "PatternFlowField.h"
#include "PatternAttract.h"
#include "PatternCube.h"
#include "Effects.h"

Matrix matrix;
Effects effects(&matrix);

AnimateDisco disco(&effects);
PatternBounce bounce(&effects);
PatternFlock flock(&effects);
PatternSwirl swirl(&effects);
PatternSpiral spiral(&effects); //TODO NOT working
PatternSpiro spiro(&effects); //TODO NOT working
PatternWave wave(&effects);
PatternRadar radar(&effects);
PatternPendulumWave p_wave(&effects);
PatternIncrementalDrift drift(&effects);
PatternFlowField flow(&effects);//TODO NOT working
PatternAttract attract(&effects);
PatternCube cube(&effects); //TODO NOT working

Drawable *patterns[] = {
        &disco,
        &bounce,
        &flock,
        &swirl,
        //&spiral,
        //&spiro,
        &wave,
        &radar,
        &p_wave,
        &drift,
        &flow,
        &attract,
        //&cube,
        0
};

int curr_pattern = 0;
void setNextPattern()
{
    curr_pattern++;
    if(!patterns[curr_pattern])
        curr_pattern = 0;

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
}
