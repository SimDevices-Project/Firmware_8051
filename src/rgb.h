#ifndef __RGB_H
#define __RGB_H

#include "bsp.h"
#include "ch552.h"


#if LED_COUNT > 0
#ifdef LED
#define LED_IO LED
#define LEDTYPE_WS2812
#else
#define LEDTYPE_RGB
#endif

#endif

void rgbInit();
void rgbSet(uint8_t index, uint32_t value);
void rgbPush();

#endif