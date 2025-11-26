#ifndef __MMGJ_H
#define __MMGJ_H

#include "stdint.h"
#include "oled.h"
#include "key.h"

#define value_len 10
extern uint8_t count, area, option, option_NUM, sel_flag, car_screen_flag, value_num;
extern uint8_t value[10][value_len] , name[10][10];

void cal_valueP(uint8_t *p, float *v);
float cal_value(uint8_t *p);
void FuXuan(uint8_t n);

void loop_screen0(void);
void loop_screen1(void);

void pidInit(void);

#endif

