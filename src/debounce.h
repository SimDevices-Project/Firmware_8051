#ifndef __DEBOUNCE_H
#define __DEBOUNCE_H

#include "ch552.h"
#include "bsp.h"

typedef uint8_t debounce_type;
#define DEBOUNCE_SIZE KEY_COUNT

void debounceInit();
void debounceUpdate();
__bit keyState(uint8_t i);

#endif