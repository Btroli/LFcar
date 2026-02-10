#ifndef __ALLHEADER_H_
#define __ALLHEADER_H_

#define BlackLine 1
#define WhiteLine 0
#define LINE BlackLine

#define loop_car_delaytime 10

#define LEFT	0b11100000
#define MID 	0b00011000
#define RIGHT	0b00000111

#define u8  uint8_t
#define u16 uint16_t 
#define u32 uint32_t 

#include "global.h"

#include "stm32f10x.h"
#include "key.h"
#include "pwm.h"
#include "tim.h"
#include "mmgj.h"
#include "oled.h"
#include "delay.h"
//#include "button.h"
#include "encoder.h"
#include "loopmode.h"
#include "ultrasound.h"
#include "LightSensor.h"

#endif
