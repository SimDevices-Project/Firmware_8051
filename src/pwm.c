/********************************** (C) COPYRIGHT *******************************
* File Name          : PWM.C
* Author             : WCH
* Version            : V1.1
* Date               : 2018/02/27
* Description        : CH554 PWM中断使能和中断处理               				   
*******************************************************************************/

#include "ch552.h"
#include "pwm.h"
#include "stdio.h"

#if PWM_INTERRUPT
/*******************************************************************************
* Function Name  : PWMInterruptEnable()
* Description    : PWM中断使能
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void  PWMInterruptEnable()
{
    PWM_CTRL |= bPWM_IF_END | bPWM_IE_END;                                      //清除PWM中断，使能PWM中断
    IE_PWMX = 1;	
}

/*******************************************************************************
* Function Name  : PWMInterrupt(void)
* Description    : PWM中断服务程序   
*******************************************************************************/
void	PWMInterrupt( void ) interrupt INT_NO_PWMX using 1                      //PWM1&2中断服务程序,使用寄存器组1
{
    PWM_CTRL |= bPWM_IF_END;                                                  //清除PWM中断
    SetPWM1Dat(0x10);
    SetPWM2Dat(0x40);	
}
#endif