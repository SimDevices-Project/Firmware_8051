#include "motor.h"
#include "sys.h"

#ifdef MOTOR

uint16_t motorDelay = 0;
MOTOR               = 0;

void motorUpdate()
{
  if (motorDelay)
  {
    motorDelay--;
    if (motorDelay == 0)
    {
      MOTOR = 0;
    }
  }
}

void activeMotor(uint16_t time)
{
  motorDelay = time;
  MOTOR      = 1;
}

#endif