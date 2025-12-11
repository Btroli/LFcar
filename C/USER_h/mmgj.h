#ifndef __MMGJ_H
#define __MMGJ_H

#include "stdint.h"
#include "oled.h"
#include "key.h"
#include "pwm.h"

#define value_len 10
extern uint8_t count, area, option, option_NUM, sel_flag, car_screen_flag, value_num;
extern uint8_t value[10][value_len], name[10][10];

//pid0
extern int16_t SPDA, SPDB;
extern uint8_t Kp_A, Ki_A, Kd_A, Kp_B, Ki_B, Kd_B;
extern int16_t PA, PB, pre_PA, pre_PB, sum_PA, sum_PB;
extern volatile int16_t EvalueA, EvalueB;
extern int16_t GA,GB;


void cal_valueP(uint8_t *p, float *v);
float cal_value(uint8_t *p);
void FuXuan(uint8_t n);

void loop_screen0(void);
void loop_screen1(void);

void pidInit(void);
void pidInit_1(void);

//下面的值，除以100就是 速度 cm/s
//速度 距离 计算，查表更方便
static uint16_t EcdSpeed[81] = {0, 142, 285, 427, 569, 711, 854, 996, 1138, 1281, 1423, 1565, 1707, 1850, 1992, 2134, 2276, 2419, 2561, 2703, 2846, 2988, 3130, 3272, 3415, 3557, 3699, 3842, 3984, 4126, 4268, 4411, 4553, 4695, 4837, 4980, 5122, 5264, 5407, 5549, 5691, 5833, 5976, 6118, 6260, 6403, 6545, 6687, 6829, 6972, 7114, 7256, 7398, 7541, 7683, 7825, 7968, 8110, 8252, 8394, 8537, 8679, 8821, 8964, 9106, 9248, 9390, 9533, 9675, 9817, 9959, 10102, 10244, 10386, 10529, 10671, 10813, 10955, 11098, 11240, 11382};

#endif
