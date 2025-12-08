#ifndef _ENCODER_H
#define _ENCODER_H

#include "stm32f10x.h"

//初始化函数，需在main开头来一次。
void Encoder_PA_SET(volatile int16_t *v2, volatile int16_t *v3, volatile uint32_t *Stime);

//int Read_Encoder(u8 TIMX);	//例：Read_Encoder(2);返回脉冲数（自清零以来）

/* 引脚对应接线
	PA0 -> E1A
	PA1 -> E1B
	PA6 -> E2A
	PA7 -> E2B
*/

#endif
