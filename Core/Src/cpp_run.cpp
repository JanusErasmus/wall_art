#include "animate_disco.h"
#include "Adafruit_GFX.h"
#include "ws2812.h"

AnimateDisco disco;
GFXcanvas16 matrix(16, 16);

void sync_GFX()
{
    uint16_t *buffer = matrix.getBuffer();

    for (int x = 0; x < 16; ++x)
    {
        for (int y = 0; y < 16; ++y)
        {
            uint8_t r = (buffer[x + y * 16] & 0xF800) >> 8;
            uint8_t g = (buffer[x + y * 16] & 0x07E0) >> 3;
            uint8_t b = (buffer[x + y * 16] & 0x1F) << 3;
            ws2812b_set_pixel(x, y, r, g, b);
        }
    }
    ws2812b_paint();
}

extern "C" {
void cpp_init()
{
    matrix.setRotation(1);
    matrix.fillCircle(7, 7, 7, 0x001f);
    matrix.fillCircle(7, 7, 3, 0xf800);
    matrix.fillCircle(7, 7, 1, 0x07e0);



    sync_GFX();
}

uint32_t tick = 0;

void cpp_run()
{
    //disco.run();

    if(tick < HAL_GetTick())
    {
        tick = HAL_GetTick() + 1000;
    }
}
}
