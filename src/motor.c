#include "motor.h"
#include "sys.h"

#ifdef HAS_MOTOR

#include "pwm.h"

uint16_t motorDelay = 0;

void motorInit()
{
  MOTOR_IN1 = 0;
  MOTOR_IN2 = 0;

  SetPWMClk(0xFF);                                                           //PWM时钟配置，Fsys/256/255分频
  ForceClearPWMFIFO( );                                                      //强制清除PWM FIFO和COUNT
  CancleClearPWMFIFO( );                                                     //取消清除PWM FIFO和COUNT

  PWM1PINAlter( );                                                           //PWM映射脚P30

  DsiablePWM1Out();
  DisablePWM2Out();

  SetPWM1Dat(128);                                                           //PWM占空比
  SetPWM1Dat(128);                                                            //PWM占空比

  PWM1OutPolarHighAct( );                                                    //PWM1输出默认低，高有效
  PWM2OutPolarLowAct( );                                                     //PWM2输出默认高，低有效

}

void motorUpdate()
{
  if (motorDelay)
  {
    motorDelay--;
    if (motorDelay == 0)
    {
      DsiablePWM1Out();
      DisablePWM2Out();
    }
  }
}

void activeMotor(uint16_t time)
{
  motorDelay = time;
  PWM1OutEnable();
  PWM2OutEnable();
}

#endif