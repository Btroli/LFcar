#include "pwm.h"

void PWM_PB_SET(void);
void pwm_int(void);

void PWM_SET(void) {
	pwm_int();
	PWM_PB_SET();
}

void PWM_PB_SET(void) {	//初始化引脚，设置引脚的功能。
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = STBY | BIN2 | BIN1 | AIN1 | AIN2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = PWMA | PWMB;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void pwm_int(void) {

	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;       //定义结构TIM_TimeBaseStructure
	TIM_OCInitTypeDef  TIM_OCInitStructure;               //定义结构TIM_OCInitStructure


	//使能时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);


	//定时器基础配置
	TIM_TimeBaseStructure.TIM_Period = 7199;       // ARR值
	TIM_TimeBaseStructure.TIM_Prescaler = 0;       // 无分频
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);


	//PWM输出配置
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_Pulse = 0;  // 初始占空比0%

	//**//  占空比 = TIM_Pulse / （TIM_Period + 1）
	//**//  Set_PWMA 中 (int PWM）即为设置 TIM_Pulse


	// 配置通道 1 (PB6) 和通道 2 (PB7)
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);

	// 启用预装载
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);

	// 使能预装载和定时器
	TIM_ARRPreloadConfig(TIM4, ENABLE);
	TIM_CtrlPWMOutputs(TIM4, ENABLE);
	TIM_Cmd(TIM4, ENABLE);
}

void Set_PWMA(int PWM) {
	if (PWM < 0) {                    //正转（车前进）
		GPIO_SetBits(GPIOB, AIN2);	  // 高电平
		GPIO_ResetBits(GPIOB, AIN1);  // 低电平

		TIM_SetCompare1(TIM4, -PWM);  //设置TIM4通道1的占空比
	} else {                          //反转
		GPIO_SetBits(GPIOB, AIN1);	  // 高电平
		GPIO_ResetBits(GPIOB, AIN2);  // 低电平

		TIM_SetCompare1(TIM4, PWM);   //设置TIM4通道1的占空比
	}
}

void Set_PWMB(int PWM) {
	if (PWM < 0) {
		GPIO_SetBits(GPIOB, BIN2);
		GPIO_ResetBits(GPIOB, BIN1);

		TIM_SetCompare2(TIM4, -PWM);
	} else {
		GPIO_SetBits(GPIOB, BIN1);
		GPIO_ResetBits(GPIOB, BIN2);

		TIM_SetCompare2(TIM4, PWM);
	}
}
