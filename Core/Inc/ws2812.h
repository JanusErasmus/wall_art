#ifndef WS2812B_H_
#define WS2812B_H_
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void ws2812b_init();
int ws2812b_paint();
void ws2812b_clear();
void ws2812b_set_pixel(uint8_t row, uint16_t column, uint8_t red, uint8_t green, uint8_t blue);

#ifdef __cplusplus
}
#endif
#endif /* WS2812B_H_ */
