#ifndef __PWM_H
#define	__PWM_H

#include "AllHeader.h"

/********************************************/

#define PWMA    GPIO_Pin_6
#define PWMB    GPIO_Pin_7
#define BIN2    GPIO_Pin_11
#define BIN1    GPIO_Pin_12
#define AIN1    GPIO_Pin_13
#define AIN2    GPIO_Pin_14
#define STBY    GPIO_Pin_15

#define digitalHigh(p, i) {p->BSRR = i;}
#define digitalLow(p, i) {p->BRR = i;}
#define digitalToggle(p, i) {p->ODR ^= i;}

#define HIGH(i) digitalHigh(GPIOB, i)
#define LOW(i)  digitalLow(GPIOB, i)
#define TOG(i)  digitalToggle(GPIOB, i)

/********************************************/

void PWM_SET(void);

void Set_PWMA(int PWM);
void Set_PWMB(int PWM);


#endif
