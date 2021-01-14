#include "ch552.h"
#include "sys.h"
#include "rom.h"
#include "bsp.h"

void main()
{
  sysClockConfig();
  delay_ms(5);

#if defined(HAS_ROM)
  romInit();
  delay(500);
#endif

  sysTickConfig();
  usbDevInit();

  EA = 1; // 启用中断

  usbReleaseAll();
  requestHIDData();

  while (1)
  {
  }
}