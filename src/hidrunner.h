#ifndef __HID_RUNNER_H_
#define __HID_RUNNER_H_

#include <stdint.h>
#include "ch552.h"
#include "usb.h"

#define HID_CMD_VERSION 0x01

#define hid_cmd uint8_t

// HID 命令表
typedef enum
{
  GET_INFO_LIST = 0xAA,         // 获取设备基本信息列表

  READ_FLASH = 0x00,            // 读内部存储设备
  WRITE_FLASH = 0x01,           // 写内部存储设备
  READ_FLASH_EX = 0x02,         // 读外部存储设备
  WRITE_FLASH_EX = 0x03,        // 写外部存储设备

  RELOAD_SETTINGS = 0x10,       // 从存储器重新加载设置
  SAVE_ALL_DATA = 0x11,         // 将临时设置全部保存至存储器

  KEY_DATA = 0x20,              // 按键功能
  LED_KEY = 0x21,               // 按键灯控制（彩色）
  LED_NORMAL = 0x22,            // 普通灯控制（彩色）
  COLOR_LIST = 0x29,            // 色表操作

  TOUCH_KEY_SAMPLE = 0xB1,      // 触摸按键采样
  TOUCH_KEY_SENSITIVITY = 0x31, // 触摸按键灵敏度

  MACRO_DATA = 0xA0,            // 宏功能

  MODE_IAP = 0xFE,              // IAP
  MODE_BOOTLOADER = 0xFF        // BootLoader
} HID_CMD;

// HID 响应状态码
typedef enum
{
  OK = 0x00,                    // OK
  
  PAGE_DOWNLOADED = 0x10,       // 下位机已接受分页数据包
  PAGE_UPLOADED = 0x11,         // 向上位机发送数据包尚未完成

  CHECKSUM_ERROR = 0x20,        // 校验和错误
  DATA_FORMAT_ERROR = 0x21,     // 数据格式错误

  UNSUPPORTED_ITEM = 0xFF       // 项目不被支持
} STATE_CODE;


void hid_run();

#endif