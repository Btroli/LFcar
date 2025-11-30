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

//#define LEFT	0b11100000;
//#define MID 	0b00011000;
//#define RIGHT	0b00000111;

#define value_len 10

uint8_t count = 1, area = 0, option = 0, option_NUM = 10, sel_flag = 1, car_screen_flag = 0, value_num = 0;

unsigned int Stime;

int16_t EvalueA = 0, EvalueB = 0;

int16_t SPDA = 0, SPDB = 0;

uint8_t Kp_A, Ki_A, Kd_A, Kp_B, Ki_B, Kd_B;

uint8_t loop_car_delaytime = 10;

int16_t GA = 40, GB = 40;

int16_t PA = 0, PB = 0, pre_PA = 0, pre_PB = 0, sum_PA = 0, sum_PB = 0;

uint32_t distance = 120000;	//期望120cm，量乘1000倍。
uint32_t distance_now = 0;

uint8_t value[10][value_len] = {
	{4, 0, 0, 0, 0, 0, 0, 0, 0, 2},	//0
	{4, 0, 0, 0, 0, 0, 0, 0, 0, 2},	//1
	{5, 0, 0, 0, 0, 0, 0, 0, 0, 2},	//2
	{3, 0, 0, 0, 0, 0, 0, 0, 0, 2},	//3
	{0, 7, 0, 0, 0, 0, 0, 0, 0, 2},	//4
	{5, 0, 0, 0, 0, 0, 0, 0, 0, 2},	//5
	{3, 0, 0, 0, 0, 0, 0, 0, 0, 2},	//6
	{0, 7, 0, 0, 0, 0, 0, 0, 0, 2},	//7
	{0, 1, 0, 0, 0, 0, 0, 0, 0, 3},	//8
	{9, 0, 0, 1, 2, 0, 0, 0, 0, 9},	//9
};

uint8_t name[10][10] = {
	"GA",	//0
	"GB",	//1
	"Kp A",	//2
	"Ki A",	//3
	"Kd A",	//4
	"Kp B",	//5
	"Ki B",	//6
	"Kd B",	//7
	"delay ms",	//8
	"distance",	//9
};

/********************************************/

//void loop_screen0(void);
//void loop_screen1(void);
void loop_car(void);
void pidInit(void);

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

	HIGH(STBY);

//	ReadNow = ReadAll();
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
	if (KEY_Scan(4)) {
		car_screen_flag = 1;
		return;
	}
	if (KEY_Scan(1)) {
		GA += 10;
		if (GA > 80)
			GA -= 80;
		GB = GA;
	}
	if (KEY_Scan(2)) {
		GA += 20;
		if (GA > 80)
			GA -= 80;
		GB = GA;
	}
	if (KEY_Scan(3)) {
		GA += 30;
		if (GA > 80)
			GA -= 80;
		GB = GA;
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

	//if (PA == 1 || PA == -1)
	//	PA = 0;
	//if (PB == 1 || PB == -1)
	//	PB = 0;

	sum_PA += PA;
	sum_PB += PB;


	SPDA = Kp_A * PA + Ki_A * sum_PA - Kd_A * (PA - pre_PA);
	SPDB = Kp_B * PB + Ki_B * sum_PB - Kd_B * (PB - pre_PB);

	//SPDA = Kp * PA + Ki * sum_PA + Kd * (PA - pre_PA);

	if (SPDA > 7000)
		SPDA = 7000;
	if (SPDB > 7000)
		SPDB = 7000;

	distance_now += EcdSpeed[(EvalueA + EvalueB) / 2];

	if (distance <= distance_now) {
		Set_PWMA(-3000);
		Set_PWMB(-3000);
		delay_ms(10);
		LOW(STBY);

		while (1)
			if (KEY_Scan(4)) {
				car_screen_flag = 1;
				distance_now = 0;
				Stime = 0;
				return;
			}

	}

//	GA = 28;		//加权小测验
//	GB = 28;
//	uint8_t rn = ReadAll();
//	for (uint8_t i = 0; i < 8; i++) {
//		int8_t nnn[8] = {16, 8, 4, 2, -2, -4, -8, -16};
//		if (rn & (1 << i)) {
//			GA += nnn[i]/2;
//			GB -= nnn[i]/2;
//		}
//	}

	delay_ms(loop_car_delaytime);
}
void pidInit(void) {
	GA = value[0][0] * 10 + value[0][1];
	GB = value[1][0] * 10 + value[1][1];
	Kp_A = value[2][0] * 10 + value[2][1];
	Ki_A = value[3][0] * 10 + value[3][1];
	Kd_A = value[4][0] * 10 + value[4][1];
	Kp_B = value[5][0] * 10 + value[5][1];
	Ki_B = value[6][0] * 10 + value[6][1];
	Kd_B = value[7][0] * 10 + value[7][1];

	distance = cal_value(value[9]);

	HIGH(STBY);

	loop_car_delaytime = value[8][0] * 100 + value[8][1] * 10 + value[8][2];
	if (!loop_car_delaytime)	//delay_ms(0)会爆炸
		loop_car_delaytime++;

	SPDA = 0, SPDB = 0;
	PA = 0, PB = 0, pre_PA = 0, pre_PB = 0;
	sum_PA = 0, sum_PB = 0;
}
