#include "sys.h"

#include "bsp.h"
#include "ch552.h"
#include "rom.h"

#include <string.h>

// volatile uint32_i sysTickCount = 0; // 系统刻计数器
static __xdata SysConfig sysConfig;  // 系统配置存储
uint8_t sysMsCounter = 0;            // 系统ms计数器

/*
 * 系统时钟配置
 */
void sysClockConfig() {
  SAFE_MOD = 0x55;
  SAFE_MOD = 0xAA;
  CLOCK_CFG |= bOSC_EN_INT;

#if OSC_EN_XT
  SAFE_MOD = 0x55;
  SAFE_MOD = 0xAA;
  CLOCK_CFG |= bOSC_EN_XT;    // 使能外部晶振
  CLOCK_CFG &= ~bOSC_EN_INT;  // 关闭内部晶振
#endif

  SAFE_MOD = 0x55;
  SAFE_MOD = 0xAA;
  CLOCK_CFG &= ~MASK_SYS_CK_SEL;

  SAFE_MOD = 0x55;
  SAFE_MOD = 0xAA;
#if FREQ_SYS == 24000000
  CLOCK_CFG |= 0x06;  // 24MHz
#endif
#if FREQ_SYS == 16000000
  CLOCK_CFG |= 0x05;  // 16MHz
#endif
#if FREQ_SYS == 12000000
  CLOCK_CFG |= 0x04;  // 12MHz
#endif
#if FREQ_SYS == 6000000
  CLOCK_CFG |= 0x03;  // 6MHz
#endif
#if FREQ_SYS == 3000000
  CLOCK_CFG |= 0x02;  // 3MHz
#endif
#if FREQ_SYS == 750000
  CLOCK_CFG |= 0x01;  // 750KHz
#endif
#if FREQ_SYS == 187500
  CLOCK_CFG |= 0x00;  // 187.5KHz
#endif

  SAFE_MOD = 0x00;
}

/*
 * 微秒延时
 */
void delay_us(uint16_t n) {
#ifdef FREQ_SYS
#if FREQ_SYS <= 6000000
  n >>= 2;
#endif
#if FREQ_SYS <= 3000000
  n >>= 2;
#endif
#if FREQ_SYS <= 750000
  n >>= 4;
#endif
#endif
  while (n) {    // total = 12~13 Fsys cycles, 1uS @Fsys=12MHz
    ++SAFE_MOD;  // 2 Fsys cycles, for higher Fsys, add operation here
#ifdef FREQ_SYS
#if FREQ_SYS >= 14000000
    ++SAFE_MOD;
#endif
#if FREQ_SYS >= 16000000
    ++SAFE_MOD;
#endif
#if FREQ_SYS >= 18000000
    ++SAFE_MOD;
#endif
#if FREQ_SYS >= 20000000
    ++SAFE_MOD;
#endif
#if FREQ_SYS >= 22000000
    ++SAFE_MOD;
#endif
#if FREQ_SYS >= 24000000
    ++SAFE_MOD;
#endif
#endif
    --n;
  }
}

/*
 * 毫秒延时
 */
void delay_ms(uint16_t n) {
  while (n) {
    delay_us(1000);
    --n;
  }
}

/*
 * 刻计数器更新中断 - 定时器2，去抖计算
 * 需要注意的是，非总线LED驱动也使用此中断，这种情况下刻计数器更新在 rgb.c
 * 内实现
 */
#if !defined(SIMPAD_V2)
void __tim2Interrupt() __interrupt(INT_NO_TMR2) __using(2) {
  if (TF2) {
    TF2 = 0;
    // sysTickCount += 1;
    sysMsCounter += 1;
  }
}
#endif

/*
 * 系统刻计数初始化 - 定时器2
 */
void sysTickConfig() {
#if !defined(SIMPAD_V2)
  T2MOD &= ~bT2_CLK;
  C_T2 = 0;
  RCAP2 = T2COUNT = 0xFFFF - (uint16_t)(FREQ_SYS / 12L / 1000L);  // 1000Hz
  TR2 = 1;
  ET2 = 1;
#endif
}

/*
 * 获取当前刻
 */
// uint32_t sysGetTickCount() {
//     return sysTickCount;
// }

/*
 *  获取芯片 ID 号并校验
 */
uint32_t getChipID(void) {
  uint8_t d0, d1;
  uint16_t xl, xh;
  E_DIS = 1;  // 避免进入中断
  d0 = *(puint8_c)(ROM_CHIP_ID_LO + 0);
  d1 = *(puint8_c)(ROM_CHIP_ID_LO + 1);  // ID号低字
  xl = (d1 << 8) | d0;
  d0 = *(puint8_c)(ROM_CHIP_ID_LO + 2);
  d1 = *(puint8_c)(ROM_CHIP_ID_LO + 3);  // ID号高字
  xh = (d1 << 8) | d0;
  E_DIS = 0;
  return (((uint32_t)xh << 16) | xl);
}

/*
 *  转换 16 进制数字为字符
 */
uint8_t hexToChar(uint8_t bHex) {
  bHex = bHex & 0x0F;
  if (bHex <= 9) {
    bHex += 0x30;
  } else {
    bHex += 0x37;
  }
  return bHex;
}

/*
 * 通过RGB565数据和一个字节的扩展数据得到完整的RGB888
 */
// uint32_t sysGetRGB(uint16_t color, uint8_t extend) {
//     uint32_t val = 0;
//     val = ((uint32_t) ((color & 0xF800) | ((extend & 0xE0) << 3))) << 8;
//     val |= ((uint32_t) ((color & 0x07E0) | (extend & 0x18))) << 5;
//     val |= ((color & 0x001F) << 3) | (extend & 0x07);
//     return val;
// }

#define KEY_CFG(i) (sysConfig.keyConfig[i])
#define LED_CFG(i) (sysConfig.ledConfig[i])

/*
 * 加载系统配置
 */
void sysLoadConfig() {
  memset(&sysConfig, 0x00, sizeof(SysConfig));
  uint8_t i, point = 0;
  uint16_t tmp = 0, addr = 0;
  for (i = 0; i < KEY_COUNT; i++) {
    KEY_CFG(i).mode = (KeyMode)romRead8i(point++);
    // tmp = romRead16i(0x08 + i * 4);         // BTx
    // KEY_CFG(i).marco = (tmp & 0x8000) != 0;
    KEY_CFG(i).codeHH = romRead8i(point++);
    KEY_CFG(i).codeHL = romRead8i(point++);
    // tmp = romRead16i(0x0A + i * 4);     // BTxL
    KEY_CFG(i).codeLH = romRead8i(point++);
    KEY_CFG(i).codeLL = romRead8i(point++);
  }

#if LED_COUNT > 0
  for (i = 0; i < LED_COUNT; i++) {
    LED_CFG(i).mode = romRead8i(point++);
    LED_CFG(i).value0 = romRead8i(point++);
    LED_CFG(i).value1 = romRead8i(point++);
    LED_CFG(i).value2 = romRead8i(point++);
    LED_CFG(i).bind = romRead8i(point++);
    LED_CFG(i).trigger = (LEDTriggerMode)romRead8i(point++);
  }
#endif
}

/*
 * 获取系统配置指针
 */
SysConfig* sysGetConfig() {
  return &sysConfig;
}
