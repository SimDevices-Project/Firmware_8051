#include "rgb.h"

#ifdef LEDTYPE_WS2812
static uint32_i rgbGRBData[LED_COUNT];

void __ws2812_init() {
  LED_IO = 0;
}
void __ws2812_send(uint32_t value) {
  uint8_t i, j;
  for (i = 0; i < 24; i++) {
    if (value & 0x800000) {
      LED = 1;
      for (j = 4; j > 0; j--)
        __asm__("nop");
      LED = 0;
      for (j = 1; j > 0; j--)
        __asm__("nop");
    } else {
      LED = 1;
      for (j = 1; j > 0; j--)
        __asm__("nop");
      LED = 0;
      for (j = 4; j > 0; j--)
        __asm__("nop");
    }
    value <<= 1;
  }
}
#endif

void rgbInit() {
#ifdef LEDTYPE_WS2812
  __ws2812_init();
#elif defined(LEDTYPE_RGB)
  return;
#endif
  return;
}

void rgbSet(uint8_t index, uint32_t value) {
  rgbGRBData[index] = ((value & 0x00FF00) << 8) | ((value & 0xFF0000) >> 8) |
                      (value & 0x0000FF);
}

void rgbPush() {
#ifdef LEDTYPE_WS2812
  E_DIS = 1;
  __ws2812_init();
  uint8_t i;
  for (i = 0; i < LED_COUNT; i++)
    __ws2812_send(rgbGRBData[i]);
  E_DIS = 0;
#endif
}