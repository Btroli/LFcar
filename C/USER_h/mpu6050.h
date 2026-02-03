#ifndef __MPU6050_H
#define __MPU6050_H

#include "stm32f10x.h"

#define	SCL		GPIO_Pin_8
#define	SDA		GPIO_Pin_9
#define	I2C_PORT	GPIOB

#define delay_T		0.01

extern float yaw, pitch, roll, temperature;

void I2C_SET(void);
void MPU6050_SET(void);
void MPU6050_update(void);
void Euler_cal(void);
void Euler_angle_clear(void);

void MPU6050_gz_update(void);
void Euler_yaw_cal(void);

//私有static函数
//static void reg_write(uint8_t reg, uint8_t value);
//static uint8_t reg_read(uint8_t reg);
//static uint8_t reg_readmore(uint8_t *buffer);

#endif
