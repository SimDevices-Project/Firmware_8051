#include "bsp.h"
#include "ch552.h"
#include "debounce.h"
#include "rgb.h"
#include "rom.h"
#include "sys.h"
#include "usb.h"

#ifdef HAS_MOTOR
#include "motor.h"
#endif

#ifdef TOUCH_COUNT
#include "touchkey.h"
#endif

void __usbDeviceInterrupt() __interrupt(INT_NO_USB) __using(1); // USB中断定义
#ifdef TOUCH_COUNT
void __TK_int_ISR() __interrupt(INT_NO_TKEY) __using(1); // TouchKey中断定义
#endif

void __tim2Interrupt() __interrupt(INT_NO_TMR2) __using(2);

extern uint8_t sysMsCounter;

#if KEY_COUNT <= 8
uint8_t prevKey = 0; // 上一次扫描时的按键激活状态记录
uint8_t activeKey;   // 最近一次扫描时的按键激活状态记录
#elif KEY_COUNT <= 16
uint16_t prevKey = 0; // 上一次扫描时的按键激活状态记录
uint16_t activeKey;   // 最近一次扫描时的按键激活状态记录
#endif

/**
 * @brief 标准8字节USB键盘报表，控制键字节；兼做鼠标
 */
uint8_t ctrlKey;

/** @brief 游戏手柄报表，16个按键，16比特 */
uint8_t controllerKeyH = 0, controllerKeyL = 0;
SysConfig *cfg;

void checkKey()
{
  uint8_t i, j;
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
#ifdef HAS_MOTOR
        // 如果第i个键是被更改的，并且这个操作是激活
        if ((activeKey >> i) & 0x01)
        {
          activeMotor(5000);
        }
#endif
        usbReleaseAll();
        switch (cfg->keyConfig[i].mode)
        {
        // 标准键盘
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

          usbPushKeydata(); // 发送 HID1 数据包
          break;
        // 多媒体按键
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

          usbPushKeydata(); // 发送 HID1 数据包
          break;
        // 鼠标
        case Mouse:

          ctrlKey = 0;
          for (j = 0; j < KEY_COUNT; j++)
          {
            if (cfg->keyConfig[j].mode == Mouse)
            {
              if ((activeKey >> j) & 0x01)
              {
                ctrlKey |= cfg->keyConfig[j].codeHL;
              }
            }
          }

          usbSetMousecode(0, 1); // 报表位0设置为1，即标准鼠标

          usbSetMousecode(1, ctrlKey); // 鼠标操作

          // 如果第i个键是被更改的，并且这个操作是激活
          // 因为是相对输入报表，所以只输入一次
          if ((activeKey >> i) & 0x01)
          {
            j = (activeKey >> i) & 0x01;
            usbSetMousecode(2, cfg->keyConfig[j].codeLH); // 鼠标X位移
            usbSetMousecode(3, cfg->keyConfig[j].codeLL); // 鼠标Y位移
            usbSetMousecode(4, cfg->keyConfig[j].codeHH); // 鼠标滚轮位移
          }

          usbPushMousedata();

          break;
        // 手柄按钮 及 手柄摇杆
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

void main()
{
  sysClockConfig();
  delay_ms(20);

#if defined(HAS_ROM)
  romInit();
  delay_ms(300);
#endif

#ifdef HAS_MOTOR
  motorInit();
#endif

#ifdef TOUCH_COUNT
  TK_Init(BIT4 | BIT5 | BIT6 | BIT7, 0, 1); /* Choose TIN2, TIN3, TIN4, TIN5 for touchkey input. enable interrupt. */
  TK_SelectChannel(0);                      /* NOTICE: ch is not compatible with the IO actually. */
#endif

  sysLoadConfig();
  cfg = sysGetConfig();

  usbDevInit();
  debounceInit();
  // rgbInit();

  EA = 1; // 启用中断

  delay_ms(100);

  usbReleaseAll();
  requestHIDData();

  sysTickConfig();

  sysMsCounter = 0;
  while (1)
  {
    // 定时器毫秒区间
    while (sysMsCounter--)
    {
#ifdef HAS_MOTOR
      motorUpdate();
#endif
      debounceUpdate();
      // rgbPush();
    }

    if (hasHIDData())
    {
      hid_run();
    }

    checkKey();
  }
}