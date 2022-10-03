#include "hidrunner.h"

void hid_run(){}

// void writeHIDData(uint8_t dataLength, uint8_t* data){}
void writeHIDFlash(uint8_t dataLength, uint8_t addr){}
void writeHIDFlashEx(uint8_t dataLength, uint16_t addr){}

void returnHIDData(STATE_CODE state, uint8_t dataLength, uint8_t *data){}
void returnHIDFlash(STATE_CODE state, uint8_t dataLength, uint8_t addr){}
void returnHIDFlashEx(STATE_CODE state, uint8_t dataLength, uint16_t addr){}