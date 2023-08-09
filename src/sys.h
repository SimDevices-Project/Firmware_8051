#ifndef __SYS_H
#define __SYS_H

#include <stdint.h>
#include <stdbool.h>

#include "bsp.h"

#define FREQ_SYS 24000000 // 系统主频24MHz
#define OSC_EN_XT 0       // 外部晶振使能，默认开启内部晶振
#define MARCO_MAX 48      // 单个宏指令大小

void sysClockConfig();     // 时钟选择和配置
void delay_us(uint16_t n); // 以uS为单位延时
void delay_ms(uint16_t n); // 以mS为单位延时

void sysTickConfig();
// uint32_t sysGetTickCount();

uint32_t getChipID();
uint8_t hexToChar(uint8_t bHex);

typedef enum // 按键模式
{
    Keyboard = 0x00,      // 键鼠
    Media = 0x01,              // 媒体
    Roller = 0x02,             // 滚轮
    GamepadButton = 0x03,      // 手柄按键
    Mouse = 0x04,          // 鼠标移动
    Absolute = 0x05,           // 绝对定位
    GamepadRocker = 0x06,      // 手柄摇杆
    MacroPressDown = 0xF0,     // 按下触发一次宏
    MacroPressContinue = 0xF1, // 按下持续
    MacroPressSwitch = 0xF2    // 按下启/停
} KeyMode;

#ifndef __LED_COLOR_MODE_LIST_
#define __LED_COLOR_MODE_LIST_

#define LED_COLOR_MODE_CONSTANT = 0x00           // 固定颜色
#define LED_COLOR_MODE_RAINBOW = 0x10            // 标准彩虹色表
#define LED_COLOR_MODE_COLOR_GROUP_LOOP = 0x20   // 色表循环
#define LED_COLOR_MODE_COLOR_GROUP_RANDOM = 0x30 // 色表循环
#define LED_COLOR_MODE_RANDOM = 0x40             // 随机颜色

#endif

#ifndef __LED_ACTIVE_MODE_LIST_
#define __LED_ACTIVE_MODE_LIST_

#define LED_ACTIVE_MODE_ALWAYS_ON = 0x00      // 常亮
#define LED_ACTIVE_MODE_FADE_IN = 0x01        // 渐显
#define LED_ACTIVE_MODE_FADE_OUT = 0x02       // 渐隐
#define LED_ACTIVE_MODE_BLINK = 0x03          // 闪烁
#define LED_ACTIVE_MODE_BLINK_ONCE = 0x04     // 闪烁一次
#define LED_ACTIVE_MODE_GRADIENT = 0x05       // 渐变
#define LED_ACTIVE_MODE_GRADIENT_ONCE = 0x06  // 渐变一项
#define LED_ACTIVE_MODE_BREATHING = 0x07      // 呼吸灯
#define LED_ACTIVE_MODE_BREATHING_ONCE = 0x08 // 呼吸一次

#endif

/**
 * @brief 按键配置结构体
 */
typedef struct
{
    KeyMode mode;
    uint8_t codeHH;
    uint8_t codeHL;
    uint8_t codeLH;
    uint8_t codeLL;
    // uint8_t program[MARCO_MAX];
    // uint16_t length;
} KeyConfig;

typedef enum // 灯光模式
{
    AlwaysOn = 0x00,      // 不做管理，常亮
    PressDown = 0x01,     // 按下才触发
    PressUp = 0x02,       // 抬起才触发
    PressContinue = 0x03, // 按下时持续触发
} LEDTriggerMode;

typedef struct
{ // LED配置结构体
    // bool marco;
    uint8_t mode;
    // uint8_t program[MARCO_MAX];
    uint8_t value0;
    uint8_t value1;
    uint8_t value2;
    uint8_t bind;
    LEDTriggerMode trigger;
} LEDConfig;

/**
 * @brief 系统设置结构体
 */
typedef struct
{
    KeyConfig keyConfig[KEY_COUNT];
    LEDConfig ledConfig[LED_COUNT];
} SysConfig;

// uint32_t sysGetRGB(uint16_t color, uint8_t extend);
void sysLoadConfig();
SysConfig *sysGetConfig();

#endif
