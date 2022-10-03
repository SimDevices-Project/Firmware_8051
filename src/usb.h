#ifndef __USB_H
#define __USB_H

#include <stdint.h>
#include "bsp.h"

void usbDevInit();                              // USB设备初始化

void usbSetKeycode(uint8_t i, uint8_t key);     // 设置键值
void usbReleaseAll();                           // 清除上传缓冲区
void usbPushKeydata();                          // 上传按键数据

#define KEY_BUFFER 16                           // 键盘端点最大缓冲大小
#define MOUSE_BUFFER 16                         // 鼠标端点最大缓冲大小
#define HID_BUFFER 64                           // HID数据缓冲大小
//#define USE_EXT_STR                           // 是否启用额外的字符串描述符

uint8_t getHIDData(uint8_t index);              // 从HID接收缓冲中读取一个字节
void setHIDData(uint8_t index, uint8_t data);   // 设置HID发送缓冲区对应偏移的数据
__bit hasHIDData();                             // 是否有数据下发
void requestHIDData();                          // 请求数据下发
void pushHIDData();                             // 将HID发送缓冲区内的数据上传
uint8_t* fetchHIDData();                        // 获取HID接收缓冲区的数据

#endif
