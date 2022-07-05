#ifndef __UTILS_H__
#define __UTILS_H__

//延时函数，IAR自带，经常使用到
#define CPU_F ((double)8000000) //外部高频晶振8MHZ

#define delay_us(x) __delay_cycles((long)(CPU_F * (double)x / 1000000.0))
#define delay_ms(x) __delay_cycles((long)(CPU_F * (double)x / 1000.0))

#define IR1 (P1IN & BIT4) //寻迹检测
#define IR2 (P1IN & BIT5) //停止检测

#define KEY3 (P1IN & BIT3) //按键检测（开发板内置）（按下低电平，抬起高电平）
#define KEY2 (P1IN & BIT7) //按键检测（按下低电平，抬起高电平）
#define KEY1 (P2IN & BIT5) //按键检测（按下低电平，抬起高电平）

#define BUZZER_SET_L (P2OUT &= ~BIT0) //蜂鸣器置低电平
#define BUZZER_SET_H (P2OUT |= BIT0)  //蜂鸣器置高电平

#endif