#ifndef __LIGHTSENSOR_H
#define	__LIGHTSENSOR_H

#include "stm32f10x.h"

#define G_OUT0    GPIO_Pin_8
#define G_OUT1    GPIO_Pin_9
#define G_OUT2    GPIO_Pin_10
#define G_OUT3    GPIO_Pin_11
#define G_OUT4    GPIO_Pin_12
#define G_OUT5    GPIO_Pin_13
#define G_OUT6    GPIO_Pin_14
#define G_OUT7    GPIO_Pin_15

void Guangmin_PG_SET(void);	//初始化函数

uint8_t ReadOne(uint8_t pinNum);
uint8_t ReadAll(void);

#endif
