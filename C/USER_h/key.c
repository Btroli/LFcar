#include "key.h"

void KEY_SET(void) {
	GPIO_InitTypeDef GPIO_InitStructure; //定义结构体
	RCC_APB2PeriphClockCmd (KEY_CLK, ENABLE); 	//初始化KEY时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);		// 开启IO口复用时钟
//	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);	//	禁用JTAG 只使用SWD	，不然PA15、PB3、PB4无法正常使用

	GPIO_InitStructure.GPIO_Pin = KEY_PIN1 | KEY_PIN2 | KEY_PIN3 | KEY_PIN4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; //速度选择
	GPIO_Init(KEY_PORT, &GPIO_InitStructure);

}

uint8_t	KEY_Scan(uint8_t pin) {
	pin = 5 - pin;	//将顺序反转，按钮板反装。
	pin = (1 << (pin + 3));	//从pin1到pin4
	if ( GPIO_ReadInputDataBit ( KEY_PORT, pin) == 0 ) {	//检测按键是否被按下
		delay_ms(10);	//延时消抖
		if (GPIO_ReadInputDataBit ( KEY_PORT, pin) == 0) {	//再次检测是否为低电平
			while (GPIO_ReadInputDataBit ( KEY_PORT, pin) == 0);	//等待按键放开
			return KEY_ON;	//返回按键按下标志
		}
	}
	return KEY_OFF;

//#define GPIO_Pin_4	((uint16_t)0x0010)  // Pin 4 selected
//#define GPIO_Pin_5	((uint16_t)0x0020)  // Pin 5 selected
//#define GPIO_Pin_6	((uint16_t)0x0040)  // Pin 6 selected
//#define GPIO_Pin_7	((uint16_t)0x0080)  // Pin 7 selected
}

uint8_t	KEY_ScanNF(uint8_t pin) {
	pin = 5 - pin;	//将顺序反转，按钮板反装。
	pin = (1 << (pin + 3));
	if ( GPIO_ReadInputDataBit ( KEY_PORT, pin) == 0 ) {
		delay_ms(10);
		if (GPIO_ReadInputDataBit ( KEY_PORT, pin) == 0) {
			return KEY_ON;
		}
	}
	return KEY_OFF;
}
