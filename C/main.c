#include "stm32f10x.h"
#include "i2c.h"
#include "key.h"
#include "pwm.h"
#include "tim.h"
//#include "mmgj.h"
#include "oled.h"
#include "delay.h"
#include "encoder.h"
#include "ultrasound.h"
#include "LightSensor.h"

/********************************************/
#define	SCL		GPIO_Pin_8
#define	SDA		GPIO_Pin_9
#define	I2C_PORT	GPIOB
/********************************************/
float ax, ay, az, gx, gy, gz, temperature;
/********************************************/

void I2C_SET(void) {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_I2C1, ENABLE);
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, DISABLE);	//关闭时钟
	/*
	 * AFIO：复用功能IO控制器，与GPIO并列。
	 * 重映射寄存器相当于一个触发器，只有时钟信号的时候才能改变状态，所以需要打开AFIO时钟。
	 * 这里是将I2C默认的PB6,7改道(重映射)至PB8,9。（I2C1只有两个可能）
	 * 开了AFIO时钟后，通常不关，不知道为什么，反正通常不关。
	 */

	//PB8,9
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = SCL | SDA;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;	//开漏复用输出模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//I2C1 初始化，格式同上。
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	I2C_InitTypeDef I2C_InitStructure;
	I2C_InitStructure.I2C_ClockSpeed = 400000;	//通信速度：400k，见于6050规格书
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;	//占空比：1/2
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_Init(I2C1, &I2C_InitStructure);
	//I2C_Cmd(I2C1, ENABLE);	//使能I2C1
}

//写入value值 于 寄存器reg
static void reg_write(uint8_t reg, uint8_t value) {
	uint8_t send_value[] = {reg, value};
	My_I2C_SendBytes(I2C1, 0xd0, send_value, 2);
}

//读取值 从 寄存器reg
static uint8_t reg_read(uint8_t reg) {
	My_I2C_SendBytes(I2C1, 0xd0, &reg, 1);
	uint8_t receive_value;

	My_I2C_ReceiveBytes(I2C1, 0xd0, &receive_value, 1);

	return receive_value;
}

/*
static uint8_t reg_readmore(void) {
	My_I2C_SendBytes(I2C1, 0xd0, &reg, 1);
	uint8_t receive_value;

	My_I2C_ReceiveBytes(I2C1, 0xd0, &receive_value, 1);

	return receive_value;
}
*/

void MPU6050_SET(void) {
	I2C_SET();

	reg_write(0x6b, 0x80);	//复位
	delay_ms(100);

	reg_write(0x6b, 0x00);	//关闭睡眠模式

	reg_write(0x1b, 0x18);	//陀螺仪量程 = +-2000 (度/s)
	reg_write(0x1c, 0x00);	//加速度计量程 = +-2 (g)
	//注：陀螺仪量程最大挡位精度低，可能不合适。
	//加速度计最低挡位，应该足够。
}

void MPU6050_update(void) {
	/* 版本一 */

	int16_t ax_raw = (int16_t)((reg_read(0x3b) << 8) + reg_read(0x3c));
	int16_t ay_raw = (int16_t)((reg_read(0x3d) << 8) + reg_read(0x3e));
	int16_t az_raw = (int16_t)((reg_read(0x3f) << 8) + reg_read(0x40));
	int16_t temperature_raw = (int16_t)((reg_read(0x41) << 8) + reg_read(0x42));
	int16_t gx_raw = (int16_t)((reg_read(0x43) << 8) + reg_read(0x44));
	int16_t gy_raw = (int16_t)((reg_read(0x45) << 8) + reg_read(0x46));
	int16_t gz_raw = (int16_t)((reg_read(0x47) << 8) + reg_read(0x48));

	ax = ax_raw * 6.1035e-5f;
	ay = ay_raw * 6.1035e-5f;
	az = az_raw * 6.1035e-5f;
	gx = gx_raw * 6.1035e-2f;
	gy = gy_raw * 6.1035e-2f;
	gz = gz_raw * 6.1035e-2f;
	temperature = temperature_raw / 340 + 36.53;

	/**/


}
/********************************************/

void loop(void) {
	MPU6050_update();
	delay_ms(100);
	OLED_ShowNumNoLen(6, 12 + 10 * 0, ax * 1000, 12, 1);
	OLED_ShowNumNoLen(6, 12 + 10 * 1, ay * 1000, 12, 1);
	OLED_ShowNumNoLen(6, 12 + 10 * 2, az * 1000, 12, 1);
	OLED_ShowNumNoLen(6, 12 + 10 * 3, temperature, 12, 1);
	OLED_Refresh();
	OLED_ClearRF();
	delay_ms(100);
}

int main(void) {

	SystemInit();
	delay_init();
	KEY_SET();
	PWM_SET();
	OLED_SET();
	Encoder_PA_SET();
	UltraSound_SET();
	Guangmin_PG_SET();
	//TIM67_SET(&EvalueA, &EvalueB, &Stime);

	LOW(STBY);
	TRIG_LOW;

	OLED_Refresh();
	delay_ms(1000);


	MPU6050_SET();
	while (1)
		loop();
}
