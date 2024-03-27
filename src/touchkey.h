#ifndef	__TOUCH_KEY_H
#define	__TOUCH_KEY_H

#include <stdint.h>
#include "bsp.h"

/*******************************************************************************
Input channel as below:

 bTKC_CHAN2 |bTKC_CHAN1	|bTKC_CHAN0	|	CHANNEL				| PIN		
 -----------+-----------+-----------+-----------------------+------------
 	0		|	0		|	0		|	no channel			| ****		
 	0		|	0		|	1		|	TIN0				| P1.0		
 	0		|	1		|	0		|	TIN1				| P1.1		
 	0		|	1		|	1		|	TIN2				| P1.4		
 	1		|	0		|	0		|	TIN3				| P1.5		
 	1		|	0		|	1		|	TIN4				| P1.6		
 	1		|	1		|	0		|	TIN5				| P1.7		
	1		|	1		|	1		|	Enable touch core	| no channel
	
*******************************************************************************/
#define		BIT0		(0X01)
#define		BIT1		(0X02)
#define		BIT2		(0X04)
#define		BIT3		(0X08)
#define		BIT4		(0X10)
#define		BIT5		(0X20)
#define		BIT6		(0X40)
#define		BIT7		(0X80)

/* Macro define */
#define		CHX				(0X00)							/* Shutoff Touch input */
#define		CH0				(BIT0)
#define		CH1				(BIT1)
#define		CH2				(BIT2)
#define		CH3				(BIT3)
#define		CH4				(BIT4)
#define		CH5				(BIT5)
#define		CH_FREE			(0x07)						/* Enable touch input, no channel is selected. */

#define		TH_VALUE		(0x140)
#define		SAMPLE_TIMES	(0x04)

#ifdef TOUCH_COUNT
/* External variable and function */ 
extern 		uint16_t 		Key_FreeBuf[TOUCH_COUNT];
extern		uint8_t			Touch_State;			

extern uint8_t TK_Init( uint8_t channel , uint8_t queryFreq, uint8_t ie );
extern void TK_GetKeyFreeBuff();
extern uint8_t TK_SelectChannel( uint8_t ch );
extern uint8_t TK_Measure( void )	;
#endif

#endif
