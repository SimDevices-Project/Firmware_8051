#ifndef __BSP_H
#define __BSP_H

#ifndef __MULTI_COMPILE__
/* 编译目标设置 开始 */
//#define SIMPAD_V2_AE
//#define SIMPAD_NANO_AE
#define SIM_KEY
//#define SIMPAD_V2
//#define SIMPAD_NANO
//#define SIMPAD_TOUCH
/* 编译目标设置 结束 */
#endif

#define ROM_VERSION_YEAR_H  0x20
#define ROM_VERSION_YEAR_L  0x21
#define ROM_VERSION_MONTH   0x01
#define ROM_VERSION_DATE    0x09

/* 特性支持列表 */
#ifndef __FEATURE_HARDWARE_LIST_
#define __FEATURE_HARDWARE_LIST_
    #define UNUSED              0x80  // 
    #define FEATURE_KEY         0x40  // 物理按键
    #define FEATURE_TOUCH_KEY   0x20  // 触摸按键
    #define FEATURE_LED_KEY     0x10  // 按键全彩灯
    #define FEATURE_LED_NORMAL  0x08  // 普通全彩灯
    #define FEATURE_TOGGLE      0x04  // 拨动开关
    #define FEATURE_ROLLER      0x02  // 旋转编码器
    #define FEATURE_MOTOR       0x01  // 振动器
#endif

#ifndef __FEATURE_NORMAL_LIST_
#define __FEATURE_NORMAL_LIST_
    #define FEATURE_BACKLIGHT   0x80  // 单色背光
    #define FEATURE_BLUETOOTH   0x40  // 蓝牙
    #define FEATURE_BATTERY     0x20  // 电池
    #define FEATURE_STATUS_LED  0x10  // 状态灯

    #define FEATURE_MACRO       0x02  // 宏
    #define FEATURE_EXTEND      0x01  // 扩展特性设置支持
#endif

/* 基本报表支持列表 */
#ifndef __FEATURE_BASIC_LIST_
#define __FEATURE_BASIC_LIST_
    #define FEATURE_BASIC_KEYBOARD  0x80 // 标准键盘
    #define FEATURE_BASIC_MOUSE     0x40 // 标准鼠标
    #define FEATURE_BASIC_MEDIA     0x20 // 多媒体键盘
    #define FEATURE_BASIC_GAMEPAD   0x10 // 游戏手柄
    #define FEATURE_BASIC_ABSOLUTE  0x08 // 绝对定位
    #define FEATURE_BASIC_DAIL      0x04 // Dail
#endif

/* 扩展功能支持列表 */
#ifndef __FEATURE_EXTEND_LIST_
#define __FEATURE_EXTEND_LIST_
    #define FEATURE_EXTEND_LAYER       0x80 // 按键分层

    #define FEATURE_EXTEND_DEVICE_NAME 0x20 // 自定设备名
    #define FEATURE_EXTEND_VPID        0x10 // 自定 VID/PID

    #define FEATURE_EXTEND_REALTIME    0x01 // 实时设置预览
#endif


/* 切换至性能模式 */
//#define PERFORMANCE_MODE

/* 使能字符串输入模拟指令 */
//#define INSTR_ENB_STRP

/* 使能灯光渐变列表 */
//#define RGB_USE_GRAD_LIST
/* 使能简化灯光渐变列表，CH552受存储空间限制，必须打开 */
//#define RGB_GRAD_LIST_LITE

__sfr   __at (0x90) P1;
__sbit  __at (0x90) P10;
__sbit  __at (0x91) P11;
__sbit  __at (0x92) P12;
__sbit  __at (0x93) P13;
__sbit  __at (0x94) P14;
__sbit  __at (0x95) P15;
__sbit  __at (0x96) P16;
__sbit  __at (0x97) P17;

__sfr   __at (0xB0) P3;
__sbit  __at (0xB0) P30;
__sbit  __at (0xB1) P31;
__sbit  __at (0xB2) P32;
__sbit  __at (0xB3) P33;
__sbit  __at (0xB4) P34;
__sbit  __at (0xB5) P35;
__sbit  __at (0xB6) P36;
__sbit  __at (0xB7) P37;

/* 芯片内部存储空间大小 */
#define FLASH_SIZE 0x80

