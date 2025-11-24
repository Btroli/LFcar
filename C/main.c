#include "stm32f10x.h"
#include "key.h"
#include "pwm.h"
#include "mmgj.h"
#include "oled.h"
#include "delay.h"
//#include "button.h"
#include "encoder.h"
//#include "LightSensor.h"

/********************************************/

//#define LEFT	0b11100000;
//#define MID 	0b00011000;
//#define RIGHT	0b00000111;
//
//#define SPDA 3870
//#define SPDB 3600

#define value_len 10

//int speedA = SPDA, speedB = SPDB;
//int speedA0 = SPDA, speedB0 = SPDB;
//
//int Encoder_A, Encoder_B;

uint8_t count = 1, area = 0, option = 0, option_NUM = 10, sel_flag = 1, car_screen_flag = 0
                                      , value_num = 0;

int16_t EvalueA = 0, EvalueB = 0;

int16_t SPDA = 0, SPDB = 0;

uint8_t Kp, Ki, Kd;

int16_t GA = 40, GB = 0;
int16_t PA = 0, PB = 0, pre_PA = 0, pre_PB = 0;

unsigned int sum_PA = 0, sum_PB = 0;

uint8_t value[10][value_len] = {
	{4, 0, 0, 0, 0, 0, 0, 0, 0, 2},	//0
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 2},	//1
	{3, 5, 0, 0, 0, 0, 0, 0, 0, 2},	//2
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 2},	//3
	{0, 3, 0, 0, 0, 0, 0, 0, 0, 2},	//4
	{1, 2, 3, 4, 5, 6, 7, 8, 9, 5},	//5
	{1, 2, 3, 4, 5, 6, 7, 8, 9, 6},	//6
	{1, 2, 3, 4, 5, 6, 7, 8, 9, 7},	//7
	{1, 2, 3, 4, 5, 6, 7, 8, 9, 8},	//8
	{1, 2, 3, 4, 5, 6, 7, 8, 9, 9},	//9
};

uint8_t name[10][10] = {
	"GAonly2",	//0
	"GBonly2",	//1
	"Kp",	//2
	"Ki",	//3
	"Kd",	//4
	"name05",	//5
	"name06",	//6
	"name07",	//7
	"name08",	//8
	"name09",	//9
};

/********************************************/

void loop_screen0(void);
void loop_screen1(void);
void loop_car(void);
void pidInit(void);

/********************************************/

int main(void) {

	SystemInit();
	delay_init();
	KEY_SET();
	PWM_SET();
	OLED_SET();
	//OLED_DisplayTurn(1);
//	Button_SET();
	Encoder_PA_SET(&EvalueA, &EvalueB);
//	Guangmin_PG_SET();

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
	if (KEY_Scan(1) || KEY_Scan(2) || KEY_Scan(3) || KEY_Scan(4)) {
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
	OLED_Refresh();
	OLED_ClearRF();

	pre_PA = PA;
	pre_PB = PB;
	PA = GA - EvalueA;
	PB = GB - EvalueB;

	if (PA == 1 || PA == -1)
		PA = 0;

	sum_PA += PA;
	sum_PB += PB;


	SPDA = Kp * PA + Ki * sum_PA - Kd * (PA - pre_PA);

	//SPDA = Kp * PA + Ki * sum_PA + Kd * (PA - pre_PA);

	if (SPDA > 7000)
		SPDA = 7000;


	delay_ms(200);
}
void loop_screen1(void) {
	if (KEY_Scan(1)) {
		value_num++;
		if (value_num == value_len)
			value_num = 0;
	}
	if (KEY_Scan(2)) {
		if (value_num == value[option][value_len - 1]) {
			value[option][value_len - 1] += (value[option][value_len - 1] == 9) ? -9 : 1;
			value_num += (value_num == value_len - 1) ? 1 - value_len : 1;
		} else if (value_num < value[option][value_len - 1])
			value[option][value_num] += (value[option][value_num] == 9) ? -9 : 1;
		else
			value[option][value_num - 1] += (value[option][value_num - 1] == 9) ? -9 : 1;
	}
	for (uint8_t n = 0; n < value_len; n++) {
		if (n == value[option][value_len - 1])
			OLED_ShowChar(6 + n * 6, 30, '.', 12, (n == value_num && count) ? 0 : 1);
		else if (n < value[option][value_len - 1])
			OLED_ShowChar(6 + n * 6, 30, value[option][n] + '0', 12, (n == value_num && count) ? 0 : 1);
		else
			OLED_ShowChar(6 + n * 6, 30, value[option][n - 1] + '0', 12, (n == value_num && count) ? 0 : 1);
	}
	OLED_ShowString(6, 12, name[option], 12, 1);
	OLED_Refresh();
	OLED_ClearRF();
	if (KEY_Scan(3)) {
		car_screen_flag = 0;
		sel_flag = 1;
		OLED_ClearRF();

		pidInit();

		return;
	}
	if (KEY_Scan(4)) {
		sel_flag = 1;
		OLED_ClearRF();
		return;
	}
	delay_ms(100);
}
void loop_screen0(void) {
	if (KEY_Scan(3)) {
		sel_flag = 0;
		OLED_ClearRF();
		return;
	}
	if (KEY_Scan(4)) {
		car_screen_flag = 0;
		OLED_ClearRF();

		pidInit();

		return;
	}
	if (KEY_Scan(1)) {
		if (option < option_NUM - 1)
			option++;
		if (area < 2)
			area++;
	}
	if (KEY_Scan(2)) {
		if (option > 0)
			option--;
		if (area > 0)
			area--;
	}

	OLED_ShowString(6, 12 + 18 * 0, name[option - area + 0], 12, 1);
	OLED_ShowString(6, 12 + 18 * 1, name[option - area + 1], 12, 1);
	OLED_ShowString(6, 12 + 18 * 2, name[option - area + 2], 12, 1);

	OLED_DrawBoxLine(1, 9, 126, 25, 1);
	OLED_DrawBoxLine(1, 27, 126, 43, 1);
	OLED_DrawBoxLine(1, 45, 126, 61, 1);

	if (count)
		FuXuan(area);
	else
		FuXuan(area + 3);

	OLED_Refresh();
	OLED_ClearRF();
}
void pidInit(void) {
	GA = value[0][0] * 10 + value[0][1];
	GB = value[1][0] * 10 + value[1][1];
	Kp = value[2][0] * 10 + value[2][1];
	Ki = value[3][0] * 10 + value[3][1];
	Kd = value[4][0] * 10 + value[4][1];

	SPDA = 0, SPDB = 0;
	PA = 0, PB = 0, pre_PA = 0, pre_PB = 0;
	sum_PA = 0, sum_PB = 0;
}
