#include "bsp.h"
#include "ch552.h"
#include "debounce.h"
#include "rgb.h"
#include "rom.h"
#include "sys.h"
#include "usb.h"
#ifdef TOUCH_COUNT
#include "touchkey.h"
#endif

void __usbDeviceInterrupt() __interrupt(INT_NO_USB) __using(1); // USB中断定义
#ifdef TOUCH_COUNT
void __TK_int_ISR() __interrupt(INT_NO_TKEY) __using(1); // TouchKey中断定义
#endif

// extern uint8_t FLAG;

extern uint8_t sysMsCounter;
extern uint8_t HIDKey[];

void __tim2Interrupt() __interrupt(INT_NO_TMR2) __using(2);

#if KEY_COUNT <= 8
uint8_t prevKey = 0; // 上一次扫描时的按键激活状态记录
uint8_t activeKey;   // 最近一次扫描时的按键激活状态记录
#elif KEY_COUNT <= 16
uint16_t prevKey = 0; // 上一次扫描时的按键激活状态记录
uint16_t activeKey;   // 最近一次扫描时的按键激活状态记录
#endif

/** @brief 标准8字节USB键盘报表，控制键字节 */
uint8_t ctrlKey;

/** @brief 游戏手柄报表，16个按键，16比特 */
uint8_t controllerKeyH = 0, controllerKeyL = 0;

void main()
{
  uint8_t i, j;
  // uint16_t tmp;

#ifdef MOTOR
  uint16_t motorDelay = 0;
  MOTOR               = 0;
#endif

  sysClockConfig();
  delay_ms(20);

#if defined(HAS_ROM)
  romInit();
  delay_ms(300);
#endif

#ifdef TOUCH_COUNT
  TK_Init(BIT4 | BIT5 | BIT6 | BIT7, 0, 1); /* Choose TIN2, TIN3, TIN4, TIN5 for touchkey input. enable interrupt. */
  TK_SelectChannel(0);                      /* NOTICE: ch is not compatible with the IO actually. */
#endif

  sysLoadConfig();
  SysConfig *cfg = sysGetConfig();

  usbDevInit();
  debounceInit();
  // rgbInit();

  EA = 1; // 启用中断

  delay_ms(100);

  usbReleaseAll();
  requestHIDData();

  sysTickConfig();

  // tmp = romRead16e(0x00);
  // romWrite16e(0x00, 0x8088);
  // // 灯光测试Demo
  // if (tmp == 0x8088)
  // {
  //   rgbSet(0, 0x00FFFFFF);
  // }
  // else
  // {
  //   rgbSet(0, 0x00FF0000);
  // }
  // rgbPush();

  rgbSet(0, 0x00FFFFFF);
  rgbSet(1, 0x00FFFFFF);
  rgbSet(2, 0x00FFFFFF);
  rgbSet(3, 0x00FFFFFF);
  rgbPush();

  sysMsCounter = 0;
  while (1)
  {
    while (sysMsCounter--)
    {
#ifdef MOTOR
      if (motorDelay)
      {
        motorDelay--;
        if (motorDelay == 0)
        {
          MOTOR = 0;
        }
      }
#endif
      debounceUpdate();
      // rgbPush();
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
        // 如果第i个键是被更改的
        if ((prevKey >> i) & 0x01)
        {
#ifdef MOTOR
          // 如果第i个键是被更改的，并且这个操作是激活
          if ((activeKey >> i) & 0x01)
          {
            motorDelay = 5000;
            MOTOR      = 1;
          }
#endif
          usbReleaseAll();
          switch (cfg->keyConfig[i].mode)
          {
          // 当被激活的按键类型是标准键盘按键时
          case Keyboard:
            ctrlKey = 0;
            for (j = 0; j < KEY_COUNT; j++)
            {
              if (cfg->keyConfig[j].mode == Keyboard)
              {
                if ((activeKey >> j) & 0x01)
                {
                  ctrlKey |= cfg->keyConfig[j].codeLH;
                  usbSetKeycode(j + 2, cfg->keyConfig[j].codeLL);
                }
              }
            }

            usbSetKeycode(0, 1); // 报表位0设置为1，即标准键盘
            usbSetKeycode(1, ctrlKey);

            rgbSet(0, 0x00FF0000);
            rgbPush();

            usbPushKeydata(); // 发送 HID1 数据包
            break;
          // 当被激活的按键类型是多媒体按键时
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
            usbSetKeycode(0, 2); // 报表位0设置为2，即多媒体键盘

            rgbSet(0, 0x0000FF00);
            rgbPush();

            usbPushKeydata(); // 发送 HID1 数据包
            break;
          case Mouse:
          
            rgbSet(0, 0x000000FF);
            rgbPush();

            break;
          case GamepadButton:

            controllerKeyH = 0;
            controllerKeyL = 0;
            for (j = 0; j < KEY_COUNT; j++)
            {
              if (cfg->keyConfig[j].mode == GamepadButton)
              {
                if ((activeKey >> j) & 0x01)
                {
                  controllerKeyH |= (cfg->keyConfig[j].codeLH);
                  controllerKeyL |= (cfg->keyConfig[j].codeLL);
                }
              }
            }

            usbSetKeycode(0, 3); // 报表位0设置为3，即游戏手柄

            if (activeKey == 0)
            {
              rgbSet(0, 0x00FFFFFF);
              rgbSet(1, 0x00FFFFFF);
              rgbSet(2, 0x00FFFFFF);
              rgbSet(3, 0x00FFFFFF);
            }
            else
            {
              rgbSet(0, 0x00FF0000);
              rgbSet(1, 0x0000FF00);
              rgbSet(2, 0x000000FF);
              rgbSet(3, 0x00FF0000);
            }
            rgbPush();

            usbSetKeycode(1, controllerKeyH);
            usbSetKeycode(2, controllerKeyL);

            usbSetKeycode(3, 0x00);
            usbSetKeycode(4, 0x00);
            usbSetKeycode(5, 0x00);
            usbSetKeycode(6, 0x00);

            usbPushKeydata(); // 发送 HID1 数据包
            break;
          default:
            break;
          }
        }
      }
      prevKey = activeKey;
    }
  }
}