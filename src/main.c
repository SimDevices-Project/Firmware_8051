#include "ch552.h"
#include "sys.h"
#include "rom.h"
#include "usb.h"
#include "bsp.h"
#include "debounce.h"

void __usbDeviceInterrupt() __interrupt(INT_NO_USB) __using(1); // USB中断定义
// extern uint8_t FLAG;
extern uint8_t sysMsCounter;
extern uint8_t HIDKey[];

void __tim2Interrupt() __interrupt(INT_NO_TMR2) __using(2);

uint8_t prevKey = 0;
uint8_t activeKey;

/** @brief 标准8字节USB键盘报表，控制键字节 */
uint8_t ctrlKey;

void main()
{
  uint8_t i;

  sysClockConfig();
  delay_ms(20);

#if defined(HAS_ROM)
  romInit();
  delay_ms(300);
#endif

  sysLoadConfig();
  SysConfig *cfg = sysGetConfig();

  usbDevInit();
  debounceInit();

  EA = 1; // 启用中断

  delay_ms(300);

  usbReleaseAll();
  requestHIDData();

  sysTickConfig();

  sysMsCounter = 0;
  while (1)
  {
    while (sysMsCounter--)
    {
      debounceUpdate();
    }

    activeKey = 0;
    for (i = 0; i < KEY_COUNT; i++)
    {
      activeKey <<= 1;
      activeKey |= keyState(i);
    }

    if (prevKey != activeKey)
    {
      ctrlKey = 0;
      usbReleaseAll();

      for (i = 0; i < KEY_COUNT; i++)
      {
        if (cfg->keyConfig[i].mode == KeyboardMouse)
        {
          if (keyState(i))
          {
            ctrlKey |= cfg->keyConfig[i].codeLH;
            usbSetKeycode(i + 2, cfg->keyConfig[i].codeLL);
          }
        }
      }

      usbSetKeycode(0, ctrlKey);
      usbSetKeycode(1, 0);

      usbPushKeydata();

      prevKey = activeKey;
    }
  }
}