/* 板级接口定义，需要注意的是，按键需定义为BTx */
#if defined(SIMPAD_V2_AE)
    #define BT1 P33
    #define BT2 P32
    #define BT3 P34
    #define BT4 P31
    #define BT5 P30
    #define KEY_COUNT 5
    #define LED P12
    #define LED_COUNT 4

    #define FEATURE_BASIC FEATURE_BASIC_KEYBOARD | FEATURE_BASIC_MOUSE | FEATURE_BASIC_MEDIA | FEATURE_BASIC_GAMEPAD | FEATURE_BASIC_ABSOLUTE // 特性支持，高8位
    #define FEATURE_NORMAL FEATURE_MACRO | FEATURE_EXTEND // 特性支持，低8位
    
#elif defined(SIMPAD_NANO_AE)
    #define BT1 P15
    #define BT2 P17
    #define BT3 P16
    #define KEY_COUNT 3
    #define LED P14
    #define LED_COUNT 2
    #define HAS_ROM
    #define ROM_SIZE 0x800
    #define ROM_SDA P11
    #define ROM_SCL P10
    #define ROM_WP P33
#elif defined(SIM_KEY)
    #define BT1 P17
    #define KEY_COUNT 1
    #define LED P14
    #define LED_COUNT 1
    #define HAS_ROM
    #define ROM_SIZE 0x800
    #define ROM_SDA P11
    #define ROM_SCL P10
    #define ROM_WP P33
#elif defined(SIMPAD_TOUCH)
    #define BT1 P15
    #define BT2 P14
    #define BT3 P16
    #define BT4 P17
    #define KEY_COUNT 4
    #define	TOUCH_COUNT	4
    // #define LED P33
    #define LED_COUNT 0
    #define HAS_ROM
    #define ROM_SIZE 0x800
    #define ROM_SDA P10
    #define ROM_SCL P31
    #define ROM_WP P11
    #define MOTOR P30

    #define FEATURE_HARDWARE (FEATURE_MOTOR | FEATURE_LED_KEY | FEATURE_TOUCH_KEY)
#elif defined(SIMPAD_V2)
    #define BT1 P32
    #define BT2 P33
    #define BT3 P11
    #define BT4 P10
    #define BT5 P34
    #define KEY_COUNT 5
    #define G1R P15
    #define G1G P16
    #define G1B P14
    #define G2R P13
    #define G2G P12
    #define G2B P17
    #define LED_COUNT 2
#elif defined(SIMPAD_NANO)
    #define BT1 P17
    #define BT2 P15
    #define BT3 P16
    #define KEY_COUNT 3
    #define LED P14
    #define LED_COUNT 2
#else
    #error "No board defined!"
#endif

#if LED_COUNT > 0
    #define HAS_LED
#endif

#define VENDOR_ID_H 0x80
#define VENDOR_ID_L 0x88

/*
 * 00 01 SimPad v2
 * 00 02 SimPad v2 - Extra
 * 00 03 SimPad v2 - Lite
 * 00 04 SimPad Nano
 * 00 05 SimPad 3
 * 00 06 SimPad v2 - Year Edition
 * 00 07 SimPad Nano - Year Edition
 * 00 08 SimKey
 * 00 A0 SimPad Touch
 * 00 FF SimPad Boot
 */
#if defined(SIMPAD_V2_AE)
  #define PRODUCT_ID_H 0x00
  #define PRODUCT_ID_L 0x06
#elif defined(SIMPAD_NANO_AE)
  #define PRODUCT_ID_H 0x00
  #define PRODUCT_ID_L 0x07
#elif defined(SIM_KEY)
  #define PRODUCT_ID_H 0x00
  #define PRODUCT_ID_L 0x08
#elif defined(SIMPAD_TOUCH)
  #define PRODUCT_ID_H 0x00
  #define PRODUCT_ID_L 0xA0
#elif defined(SIMPAD_V2)
  #define PRODUCT_ID_H 0x00
  #define PRODUCT_ID_L 0x01
#elif defined(SIMPAD_NANO)
  #define PRODUCT_ID_H 0x00
  #define PRODUCT_ID_L 0x04
#else
  #define PRODUCT_ID_H 0x00
  #define PRODUCT_ID_L 0x00
#endif

#define DEVICE_INFO_ARRAY [ 0x00,               0x00,               PRODUCT_ID_H,       PRODUCT_ID_L,                                         \
                            ROM_VERSION_YEAR_H, ROM_VERSION_YEAR_L, ROM_VERSION_MONTH,  ROM_VERSION_DATE,    \
                            0x00,               0x01,               0x00,               0x00                 \
                            ]

#endif