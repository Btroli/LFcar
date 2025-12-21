#include "ultrasound.h"

volatile int8_t  capture_done = 1;
volatile uint16_t  time = 0xFFFF;

void SET_TRIG_and_ECHO(void);
void TIM5_init(void);

void UltraSound_SET(void) {
	SET_TRIG_and_ECHO();
	TIM5_init();
}

void SET_TRIG_and_ECHO(void) {	//初始化引脚TRIG和ECHO
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = TRIG;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//普通推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = ECHO;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//浮空输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void TIM5_init(void) {
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);	//使能TIM5时钟

	TIM_TimeBaseStructure.TIM_Prescaler = 71;	//72MHz/(71+1)=1MHz，1MHz计数频率(1μs分辨率)(每1μs记一次)
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF;	//最大65535μs
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);

	//输入捕获配置
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;	//CH4 -> PA3
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter = 0x0;
	TIM_ICInit(TIM5, &TIM_ICInitStructure);

	//使能捕获中断
	TIM_ITConfig(TIM5, TIM_IT_CC4, ENABLE);
	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);	//可选:溢出中断

	//NVIC配置
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//启动定时器
	TIM_Cmd(TIM5, ENABLE);
}

void TIM5_IRQHandler(void) {
	if (TIM_GetITStatus(TIM5, TIM_IT_CC4) != RESET) {	//检查捕获中断标志
		TIM_ClearITPendingBit(TIM5, TIM_IT_CC4);	//清除标志

		if (TIM5->CCER & TIM_CCER_CC4P) {	//如果是下降沿捕获
			time = TIM5->CCR4;	//捕获
			TIM5->CCER &= ~TIM_CCER_CC4P;	//切换回上升沿
			capture_done = 1;	//标记测量完成
		} else {	//如果是上升沿捕获
			TIM5->CNT = 0;
			capture_done = -1;
			TIM5->CCER |= TIM_CCER_CC4P;	//切换为下降沿
		}
	}
	if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET) {
		TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
		if (capture_done == -1) {
			time = 0xFFFF;
			capture_done = 1;
		}
	}
}

float ultrasound_distance(void) {
	return time * 0.017;
}

