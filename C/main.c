#include "stm32f10x.h"
#include "key.h"
//#include "pwm.h"
#include "oled.h"
#include "delay.h"
//#include "button.h"
//#include "encoder.h"
//#include "LightSensor.h"

/********************************************/

#define LEFT	0b11100000;
#define MID 	0b00011000;
#define RIGHT	0b00000111;

#define SPDA 3870
#define SPDB 3600

#define value_len 10

int speedA = SPDA, speedB = SPDB;
int speedA0 = SPDA, speedB0 = SPDB;

int Encoder_A, Encoder_B;

uint8_t count = 1, area = 0, option = 0, option_NUM = 10, sel_flag = 1, car_screen_flag = 1
                                      , value_num = 0;

//uint8_t value[10][value_len] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 6};
uint8_t value[10][value_len] = {
	{1, 2, 3, 4, 5, 6, 7, 8, 9, 0},
	{1, 2, 3, 4, 5, 6, 7, 8, 9, 1},
	{1, 2, 3, 4, 5, 6, 7, 8, 9, 2},
	{1, 2, 3, 4, 5, 6, 7, 8, 9, 3},
	{1, 2, 3, 4, 5, 6, 7, 8, 9, 4},
	{1, 2, 3, 4, 5, 6, 7, 8, 9, 5},
	{1, 2, 3, 4, 5, 6, 7, 8, 9, 6},
	{1, 2, 3, 4, 5, 6, 7, 8, 9, 7},
	{1, 2, 3, 4, 5, 6, 7, 8, 9, 8},
	{1, 2, 3, 4, 5, 6, 7, 8, 9, 9},
};

uint8_t name[10][10] = {
	"name00",
	"name01",
	"name02",
	"name03",
	"name04",
	"name05",
	"name06",
	"name07",
	"name08",
	"name09",
};

/********************************************/

void loop_screen0(void);
void loop_screen1(void);
void loop_car(void);
void FuXuan(uint8_t n);

/********************************************/

int main(void) {

	SystemInit();
	delay_init();
	KEY_SET();
//	PWM_SET();
	OLED_SET();
//	Button_SET();
//	Encoder_PA_SET();
//	Guangmin_PG_SET();
//
//	HIGH(STBY);
//
//	ReadNow = ReadAll();
	delay_ms(1000);

	while (1) {
		if (car_screen_flag) {
			count = 1 - count;
			if (sel_flag)
				loop_screen0();
			else
				loop_screen1();
			delay_ms(200);
		} else
			loop_car();
	}
}

/********************************************/

void loop_screen1(void) {
	if (KEY_Scan(1)) {
		value_num++;
		if (value_num == value_len)
			value_num = 0;
	}
	if (KEY_Scan(2)) {
		if (value_num == value[option][value_len - 1])
			value[option][value_len - 1] += (value[option][value_len - 1] == 9) ? -9 : 1;
		else if (value_num < value[option][value_len - 1])
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
	delay_ms(100);
}
void loop_car(void) {}
void loop_screen0(void) {
	if (KEY_Scan(3)) {
		sel_flag = 0;
		OLED_ClearRF();
		return;
	}
	if (KEY_Scan(4)) {
		car_screen_flag = 0;
		OLED_ClearRF();
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
void FuXuan(uint8_t n) {
	if (n < 3) {
		OLED_DrawBoxXuLine(0, 8 + 18 * n, 127, 26 + 18 * n, 3, 1);
		OLED_DrawBoxXuLine(2, 10 + 18 * n, 125, 24 + 18 * n, 3, 1);
	} else {
		n -= 3;
		OLED_DrawBoxLine(0, 8 + 18 * n, 127, 26 + 18 * n, 1);
		OLED_DrawBoxLine(2, 10 + 18 * n, 125, 24 + 18 * n, 1);
		OLED_DrawBoxXuLine(0, 8 + 18 * n, 127, 26 + 18 * n, 3, 0);
		OLED_DrawBoxXuLine(2, 10 + 18 * n, 125, 24 + 18 * n, 3, 0);
	}
}
