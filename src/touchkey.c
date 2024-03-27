#include "touchkey.h"
#include "ch552.h"

#ifdef TOUCH_COUNT
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
uint8_t TK_Code[TOUCH_COUNT] = {
    /* Arrange the input channel */
    0x04, 0x03, /* CH3 CH2 */
    0x05, 0x06  /* CH4 CH5 */
};

uint16_t Key_FreeBuf[TOUCH_COUNT];
// uint16_t  Key_DataBuf[TOUCH_COUNT];
uint8_t Touch_State = 0xFF; /* BIT6 & BIT7 reserved, other bit means touch state */

/*******************************************************************************
 * Function Name  : TK_SelectChannel
 * Description    : Select TK input channel.
 * Input          : ch : input channel.
 * Return         : SUCCESS/FAIL.
 *******************************************************************************/
uint8_t TK_SelectChannel(uint8_t ch) {
  if (ch <= TOUCH_COUNT) {
    TKEY_CTRL = (TKEY_CTRL & 0XF8) | TK_Code[ch];
    return 0;
  }

  return 0xFF;
}

/*******************************************************************************
 * Function Name  : TK_GetKeyFreeBuff
 * Description    : GetKeyFreeBuff, DO NOT get Key_FreeBuf in Mass Production.
 *******************************************************************************/
void TK_GetKeyFreeBuff() {
  uint8_t i, j;
  uint16_t sum;
  uint16_t OverTime;
  /* Get Key_FreeBuf. Save the data in flash or macro define. */
  /* DO NOT get Key_FreeBuf in Mass Production. */
  for (i = 0; i < TOUCH_COUNT; i++) {
    sum = 0;
    j = SAMPLE_TIMES;
    TK_SelectChannel(i);
    //		TKEY_CTRL |= TK_Code[i];
    while (j--) {
      OverTime = 0;
      while ((TKEY_CTRL & bTKC_IF) == 0) {
        if (++OverTime == 0) {
          // return 0xFF;
        }
      }
      sum += TKEY_DAT; /*  */
    }
    Key_FreeBuf[i] = sum / SAMPLE_TIMES;
  }
}

/*******************************************************************************
* Function Name  : TK_Init
* Description    : Init input channel. Float input, if it used for touchkey
* Input          : channel   : IO bits, configure the Hi-z mode for touch
detection. queryFreq : Scan frequence. 0: 1ms , 1: 2ms. ie		 :
Enable interrupt, 0:disable ; i: enable.
* Return         : Return FAIL, if channel's error.
*******************************************************************************/
uint8_t TK_Init(uint8_t channel, uint8_t queryFreq, uint8_t ie) {
  if ((channel & (BIT2 + BIT3)) != 0) /* not include BIT2 & BIT3 */
  {
    return 0xFF;
  }

  P1_DIR_PU &= ~channel;
  P1_MOD_OC &= ~channel;

  if (queryFreq != 0) {
    TKEY_CTRL |= bTKC_2MS;
  }

  TK_GetKeyFreeBuff();

  if (ie != 0) /* Enable interrupt ?  */
  {
    IE_TKEY = 1;
  }

  return 0;
}

/*******************************************************************************
 * Function Name  : TK_Measure
 * Description    : Measure touch input.
 * Input          : None
 * Return         : None
 *******************************************************************************/
// uint8_t TK_Measure(void) {
//   uint8_t i;
//   uint16_t OverTime, KeyData;

//   for (i = 0; i < TOUCH_COUNT; i++) {
//     OverTime = 0;
//     TK_SelectChannel(i);

//     while ((TKEY_CTRL & bTKC_IF) == 0) {
//       if (++OverTime == 0) {
//         return 0xFF;
//       }
//     }

//     KeyData = TKEY_DAT;

//     if (KeyData < (Key_FreeBuf[i] - TH_VALUE)) {
//       Touch_State |= 1 << (TK_Code[i] - 1);
//     }

//     //		printf( "ch[%d]=%d\t", (UINT16)(TK_Code[i] - 1), (UINT16)KeyData
//     //);
//   }

//   //	printf( "%02x\n", (UINT16)Touch_IN );

//   return 0;
// }

/*******************************************************************************
 * Function Name  : TK_int_ISR
 * Description    : Touch key interrupt routing for touch key scan.
 * Input          : None.
 * Return         : None.
 *******************************************************************************/
void __TK_int_ISR() __interrupt INT_NO_TKEY __using(1) {
  static uint8_t ch = 0;

  if (TKEY_DAT < (Key_FreeBuf[ch] - TH_VALUE)) {
    Touch_State &= ~(1 << (TK_Code[ch] - 1)); // Active
  } else {
    Touch_State |= 1 << (TK_Code[ch] - 1); // Not active
  }

  //	printf( "ch[%d]=%d\t", (UINT16)(TK_Code[ch] - 1), (UINT16)KeyData );

  if (++ch >= TOUCH_COUNT) {
    //		printf("\n");
    ch = 0;
  }
  TK_SelectChannel(ch);
}

#endif