#include "mmgj.h"

uint8_t count = 1, area = 0, option = 0, option_NUM = 10, value_num = 0;

uint8_t value[10][value_len] = {
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	//0
	{0, 3, 8, 0, 0, 0, 0, 0, 0, 3},	//1
	{0, 0, 0, 0, 6, 0, 0, 0, 0, 3},	//2
	{0, 6, 6, 0, 0, 0, 0, 0, 0, 3},	//3
	{3, 0, 0, 0, 0, 0, 0, 0, 0, 2},	//4
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 5},	//5
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 5},	//6
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 5},	//7
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 5},	//8
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 5},	//9
};

uint8_t name[10][10] = {
	"MODE",	//0
	"LKp",	//1
	"LKi",	//2
	"LKd",	//3
	"GAB",	//4
	"name5",	//5
	"name6",	//6
	"name7",	//7
	"name8",	//8
	"name9",	//9
};

//pid0 给定值
int16_t GA = 40, GB = 40;
//pid0
int16_t SPDA = 0, SPDB = 0;
uint8_t Kp_A = 50, Ki_A = 30, Kd_A = 7, Kp_B = 50, Ki_B = 30, Kd_B = 7;
int16_t PA = 0, PB = 0, pre_PA = 0, pre_PB = 0, sum_PA = 0, sum_PB = 0;

//TIM
volatile uint32_t Stime;
volatile int16_t EvalueA = 0, EvalueB = 0;

/********************************************/

void InitAll(void) {
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
}

/********************************************/

void cal_valueP(uint8_t *p, float *v) {
	static const float pow10[10] = {
		1e-9f, 1e-8f, 1e-7f, 1e-6f, 1e-5f, 1e-4f, 1e-3f, 1e-2f, 1e-1f, 1e0f
	};
	*v = 0;
	for (uint8_t i = 0; i < 9; ++i)
		*v = *v * 10 + *(p++);
	*v *= pow10[*p];
}

float cal_value(uint8_t *p) {
	static const float pow10[10] = {
		1e-9f, 1e-8f, 1e-7f, 1e-6f, 1e-5f, 1e-4f, 1e-3f, 1e-2f, 1e-1f, 1e0f
	};
	float v = 0;
	for (uint8_t i = 0; i < 9; ++i)
		v = v * 10 + *(p++);
	v *= pow10[*p];
	return v;
}

/********************************************/

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

void loop_screen2(void) {
#define modeNUM 3
	uint8_t *mn = &value[MODE_raw][0];
	static const uint8_t mode_name[modeNUM][5] = {
		{2, 3, 10, 11, 0xFF},	//普通模式
		{4, 5, 10, 11, 0xFF},	//方框模式
		{0, 1, 10, 11, 0xFF},	//避障模式
	};

	if (KEY_Scan(1))
		*mn = (*mn + 1) % modeNUM;	//不超过modeNUM，加1取余，下同理
	if (KEY_Scan(2))
		*mn = (*mn + modeNUM - 1) % modeNUM;
#undef modeNUM
	OLED_zhString(8, 24, 16, mode_name[*mn], 1);
	OLED_Refresh();
	OLED_ClearRF();
	if (KEY_Scan(3)) {
		LoopMode = Loop_CHOICE;
		OLED_ClearRF();
		pidInit();
		return;
	}
	if (KEY_Scan(4)) {
		LoopMode = Loop_SCREEN0;
		OLED_ClearRF();
		return;
	}
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
		LoopMode = Loop_CHOICE;

		value_num = 0;
		OLED_ClearRF();

		pidInit();

		return;
	}
	if (KEY_Scan(4)) {
		LoopMode = Loop_SCREEN0;
		value_num = 0;
		OLED_ClearRF();
		return;
	}
	count = 1 - count;
	delay_ms(200);
}

void loop_screen0(void) {
	if (KEY_Scan(3)) {
		LoopMode = Loop_SCREEN1;
		OLED_ClearRF();
		if (option == MODE_raw)
			LoopMode = Loop_SCREEN2;
		return;
	}
	if (KEY_Scan(4)) {
		LoopMode = Loop_CHOICE;
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
	count = 1 - count;
	delay_ms(200);
}

/********************************************/

void pid0_show(void) {
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
}

void pid0(void) {
	if (KEY_Scan(4)) {
		LoopMode = Loop_SCREEN0;
		Set_PWMA(0);
		Set_PWMB(0);
		return;
	}

	Set_PWMA(SPDA);
	Set_PWMB(SPDB);

	pid0_show();

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
