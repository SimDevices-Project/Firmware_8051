#include "debounce.h"

#include <string.h>
#include "sys.h"

#ifdef TOUCH_COUNT
#include "touchkey.h"
#endif

static __data debounce_type debBuffer[DEBOUNCE_SIZE];  // 去抖缓冲
static __data uint8_t debResult[DEBOUNCE_SIZE];        // 按键状态
#define __set_pin(k) BT##k = 1;
#define __ch(k) CH##k;
#define __debounce_pin(k)                                            \
  debBuffer[k - 1] <<= 1;                                            \
  debBuffer[k - 1] |= BT##k;                                         \
  if (debBuffer[k - 1] == 0xFE || debBuffer[k - 1] == 0x00) {        \
    debResult[k - 1] = 1;                                            \
  } else if (debBuffer[k - 1] == 0x01 || debBuffer[k - 1] == 0xFF) { \
    debResult[k - 1] = 0;                                            \
  }
#define __debounce_touchkey(k)                                       \
  debBuffer[k - 1] <<= 1;                                            \
  debBuffer[k - 1] |= (Touch_State >> (k + 1)) & 1;                     \
  if (debBuffer[k - 1] == 0xFE || debBuffer[k - 1] == 0x00) {        \
    debResult[k - 1] = 1;                                            \
  } else if (debBuffer[k - 1] == 0x01 || debBuffer[k - 1] == 0xFF) { \
    debResult[k - 1] = 0;                                            \
  }

uint8_t i = 0;

/*
 * 更新计算去抖
 */
void debounceUpdate() {
#ifndef TOUCH_COUNT
#if defined(SIM_KEY)
  __debounce_pin(1);
#else
  __debounce_pin(1) __debounce_pin(2)
#if (defined(SIMPAD_V2_AE) || defined(SIMPAD_V2))
  __debounce_pin(3);
  __debounce_pin(4);
  __debounce_pin(5);
#elif (defined(SIMPAD_NANO_AE) || defined(SIMPAD_NANO))
      __debounce_pin(3);
#endif
#endif
#elif defined SIMPAD_TOUCH
  __debounce_touchkey(1);
  __debounce_touchkey(2);
  __debounce_touchkey(3);
  __debounce_touchkey(4);
#endif
}

__bit isKeyActive(uint8_t i) {
  return debResult[i];  // & 0x01;
}

void debounceInit() {
#ifndef SIMPAD_TOUCH
#if defined(SIM_KEY)
  __set_pin(1);
#else
  __set_pin(1);
  __set_pin(2);
#if (defined(SIMPAD_V2_AE) || defined(SIMPAD_V2))
  __set_pin(3);
  __set_pin(4);
  __set_pin(5);
#elif (defined(SIMPAD_NANO_AE) || defined(SIMPAD_NANO))
  __set_pin(3);
#elif (defined(SIMPAD_TOUCH))
  __set_pin(3);
  __set_pin(4);
#endif
#endif
#endif
  memset(debBuffer, 0xFF, DEBOUNCE_SIZE);
  memset(debResult, 0x00, DEBOUNCE_SIZE);
}
