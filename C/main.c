#include "stm32f10x.h"
#include "key.h"
#include "pwm.h"
#include "tim.h"
#include "mmgj.h"
#include "oled.h"
#include "delay.h"
//#include "button.h"
#include "encoder.h"
#include "ultrasound.h"
#include "LightSensor.h"

/********************************************/

#define LEFT	0b11100000
#define MID 	0b00011000
#define RIGHT	0b00000111

#define value_len 10

uint8_t count = 1, area = 0, option = 0, option_NUM = 10, sel_flag = 1, car_screen_flag = 0, value_num = 0;

uint8_t loop_car_delaytime = 10;

//距离
uint32_t distance = 0, distance_now = 0, distance_pj = 0;

//TIM
volatile uint32_t Stime;
volatile int16_t EvalueA = 0, EvalueB = 0;

//pid0 给定值
int16_t GA = 40, GB = 40;

//pid1
uint8_t ReadNow, LastRead;
float LKp, LKi, LKd;
int8_t Er, pre_Er;
int16_t sum_Er;
static const int8_t jq[8] = {25, 20, 12, 5, -5, -12, -20, -25};
uint8_t GAB;

//stop
uint8_t bian = 0, bian_pre = 0, bian_flag = 1;
uint8_t quan = 0;
volatile uint32_t Stime_bian = 0xFFFFFFFF + 1 - 200;	//2^32 - 200

//bizhang
uint8_t csb_flag = 0;
uint16_t pre_time[10] = {0xFFFF};

//通用临时变量i
uint8_t i = 0;


uint8_t value[10][value_len] = {
	{0, 3, 8, 0, 0, 0, 0, 0, 0, 3},	//0
	{0, 0, 0, 0, 6, 0, 0, 0, 0, 3},	//1
	{0, 6, 6, 0, 0, 0, 0, 0, 0, 3},	//2
	{3, 0, 0, 0, 0, 0, 0, 0, 0, 2},	//3
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 3},	//4
	{0, 1, 0, 0, 0, 0, 0, 0, 0, 2},	//5
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 5},	//6
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 5},	//7
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 5},	//8
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 5},	//9
};

uint8_t name[10][10] = {
	"LKp",	//0
	"LKi",	//1
	"LKd",	//2
	"GAB",	//3
	"quan",	//4
	"BiZhang",	//5
	"name6",	//6
	"name7",	//7
	"name8",	//8
	"distance",	//9
};

/********************************************/

//void loop_screen0(void);
//void loop_screen1(void);
void loop_car(void);
void loop_car_quan(void);

void loop_car_bz(void);

void pidInit(void);

void pidInit_1(void);

void pid0(void);
void pid1(void);

void Quan0(void);
void Quan1(void);

/********************************************/

int main(void) {

	SystemInit();
	delay_init();
	KEY_SET();
	PWM_SET();
	OLED_SET();
//	Button_SET();
	Encoder_PA_SET();
	UltraSound_SET();
	Guangmin_PG_SET();
	TIM67_SET(&EvalueA, &EvalueB, &Stime);

	LOW(STBY);
	TRIG_LOW;

	OLED_Refresh();
	delay_ms(1000);

	while (1) {
		if (car_screen_flag) {
			count = 1 - count;
			if (sel_flag)
				loop_screen0();
			else
				loop_screen1();
			delay_ms(200);
			Set_PWMA(0);
			Set_PWMB(0);
		} else {
			if (quan)
				loop_car_quan();
			else if (csb_flag)
				loop_car_bz();
			else
				loop_car();
		}
	}
}

