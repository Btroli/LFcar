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

void SendToVofa(float value1, float value2, float value3) {
	Serial_Printf("%.4f,%.4f,%.4f\n", value1, value2, value3);
}

void loop(void) {

	MPU6050_update();
	Euler_cal();

	//SendToVofa(yaw, pitch, roll);
	Serial_Printf("%.4f,%.4f,%.4f,%.5f\n", yaw, pitch, roll, temperature / 1000);

	graphic_cube_show();

	OLED_Refresh();
	OLED_ClearRF();
	MPU6050_update();
	delay_ms(10);

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
	delay_ms(1000);

	MPU6050_SET();
	Serial_Init();
	while (1)
		loop();
}

