#ifndef __LOOPMODE_H_
#define __LOOPMODE_H_

#include "AllHeader.h"

/********************************************/
//from main.c
extern volatile uint32_t Stime;
/********************************************/
//extern uint32_t distance, distance_now, distance_pj;
//extern uint8_t bian, bian_pre, bian_flag;
extern uint8_t quan;
//extern volatile uint32_t Stime_bian;

extern uint8_t csb_flag;

//extern uint8_t ReadNow, LastRead;
//extern float LKp, LKi, LKd;
//extern int8_t Er, pre_Er;
//extern int16_t sum_Er;
//extern static const int8_t jq[8];
//extern uint8_t GAB;
/********************************************/
void loop_screen0(void);
void loop_screen1(void);
void loop_screen2(void);
/********************************************/

void Select_loop(void);

void loop_car(void);
void loop_car_quan(void);
void loop_car_bz(void);

void pid1(void);

/********************************************/
extern CarState LoopMode;

extern LoopFunc loop_functions[];

/********************************************/
#endif
