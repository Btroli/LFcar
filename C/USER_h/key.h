#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"
#include "delay.h"

#define	KEY_ON	 1
#define	KEY_OFF	 0

#define KEY_PIN1	GPIO_Pin_4
#define KEY_PIN2	GPIO_Pin_5
#define KEY_PIN3	GPIO_Pin_6
#define KEY_PIN4	GPIO_Pin_7

#define KEY_PORT	GPIOG
#define KEY_CLK		RCC_APB2Periph_GPIOG	//端口时钟

void 	KEY_SET(void);		//按键IO口初始化函数
uint8_t	KEY_Scan(uint8_t pin);		//按键扫描
uint8_t	KEY_ScanNF(uint8_t pin);	//按键扫描,不用等待放开

#endif	//__KEY_H

