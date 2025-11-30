#include "LightSensor.h"

void Guangmin_PG_SET(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);

	GPIO_InitStructure.GPIO_Pin = G_OUT0 | G_OUT1 | G_OUT2 | G_OUT3 | G_OUT4 | G_OUT5 | G_OUT6 | G_OUT7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;      // 上拉输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOG, &GPIO_InitStructure);
}

uint8_t ReadOne(uint8_t pinNum) {   //只看一个传感器，只read一个（one）。pinNum是引脚号码
	return GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_0 << (pinNum + 8));
}

uint8_t ReadAll(void) {
	return (uint8_t) ((GPIO_ReadInputData(GPIOG)>>8) & 0xFF);   //返回8位的二进制，测到的是1（黑），白的是0
}