/********************************************/
//uint8_t csbtime(void) {
//	for (i = 0; i < 3+1; i++)
//		if (pre_time[i] > 1800)
//			return 0;
//	return 1;
//}
void loop_car_bz(void) {

	ReadNow = ReadAll();

	//for (i = 3; i > 0; i--)
	//	pre_time[i] = pre_time[i - 1];
	//pre_time[0] = echotime;

	//if (csbtime()) {
	if (echotime < 1400) {
		sum_PA = 0;
		sum_PB = 0;
		if (csb_flag == 1) {
			GA = 0;
			GB = 60;
		} else {
			GA = 60;
			GB = 0;
		}

		while (ReadAll()) {
			pid0();
			delay_ms(loop_car_delaytime);
		}

		if (csb_flag == 1) {
			GA = 25;
			GB = 18;
		} else {
			GA = 18;
			GB = 25;
		}
		i = 0;
		while (!ReadAll()) {
			pid0();
			delay_ms(loop_car_delaytime);
			i++;
			if (!(i % 2) && i < 2 * 12) {
				if (csb_flag == 1)
					GA++;
				else
					GB++;
			}
		}

		echotime = 0xFFFF;
		ReadNow = ReadAll();
		LastRead = ReadNow;

	} else if (ReadNow) {
		pid1();
		LastRead = ReadNow;
	} else {
		sum_PA = 0;
		sum_PB = 0;

		if (LastRead & LEFT) {
			GA = 0;
			GB = 60;
			csb_flag = 1;
		} else if (LastRead & RIGHT) {
			GA = 60;
			GB = 0;
			csb_flag = 2;
		}
	}

	CSB;
	pid0();
	delay_ms(loop_car_delaytime);
}

void loop_car(void) {

	ReadNow = ReadAll();

	if (ReadNow) {
		pid1();
		LastRead = ReadNow;
	} else {
		sum_PA = 0;
		sum_PB = 0;

		if (LastRead & LEFT) {
			GA = 0;
			GB = 60;
		} else if (LastRead & RIGHT) {
			GA = 60;
			GB = 0;
		}
	}

	pid0();

	delay_ms(loop_car_delaytime);
}

void loop_car_quan(void) {

	ReadNow = ReadAll();

	if (ReadNow) {
		pid1();
		LastRead = ReadNow;
		bian_flag = 1;
	} else {
		if (LastRead & LEFT) {
			GA = 0;
			GB = 60;
		} else if (LastRead & RIGHT) {
			GA = 60;
			GB = 0;
		}
		sum_PA = 0;
		sum_PB = 0;

		if (bian_flag && Stime - Stime_bian >= 200) {
			bian++;
			Stime_bian = Stime;
		}
		bian_flag = 0;
	}

	pid0();

	delay_ms(loop_car_delaytime);

	distance_now += EcdSpeed[(EvalueA + EvalueB) / 2];

	if (bian != bian_pre) {
		bian_pre = bian;

		if (bian == 1)
			distance = distance_now;
		else if (bian == 4 * quan) {
			distance_pj += distance_now;
			distance_pj /= (4 * quan - 1);
			distance = distance_pj - distance;
		} else
			distance_pj += distance_now;
		distance_now = 0;
	}

	if (bian == 4 * quan)
		if (distance_now >= distance) {
			LOW(STBY);
			while (1)
				if (KEY_Scan(4)) {
					car_screen_flag = 1;
					bian = 0;
					bian_pre = 0;
					bian_flag = 1;
					quan = 8;
					Stime_bian = 0xFFFFFFFF + 1 - 200;
					Stime = 0;
					return;
				}
		}
}

void pid1(void) {

	pre_Er = Er;

	Er = 0;

	for (i = 0; i < 8; i++)
		if (ReadNow & (1 << i))
			Er += jq[i];

	if (sum_Er < GAB * 100 && sum_Er > GAB * -100)
		sum_Er += Er;

	GB = LKp * Er + LKi * sum_Er + LKd * (Er - pre_Er);
	GB /= 200;

	GA = GAB + GB;
	GB = GAB - GB;

	GA = (-20 < GA && GA < 70) ? GA : ((GA < 0) ? -20 : 70);
	GB = (-20 < GB && GB < 70) ? GB : ((GB < 0) ? -20 : 70);

}
void pidInit_1(void) {
	LKp = value[0][0] * 100 + value[0][1] * 10 + value[0][2];

	LKi = cal_value(value[1]);

	LKd = value[2][0] * 100 + value[2][1] * 10 + value[2][2];

	GAB = value[3][0] * 10 + value[3][1];

	quan = value[4][0] * 100 + value[4][1] * 10 + value[4][2];

	csb_flag = value[5][0] * 10 + value[5][1];

	Er = 0;
	pre_Er = 0;
	sum_Er = 0;

}
