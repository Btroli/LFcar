#include "stm32f10x.h"
#include "key.h"
#include "pwm.h"
#include "mmgj.h"
#include "oled.h"
#include "delay.h"
//#include "button.h"
#include "encoder.h"
#include "LightSensor.h"

/********************************************/

#define LEFT	0b11100000
#define MID 	0b00011000
#define RIGHT	0b00000111

#define value_len 10

uint8_t count = 1, area = 0, option = 0, option_NUM = 10, sel_flag = 1, car_screen_flag = 0, value_num = 0;

//pid0
int16_t SPDA = 0, SPDB = 0;
uint8_t Kp_A = 50, Ki_A = 30, Kd_A = 7, Kp_B = 50, Ki_B = 30, Kd_B = 7;
uint8_t loop_car_delaytime = 10;
int16_t PA = 0, PB = 0, pre_PA = 0, pre_PB = 0, sum_PA = 0, sum_PB = 0;

//距离
uint32_t distance = 0, distance_now = 0, distance_pj = 0;

//TIM
volatile uint32_t Stime;
volatile int16_t EvalueA = 0, EvalueB = 0;

//给定值
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
uint8_t quan = 8;
volatile uint32_t Stime_bian = 0xFFFFFFFF + 1 - 200;	//2^32 - 200


uint8_t value[10][value_len] = {
	{0, 3, 8, 0, 0, 0, 0, 0, 0, 3},	//0
	{0, 0, 0, 0, 6, 0, 0, 0, 0, 3},	//1
	{0, 6, 6, 0, 0, 0, 0, 0, 0, 3},	//2
	{2, 0, 0, 0, 0, 0, 0, 0, 0, 2},	//3
	{0, 0, 1, 0, 0, 0, 0, 0, 0, 3},	//4
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 5},	//5
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 5},	//6
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 5},	//7
	{0, 1, 0, 0, 0, 0, 0, 0, 0, 5},	//8
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 5},	//9
};

uint8_t name[10][10] = {
	"LKp",	//0
	"LKi",	//1
	"LKd",	//2
	"GAB",	//3
	"quan",	//4
	"name5",	//5
	"name6",	//6
	"name7",	//7
	"delay ms",	//8
	"distance",	//9
};

/********************************************/

//void loop_screen0(void);
//void loop_screen1(void);
void loop_car(void);
void pidInit(void);

void pidInit_1(void);

void pid0(void);
void pid1(void);

/********************************************/

int main(void) {

	SystemInit();
	delay_init();
	KEY_SET();
	PWM_SET();
	OLED_SET();
//	Button_SET();
	Encoder_PA_SET(&EvalueA, &EvalueB, &Stime);
	Guangmin_PG_SET();

	LOW(STBY);

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
		} else
			loop_car();
	}
}

/********************************************/

void loop_car(void) {

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

	for (uint8_t i = 0; i < 8; i++)
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
	//LKi = value[1][0] * 10 + value[1][1];

	LKi = cal_value(value[1]);
	//LKd = cal_value(value[2]);

	LKd = value[2][0] * 100 + value[2][1] * 10 + value[2][2];

	GAB = value[3][0] * 10 + value[3][1];

	quan = value[4][0] * 100 + value[4][1] * 10 + value[4][2];

	//值=cal_value(value[i]);	//第i行的值

	Er = 0;
	pre_Er = 0;
	sum_Er = 0;

}
void pid0(void) {
	if (KEY_Scan(4)) {
		car_screen_flag = 1;
		return;
	}

	Set_PWMA(SPDA);
	Set_PWMB(SPDB);
	OLED_ShowString(6, 12, "SPDA", 12, 1);
	OLED_ShowString(70, 12, "SPDB", 12, 1);
	OLED_ShowNum(6, 12 + 10, SPDA, 4, 12, 1);
	OLED_ShowNum(70, 12 + 10, SPDB, 4, 12, 1);
	OLED_ShowString(6, 12 + 20, "ECDA", 12, 1);
	OLED_ShowString(70, 12 + 20, "ECDB", 12, 1);
	OLED_ShowNum(6, 12 + 30, EvalueA, 4, 12, 1);
	OLED_ShowNum(70, 12 + 30, EvalueB, 4, 12, 1);

	OLED_ShowString(36, 12, "GA", 12, 1);
	OLED_ShowNum(36, 12 + 10, GA, 2, 12, 1);
	OLED_ShowString(36, 12 + 20, "GB", 12, 1);
	OLED_ShowNum(36, 12 + 30, GB, 2, 12, 1);


	OLED_Refresh();
	OLED_ClearRF();

	pre_PA = PA;
	pre_PB = PB;
	PA = GA - EvalueA;
	PB = GB - EvalueB;

	sum_PA += PA;
	sum_PB += PB;

	SPDA = Kp_A * PA + Ki_A * sum_PA - Kd_A * (PA - pre_PA);
	SPDB = Kp_B * PB + Ki_B * sum_PB - Kd_B * (PB - pre_PB);

	if (SPDA > 7000)
		SPDA = 7000;
	if (SPDB > 7000)
		SPDB = 7000;

}
void pidInit(void) {
	HIGH(STBY);

	loop_car_delaytime = value[8][0] * 100 + value[8][1] * 10 + value[8][2];

	if (!loop_car_delaytime)	//delay_ms(0)会爆炸
		loop_car_delaytime++;

	SPDA = 0, SPDB = 0;
	PA = 0, PB = 0, pre_PA = 0, pre_PB = 0;
	sum_PA = 0, sum_PB = 0;
}
