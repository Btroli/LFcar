#include "tim.h"

#define NULL ((void *)0)

static volatile int16_t *value2 = NULL;
static volatile int16_t *value3 = NULL;

static volatile uint32_t *SystemTime = NULL;

void TIM6_Init(void);
void TIM7_Init(void);

void TIM67_SET(volatile int16_t *v2, volatile int16_t *v3, volatile uint32_t *Stime) {
	SystemTime = Stime;

	value2 = v2;
	value3 = v3;

	TIM6_Init();
	TIM7_Init();
}

void TIM6_Init(void) {
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	//TIM6
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

	TIM_TimeBaseStructure.TIM_Period = 99;          // 自动重装载值
	TIM_TimeBaseStructure.TIM_Prescaler = 7199;       // 预分频系数
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

	//中断
	TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM6, ENABLE);

	// NVIC
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
//中断函数
void TIM6_IRQHandler(void) {
	if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET) {

		(*SystemTime)++;

		*value2 = (int16_t)TIM2->CNT;
		TIM2->CNT = 0;
		*value3 = (int16_t)TIM3->CNT;
		TIM3->CNT = 0;

		TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
	}
}

void TIM7_Init(void) {
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	//TIM7
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);

	TIM_TimeBaseStructure.TIM_Period = 19;	//自动重装载值，0到19=20，计数20次（20us）中断一次
	TIM_TimeBaseStructure.TIM_Prescaler = 71;	//72MHz/(71+1) = 1MHz = 1µs分辨率
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);

	//中断
	TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM7, ENABLE);

	// NVIC
	NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void TIM7_IRQHandler(void) {
	if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET) {

		//每20us中断一次
		//capture_done = 3 -> 2 , 延时20us
		//#define CSB capture_done = n
		//trig高脉冲时间=(n-2)*tim7周期

		if (capture_done > 1) {
			if (capture_done-- == 2) {
				capture_done = 0;
				TRIG_LOW;
			}
			TRIG_HIGH;
		}

		TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
	}
}

