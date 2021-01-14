#include "ch552.h"
#include "sys.h"
#include "rom.h"
#include "usb.h"
#include "bsp.h"

void __usbDeviceInterrupt() __interrupt (INT_NO_USB) __using (1);       // USB中断定义
extern uint8_t FLAG;

void __tim2Interrupt() __interrupt (INT_NO_TMR2) __using (2);   

void main()
{
  sysClockConfig();
  delay_ms(5);

#if defined(HAS_ROM)
  romInit();
  delay_ms(500);
#endif

  sysTickConfig();
  usbDevInit();

  EA = 1; // 启用中断

  delay_ms(500);

  usbReleaseAll();
  requestHIDData();

  while (1)
  {
  }
}