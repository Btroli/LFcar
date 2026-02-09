#ifndef __OLED_H
#define __OLED_H

#include "AllHeader.h"

#define OLED_SCL_Clr() GPIO_ResetBits(GPIOD,GPIO_Pin_14)//SCL
#define OLED_SCL_Set() GPIO_SetBits(GPIOD,GPIO_Pin_14)

#define OLED_SDA_Clr() GPIO_ResetBits(GPIOD,GPIO_Pin_15)//DIN
#define OLED_SDA_Set() GPIO_SetBits(GPIOD,GPIO_Pin_15)

//#define OLED_RES_Clr() GPIO_ResetBits(GPIOD,GPIO_Pin_4)//RES
//#define OLED_RES_Set() GPIO_SetBits(GPIOD,GPIO_Pin_4)

#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

//*********************************************************************************************
/*
此屏幕损坏：程序中字符必须y>5才能完整显示。(12大的字符上方空两行。)

行：(0-63)，共64行。y
列：(0-127)，共128列。x

		0-y
		|
		x

*/
//*********************************************************************************************
/*
待办：清除OLED_GRAM中的一部分区域。设计函数。
*/
//*********************************************************************************************


void OLED_SET(void);	//初始化


void OLED_DisPlay_On(void);	//启动屏幕
void OLED_DisPlay_Off(void);	//关闭屏幕
void OLED_ColorTurn(u8 i);	//1=反相
void OLED_DisplayTurn(u8 i);	//1=反转180
void OLED_Refresh(void);	//将显存OLED_GRAM数据搬到屏幕上
void OLED_Clear(void);	//清屏
void OLED_ClearRF(void);	//注意：误删。只清除OLED_GRAM，不Refresh。

void OLED_DrawPoint(u8 x, u8 y, u8 t);	//画点(x,y)，t=0是擦掉点
void OLED_DrawLineX(u8 x, u8 mode);
void OLED_DrawLineY(u8 y, u8 mode);
void OLED_DrawLine(u8 x1, u8 y1, u8 x2, u8 y2, u8 mode);	//画线(x1,y1)->(x2,y2)
void OLED_DrawBoxLine(u8 x0, u8 y0, u8 x1, u8 y1, u8 mode);	//画框
void OLED_DrawBoxXuLine(u8 x0, u8 y0, u8 x1, u8 y1, u8 linelong, u8 mode);	//虚线框
void OLED_DrawBox(u8 x0, u8 y0, u8 x1, u8 y1, u8 mode);	//画矩(实心)
void OLED_DrawCircle(u8 x, u8 y, u8 r, u8 mode);	//圆(x,y),r

u8 StringLenX(u8 size, u8 num);	//字符串长度
void OLED_ShowChar(u8 x, u8 y, u8 chr, u8 size1, u8 mode);	//显示普通字符(x,y,编号,大小,反相=0<0就是写上0,1就是写上1>)
void OLED_ShowString(u8 x, u8 y, u8 *chr, u8 size1, u8 mode);	//显示字符串
void OLED_ShowNum(u8 x, u8 y, int32_t num, u8 len, u8 size, u8 mode);	//显示数字(x,y,num,位数,大小,反相=0)
void OLED_ShowNumNoLen(u8 x, u8 y, int32_t num, u8 size, u8 mode);
void OLED_ShowChinese(u8 x, u8 y, u8 num, u8 size1, u8 mode);	//中文

void OLED_ScrollDisplay(u8 num, u8 space, u8 mode);	//滚动显示(中文数，间隔，反相)
void OLED_ShowPicture(u8 x, u8 y, u8 sizex, u8 sizey, u8 BMP[], u8 mode);	//显示位图(x,y,x向大小,y向大小,图,反相)

//*********************************************************************************************

void I2C_Start(void);
void I2C_Stop(void);
void I2C_WaitAck(void);
void Send_Byte(u8 dat);
void OLED_WR_Byte(u8 dat, u8 mode);

#endif
