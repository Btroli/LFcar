#include "loopmode.h"

CarState LoopMode = Loop_SCREEN0;

LoopFunc loop_functions[] = {
	loop_screen0,
	loop_screen1,
	loop_screen2,
	Select_loop,
	loop_car,
	loop_car_quan,
	loop_car_bz
};

//stop
uint32_t distance = 0, distance_now = 0, distance_pj = 0;
uint8_t bian = 0, bian_pre = 0, bian_flag = 1;
uint8_t quan = 0;
volatile uint32_t Stime_bian = 0xFFFFFFFF + 1 - 200;	//2^32 - 200

//bizhang
uint8_t csb_flag = 0;

//pid1
uint8_t ReadNow, LastRead;
float LKp, LKi, LKd;
int8_t Er, pre_Er;
int16_t sum_Er;
static const int8_t jq[8] = {25, 20, 12, 5, -5, -12, -20, -25};
uint8_t GAB;


uint8_t i = 0;


void Select_loop(void) {

	LoopMode = (CarState)(Loop_CAR + value[MODE_raw][0]);

	Set_PWMA(0);
	Set_PWMB(0);

	return;
}

void loop_car_bz(void) {

	ReadNow = ReadAll();

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
			GA = -20;
			GB = 60;
		} else if (LastRead & RIGHT) {
			GA = 60;
			GB = -20;
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

		if (bian_flag && Stime - Stime_bian >= 50) {	//此处防止转弯不稳定
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
					LoopMode = Loop_SCREEN0;
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

void pidInit(void) {
	HIGH(STBY);

	SPDA = 0, SPDB = 0;
	PA = 0, PB = 0, pre_PA = 0, pre_PB = 0;
	sum_PA = 0, sum_PB = 0;

	LKp = value[1][0] * 100 + value[1][1] * 10 + value[1][2];
	LKi = cal_value(value[2]);
	LKd = value[3][0] * 100 + value[3][1] * 10 + value[3][2];
	GAB = value[4][0] * 10 + value[4][1];

	Er = 0;
	pre_Er = 0;
	sum_Er = 0;
}

