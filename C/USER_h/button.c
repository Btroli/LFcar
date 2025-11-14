#include "button.h"
#include "delay.h"

//extern uint8_t n, m, f1, f2;

void Button_SET(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	// 使能PG + AFIO时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG | RCC_APB2Periph_AFIO, ENABLE);

	// 配置PG8 - 11
	GPIO_InitStructure.GPIO_Pin = BUTTON1 | BUTTON2 | BUTTON3 | BUTTON4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // 上拉输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOG, & GPIO_InitStructure);

	// 连接EXTI
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOG, GPIO_PinSource0);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOG, GPIO_PinSource1);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOG, GPIO_PinSource2);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOG, GPIO_PinSource3);

	// 配置EXTI
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; // 下降沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(& EXTI_InitStructure);

	EXTI_InitStructure.EXTI_Line = EXTI_Line1;
	EXTI_Init(& EXTI_InitStructure);

	EXTI_InitStructure.EXTI_Line = EXTI_Line2;
	EXTI_Init(& EXTI_InitStructure);

	EXTI_InitStructure.EXTI_Line = EXTI_Line3;
	EXTI_Init(& EXTI_InitStructure);

	// 配置NVIC
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(& NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_Init(& NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
	NVIC_Init(& NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
	NVIC_Init(& NVIC_InitStructure);
}

// 中断函数

//void EXTI0_IRQHandler(void) {
//	if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
//		EXTI_ClearITPendingBit(EXTI_Line0);	// 清除中断标志
//		n++;
//		delay_ms(20);	//防抖延时
//	}
//}
//void EXTI1_IRQHandler(void) {
//	if (EXTI_GetITStatus(EXTI_Line1) != RESET) {
//		EXTI_ClearITPendingBit(EXTI_Line1);
//		m++;
//		delay_ms(20);
//	}
//}
//void EXTI2_IRQHandler(void) {
//	if (EXTI_GetITStatus(EXTI_Line2) != RESET) {
//		EXTI_ClearITPendingBit(EXTI_Line2);
//		if (f2 == 0)
//			f1++;
//		delay_ms(20);
//	}
//}
//void EXTI3_IRQHandler(void) {
//	if (EXTI_GetITStatus(EXTI_Line3) != RESET) {
//		EXTI_ClearITPendingBit(EXTI_Line3);
//		if (f1 == 0)
//			f2++;
//		delay_ms(20);
//	}
//}
