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
extern Effects *effects;

AnimateDisco disco;
PatternBounce bounce;
PatternFlock flock;
PatternSwirl swirl;
//PatternSpiral spiral; NOT
//PatternSpiro spiro; NOT
PatternWave wave;
PatternRadar radar;
PatternPendulumWave p_wave;
PatternIncrementalDrift drift;
//PatternFlowField flow; NOT
PatternAttract attract;
PatternCube cube;
extern "C" {
void cpp_init()
{
    matrix = new Matrix();
    effects = new Effects();
    effects->Setup(matrix->getBuffer());

    flock.start();
    bounce.start();
    swirl.start();
    wave.start();
    radar.start();
    p_wave.start();
    drift.start();
    attract.start();
    cube.start();

}

uint32_t tick = 0;

void cpp_run()
{
    //disco.run();

    if(tick < HAL_GetTick())
    {
        tick = HAL_GetTick() + 20;
        //bounce.drawFrame();
        //flock.drawFrame();
        //swirl.drawFrame();
        //spiral.drawFrame();
        //wave.drawFrame();
        //radar.drawFrame();
        //p_wave.drawFrame();
        //drift.drawFrame();
        //flow.drawFrame();
        attract.drawFrame();
        //cube.drawFrame();
    }
}
}
