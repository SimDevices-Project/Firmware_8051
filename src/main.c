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

#if KEY_COUNT <= 8
uint8_t prevKey = 0;
uint8_t activeKey;
#elif KEY_COUNT <= 16
uint16_t prevKey = 0;
uint16_t activeKey;
#endif

/** @brief 标准8字节USB键盘报表，控制键字节 */
uint8_t ctrlKey;

void main()
{
  uint8_t i, j;

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
      activeKey |= isKeyActive(i);
    }

    if (prevKey != activeKey)
    {
      prevKey ^= activeKey;
      for (i = 0; i < KEY_COUNT; i++)
      {
        if ((prevKey >> i) & 0x01)
        {
          usbReleaseAll();
          switch (cfg->keyConfig[i].mode)
          {
          case KeyboardMouse:
            ctrlKey = 0;
            for (j = 0; j < KEY_COUNT; j++)
            {
              if (cfg->keyConfig[j].mode == KeyboardMouse)
              {
                if ((activeKey >> j) & 0x01)
                {
                  ctrlKey |= cfg->keyConfig[j].codeLH;
                  usbSetKeycode(j + 2, cfg->keyConfig[j].codeLL);
                }
              }
            }

            usbSetKeycode(0, 1);
            usbSetKeycode(1, ctrlKey);
            break;
          case Media:
            for (j = 0; j < KEY_COUNT; j++)
            {
              if (cfg->keyConfig[j].mode == Media)
              {
                if ((activeKey >> j) & 0x01)
                {
                  usbSetKeycode(2, cfg->keyConfig[j].codeLH);
                  usbSetKeycode(1, cfg->keyConfig[j].codeLL);
                  break;
                }
              }
            }
            usbSetKeycode(0, 2);
            break;
          default:
            break;
          }
          usbPushKeydata();
        }
      }
      prevKey = activeKey;
    }
  }
}