#ifndef __OLED_H__
#define __OLED_H__

#include <msp430g2553.h>

#define OLED_HIGH P2OUT
#define OLED_DIR P2DIR

#define OLED_SCL_LOW BIT3
#define OLED_SDA_LOW BIT2

#define BRIGHTNESS 0xCF
#define X_WIDTH 128
#define Y_WIDTH 64

#define OLED_CMD 0  //写命令
#define OLED_DATA 1 //写数据

void OLED_WrDat(unsigned char IIC_Data);

void OLED_WrCmd(unsigned char IIC_Command);

void OLED_Set_Pos(unsigned char x, unsigned char y);

void OLED_Fill(unsigned char bmp_dat);

void OLED_CLS(void);

void OLED_WR_Byte(unsigned dat, unsigned cmd);

void OLED_ShowChinese(unsigned char x, unsigned char y, unsigned char no);

void OLED_ShowChar(unsigned char x, unsigned char y, unsigned char chr);

void OLED_Init(void);

#endif