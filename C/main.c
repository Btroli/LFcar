#include "stm32f10x.h"
#include "i2c.h"
#include "key.h"
#include "pwm.h"
#include "tim.h"
//#include "mmgj.h"
#include "oled.h"
#include "delay.h"
#include "Serial.h"
#include "encoder.h"
#include "mpu6050.h"
#include "ultrasound.h"
#include "LightSensor.h"
#include "graphic_mpu6050.h"

#include "math.h"

/********************************************/
void TIM6_Initt(void);
/********************************************/

void SendToVofa(float value1, float value2, float value3) {
	Serial_Printf("%.4f,%.4f,%.4f\n", value1, value2, value3);
}

/********************************************/

void loop(void) {

	Serial_Printf("%.4f,%.4f,%.4f,%.5f\n", yaw, pitch, roll, temperature / 1000);

	graphic_cube_show();

	OLED_Refresh();
	OLED_ClearRF();
	delay_ms(100);

	if (KEY_Scan(1))
		Euler_angle_clear();
}

/********************************************/

int main(void) {

	SystemInit();
	delay_init();
	KEY_SET();
	OLED_SET();

	OLED_Refresh();

	MPU6050_SET();
	TIM6_Initt();
	Serial_Init();

	for (uint8_t i = 0; i < 6; i++) {
		MPU6050_gz_update();
		delay_ms(10);
	}

	while (1)
		loop();
}

void TIM6_Initt(void) {
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	//TIM6
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

	TIM_TimeBaseStructure.TIM_Period = 99;          // 自动重装载值
	TIM_TimeBaseStructure.TIM_Prescaler = 7199;       // 预分频系数
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

	//中断
	TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM6, ENABLE);

	// NVIC
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

//中断函数
void TIM6_IRQHandler(void) {
	if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET) {

		MPU6050_update();
		Euler_cal();

		TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
	}
}

