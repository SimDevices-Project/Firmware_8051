#ifndef __MOTOR_H
#define __MOTOR_H

#include "bsp.h"
#include "ch552.h"

void motorInit();
void motorUpdate();
void activeMotor(uint16_t time);

#endif