#ifndef _ULTRASOUND_H
#define _ULTRASOUND_H

#include "stm32f10x_gpio.h"

#define TRIG GPIO_Pin_2
#define ECHO GPIO_Pin_3

#define TRIG_HIGH	{GPIOA->BSRR = TRIG;}
#define TRIG_LOW	{GPIOA->BRR = TRIG;}
#define TRIG_TOGGLE	{GPIOA->ODR ^= TRIG;}

extern volatile int8_t  capture_done;
extern volatile uint16_t  echotime;

#define CSB capture_done=3
/*
 * capture_done = 3->0 （中断，特殊）trig态，忽略
 * capture_done = 1 完成态，测量结束。
 * capture_done = 0 等待echo态
 * capture_done = -1 计时echo态
 */

void UltraSound_SET(void);
float ultrasound_distance(void);

#endif

/*
   最原始状态

while (1) {
	TRIG_HIGH;
	delay_us(15);
	TRIG_LOW;

	//重置状态
	capture_done = 0;

	//等待测量完成
	while (capture_done == 1);
}
*/

