#ifndef __BUTTON_H
#define __BUTTON_H

#include "AllHeader.h"

#define BUTTON1    GPIO_Pin_0
#define BUTTON2    GPIO_Pin_1
#define BUTTON3    GPIO_Pin_2
#define BUTTON4    GPIO_Pin_3

void Button_SET(void);

void EXTI0_IRQHandler(void);
void EXTI1_IRQHandler(void);
void EXTI2_IRQHandler(void);
void EXTI3_IRQHandler(void);

#endif
