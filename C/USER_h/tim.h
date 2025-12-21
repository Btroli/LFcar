#ifndef _TIM_H
#define _TIM_H

#include "stm32f10x_gpio.h"
#include "ultrasound.h"

void TIM67_SET(volatile int16_t *v2, volatile int16_t *v3, volatile uint32_t *Stime);

#endif
