#include "encoder.h"
#include "stm32f10x_gpio.h"

#define NULL ((void *)0)

static volatile int16_t *value2 = NULL;
static volatile int16_t *value3 = NULL;

static volatile uint32_t *SystemTime = NULL;

void Encoder_Init_Tim2(void);
void Encoder_Init_Tim3(void);
void TIM6_Init(void);

void Encoder_PA_SET(volatile int16_t *v2, volatile int16_t *v3, volatile uint32_t *Stime) {
	SystemTime = Stime;

	value2 = v2;
	value3 = v3;
	Encoder_Init_Tim2();
	Encoder_Init_Tim3();
	TIM6_Init();
}

void Encoder_Init_Tim2(void) {
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//使能定时器2的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能PA端口时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;	//端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);					      //根据设定参数初始化GPIOA

	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler = 0x0; // 预分频器
	TIM_TimeBaseStructure.TIM_Period = 65535; //设定计数器自动重装值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//选择时钟分频：不分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;////TIM向上计数
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	TIM_EncoderInterfaceConfig(TIM2, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising,
	                           TIM_ICPolarity_Rising);//使用编码器模式3
	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_ICFilter = 10;	//滤波10
	TIM_ICInit(TIM2, &TIM_ICInitStructure);
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);//清除TIM的更新标志位
	TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);
	//Reset counter
	TIM_SetCounter(TIM2, 0);
	TIM_Cmd(TIM2, ENABLE);
}

void Encoder_Init_Tim3(void) {
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//使能定时器4的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能PB端口时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;	//端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);					      //根据设定参数初始化GPIOB

	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler = 0x0; // 预分频器
	TIM_TimeBaseStructure.TIM_Period = 65535; //设定计数器自动重装值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//选择时钟分频：不分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;////TIM向上计数
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising,
	                           TIM_ICPolarity_Rising);//使用编码器模式3
	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_ICFilter = 10;
	TIM_ICInit(TIM3, &TIM_ICInitStructure);
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);//清除TIM的更新标志位
	TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE);
	//Reset counter
	TIM_SetCounter(TIM3, 0);
	TIM_Cmd(TIM3, ENABLE);
}


/**************************************************************************
函数功能：单位时间读取编码器计数
返回值：速度值
**************************************************************************/
//int Read_Encoder(u8 TIMX) {
//	int Encoder_TIM;
//	switch (TIMX) {
//		case 2:
//			Encoder_TIM = (short)TIM2 -> CNT;
//			TIM2 -> CNT = 0;
//			break;
//		case 3:
//			Encoder_TIM = (short)TIM3 -> CNT;
//			TIM3 -> CNT = 0;
//			break;
//		default:
//			Encoder_TIM = 0;
//	}
////	 if(Encoder_TIM<0)  Encoder_TIM=-Encoder_TIM;
//	return Encoder_TIM;
//}

/**************************************************************************
函数功能：TIM3中断服务函数
入口参数：无
返回  值：无
**************************************************************************/
//void TIM3_IRQHandler(void) {
//	if (TIM3->SR & 0X0001) { //溢出中断
//	}
//	TIM3->SR &= ~(1 << 0); //清除中断标志位
//}

/**************************************************************************
函数功能：TIM2中断服务函数
入口参数：无
返回  值：无
**************************************************************************/
//void TIM2_IRQHandler(void) {
//	if (TIM2->SR & 0X0001) { //溢出中断
//	}
//	TIM2->SR &= ~(1 << 0); //清除中断标志位
//}
/**************************************************************************
  下面是定时器：读取encoder
**************************************************************************/
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
