// #include <msp430g2553.h>
// #include <in430.h>

// #include <oled.h>
// #include <oled_helper.h>

// #include <self_check.h>

// #include <utils.h>

// void button_init()
// {
//   P1SEL &= ~(BIT3 + BIT4 + BIT5 + BIT7); // 引脚配置 0101 1100
//   P1DIR &= ~(BIT3 + BIT4 + BIT5 + BIT7); //输入模式，按键引脚 红外寻迹0101 1100
//   P1REN |= BIT3;                         // P1.3引脚接上拉电阻
//   P1OUT |= (BIT3 + BIT4 + BIT5 + BIT7);  //按键引脚上拉

//   P2SEL &= ~BIT5; // P2.5按键引脚
//   P2DIR &= ~BIT5; // P2.5输入模式
//   P2OUT |= BIT5;  //按键引脚上拉
//   delay_ms(200);
// }

// void motor_init()
// {
//   P2SEL |= (BIT1 + BIT4);
//   P2DIR |= (BIT4 + BIT4);
//   P2OUT &= ~BIT1;
//   P2OUT &= ~BIT4;
// }

// void timer_init()
// {
//   P2DIR |= (BIT1 + BIT4); // P2.1和P2.4  配置为输出
//   P2SEL |= (BIT1 + BIT4); // P2.1和P2.4  配置为TimerA1的Out1和Out2输出
//   P2OUT &= ~BIT1;         // 置低
//   P2OUT &= ~BIT4;         // 置低
//   TA1CCR0 = 20000;        // PWM 周期 20MS
//   TA1CCTL1 = OUTMOD_7;
//   TA1CCR1 = 0; // PWM为0停止状态

//   TA1CCTL2 = OUTMOD_7;
//   TA1CCR2 = 0; // PWM为0停止状态

//   // TA1时钟设置      SMCLK做时钟源        8分频       向上计数模式
//   TA1CTL = (TASSEL1 + ID0 + ID1 + MC0);
// }

// void buzzer_init()
// {
//   P2DIR |= BIT0;
//   P2OUT &= ~BIT0;
//   delay_ms(200);
// }

// void internal_led_init()
// {
//   P1DIR |= (BIT0 + BIT6);
//   P1OUT &= ~(BIT0 + BIT6);
// }

// void go()
// {
//   OLED_CLS();
//   volatile unsigned cycles = 0;
//   for (;;)
//   {
//     cycles++;
//     oled_puts(0, 0, "C:");
//     oled_putint(16, 0, cycles);

//     if (IR1 && IR2) // end line
//     {
//       oled_puts(0, 2, "End detected  ");
//       TA1CCR1 = 0;
//       TA1CCR2 = 0;
//       while (KEY1 != 0)
//       {
//         P1OUT |= (BIT0 + BIT6);
//         delay_ms(1000);
//         P1OUT &= ~(BIT0 + BIT6);
//         delay_ms(1000);
//       }
//     }
//     else if (IR1 && !IR2)
//     {
//       oled_puts(0, 2, "Continue detected  ");
//     }
//     else if (!IR1 && IR2)
//     {
//       oled_puts(0, 2, "Turn detected  ");
//     }
//     else
//     {
//       P1OUT |= (BIT0 | BIT6);
//       oled_puts(0, 2, "Wrong status  ");
//     }

//     delay_ms(100);
//   }
// }

// int main()
// {
//   WDTCTL = WDTPW + WDTHOLD;

//   BCSCTL1 = CALBC1_8MHZ; // Set range
//   DCOCTL = CALDCO_8MHZ;

//   // Car motor
//   motor_init();
//   delay_ms(200);

//   // PWM timer
//   timer_init();
//   delay_ms(200);

//   // OLED
//   OLED_Init();
//   delay_ms(200);

//   // Buttons
//   button_init();
//   delay_ms(200);

//   // Buzzer
//   buzzer_init();
//   delay_ms(200);

//   // Internal LEDs
//   internal_led_init();
//   delay_ms(200);

//   self_check();

//   go();

//   for (;;)
//   {
//     P1DIR |= BIT0;
//     delay_ms(500);
//     P1DIR &= ~BIT0;
//     delay_ms(500);
//   }

//   return 0;
// }

#include <msp430.h>

#include "oledfont.h"
#include "oled.h" //里面有SDA和SCL引脚定义，移植时需注意修改
#include <oled_helper.h>

#include <utils.h>

#define hy1 (P1IN & BIT4) //寻迹检测
#define hy2 (P1IN & BIT5) //停止检测

#define key3 (P1IN & BIT3) //按键检测（开发板内置）（按下低电平，抬起高电平）
#define key2 (P1IN & BIT7) //按键检测（按下低电平，抬起高电平）
#define key1 (P2IN & BIT5) //按键检测（按下低电平，抬起高电平）

#define FM_L P2OUT &= ~BIT0 //蜂鸣器置低电平
#define FM_H P2OUT |= BIT0  //蜂鸣器置高电平

//***********************************************************************
//             TIMERA初始化，设置为UP模式计数
//***********************************************************************
int t_miao = 0, count, setmode = 15;
void TIMERA_Init(void)
{ //打开TA0捕捉/比较中断使能
  TA0CCTL0 = CCIE;
  // TA0时钟设置      SMCLK做时钟源        8分频            连续计数模式
  TA0CTL |= (TASSEL1 + ID0 + ID1 + MC1);
  //计数50000次开中断
  TA0CCR0 = 50000;
}

//***********************************************************************
//             TIMERA中断服务程序，需要判断中断类型
//***********************************************************************
// Timer A0 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A(void)
#elif defined(__GNUC__)
void __attribute__((interrupt(TIMER0_A0_VECTOR))) Timer_A(void)
#else
#error Compiler not supported!
#endif
{
  TA0CCR0 += 50000; // Add Offset to CCR0
  count++;

  if (count >= 20)
  {
    count = 0;
    t_miao++;
    P1OUT ^= BIT6; // LED闪烁提示
  }
}
//***********************************************************************
//             TIMERA初始化，结束
//***********************************************************************

/*********************小车启动程序**************************/

#define TURN_DELAY 200

int turn_constant = 30;
const int turn_multipier = 3;
const int turn_bias = 40;
#define TURN_MAX 120

int delaytime;
void go()
{
  P2DIR |= (BIT1 + BIT4); // P2.1和P2.4  配置为输出
  P2SEL |= (BIT1 + BIT4); // P2.1和P2.4  配置为TimerA1的Out1和Out2输出
  TA1CCR0 = 20000;        // PWM 周期 20MS
  TA1CCTL1 = OUTMOD_7;
  TA1CCR1 = 0; // PWM为0停止状态

  TA1CCTL2 = OUTMOD_7;
  TA1CCR2 = 0; // PWM为0停止状态

  // TA1时钟设置      SMCLK做时钟源        8分频       向上计数模式
  TA1CTL = (TASSEL1 + ID0 + ID1 + MC0);

  while (1)
  {
    OLED_ShowChar(80, 0, t_miao / 100 + '0'); //时钟运行提示
    OLED_ShowChar(88, 0, t_miao % 100 / 10 + '0');
    OLED_ShowChar(96, 0, t_miao % 10 + '0');

    if (hy2 != 0 && hy1 != 0) //终点检测到
    {
      TA1CCR1 = 0; //速度为0
      TA1CCR2 = 0;
      oled_puts(0, 0, "end ");

      while (1)
      {
        FM_L; //启动蜂鸣器
        delay_ms(100);
        FM_H; //关闭蜂鸣器
        delay_ms(100);
      }
    }
    else //没有到达终点
    {
      if (hy1 == 0) //红外1检测
      {
        oled_puts(0, 2, "hy1 detect    ");
      }
      else
      {
        oled_puts(0, 2, "hy1 not detect");
      }

      if (hy2 == 0) //红外2检测
      {
        oled_puts(0, 4, "hy2 detect     ");
      }
      else
      {
        oled_puts(0, 4, "hy2 not detect");
      }

      //  setmode=10-20  100-300 R
      if (hy1 == 0 && hy2 != 0) //靠近黑线
      {
        TA1CCR2 = (int)turn_constant * (100 + 20 * (setmode - 10)); //速度按照档位值

        turn_constant = turn_multipier * turn_constant + turn_bias;
        if (turn_constant > TURN_MAX)
        {
          turn_constant = TURN_MAX;
        }

        if (setmode <= 13) //另外一边速度慢，这样才可以靠近黑线
        {
          TA1CCR1 = (int)10.5 * 30 * 0.2;
          delay_ms(TURN_DELAY);
        }
        else if (setmode <= 15)
        {
          TA1CCR1 = (int)10.5 * 50 * 0.2;
          delay_ms(TURN_DELAY);
        }
        else if (setmode <= 17)
        {
          TA1CCR1 = (int)10.5 * 60 * 0.2;
          delay_ms(TURN_DELAY);
        }
        else
        {
          TA1CCR1 = (int)10.5 * 70 * 0.2;
          delay_ms(TURN_DELAY);
        }
        delay_ms(30);
      }
      else if (hy2 == 0 && hy1 != 0)
      {
        volatile int speed = 30.5 * (100 + 20 * (setmode - 10));
        TA1CCR1 = (int)speed; //速度按照档位值

        if (setmode <= 13) //另外一边速度慢，这样才可以远离黑线
        {
          TA1CCR2 = (int)speed;
          delay_ms(30);
        }
        else if (setmode <= 16)
        {
          TA1CCR2 = (int)speed * 0.8;
          delay_ms(30);
        }
        else
        {
          TA1CCR2 = (int)speed * 0.7;
          delay_ms(30);
        }
      }
      else if (hy1 == 0 && hy2 == 0)
      {
        TA1CCR1 = (int)30.5 * (100 + 20 * (setmode - 10));
        TA1CCR2 = (int)32 * (100 + 20 * (setmode - 10));
      }
    }
  }
}
/*********************小车运行结束**************************/

//
int main() //主函数
{
  WDTCTL = WDTPW + WDTHOLD;

  BCSCTL1 = CALBC1_8MHZ; // Set range
  DCOCTL = CALDCO_8MHZ;

  TIMERA_Init(); //设置TIMERA
  _EINT();

  P2SEL |= (BIT1 + BIT4); // P2.1和P2.4  配置为TimerA1的Out1和Out2输出
  P2DIR |= (BIT1 + BIT4); // P2.1和P2.4  配置为输出
  P2OUT &= ~BIT1;         // 置低
  P2OUT &= ~BIT4;         // 置低

  OLED_Init(); // OLED初始化
  delay_ms(200);

  //
  P1SEL &= ~(BIT3 + BIT4 + BIT5 + BIT7); // 引脚配置 0101 1100
  P1DIR &= ~(BIT3 + BIT4 + BIT5 + BIT7); //输入模式，按键引脚 红外寻迹0101 1100
  P1REN |= BIT3;                         // P1.3引脚接上拉电阻
  P1OUT |= (BIT3 + BIT4 + BIT5 + BIT7);  //按键引脚上拉

  P2SEL &= ~BIT5; // P2.5按键引脚
  P2DIR &= ~BIT5; // P2.5输入模式
  P2OUT |= BIT5;  //按键引脚上拉

  P2DIR |= BIT0; //蜂鸣器输出高
  P2OUT |= BIT0; //蜂鸣器输出高

  P1DIR |= (BIT0 + BIT6); // LED提示引脚
  P1OUT |= (BIT0 + BIT6);

  while (1)
  {
    oled_puts(0, 0, "stop");

    OLED_ShowChar(80, 0, t_miao / 100 + '0'); //时钟运行提示
    OLED_ShowChar(88, 0, t_miao % 100 / 10 + '0');
    OLED_ShowChar(96, 0, t_miao % 10 + '0');

    oled_puts(0, 6, "M=");
    oled_putint(32, 6, setmode);

    if (key2 == 0) //按键2 设定+1
    {
      if (setmode != 20)
        setmode++;
      delay_ms(300);
    }
    else if (key1 == 0) //按键1 设定-1
    {
      if (setmode != 10)
        setmode--;
      delay_ms(300);
    }
    else if (key3 == 0) //启动按键
    {
      oled_puts(0, 0, "run ");

      while (key3 == 0)
        ;             //等待松手
      delay_ms(1000); //等等3秒
      delay_ms(1000);
      delay_ms(1000);
      t_miao = 0;
      go(); //小车启动
    }

    if (hy1 == 0) //红外1检测
    {
      oled_puts(0, 2, "hy1 detect     ");
    }
    else
    {
      oled_puts(0, 2, "hy1 not detect");
    }

    if (hy2 == 0) //红外2检测
    {
      oled_puts(0, 4, "hy2 detect     ");
    }
    else
    {
      oled_puts(0, 4, "hy2 not detect");
    }

    P1OUT ^= BIT0; // LED闪烁提示
    delay_ms(100);
  }

  return 0;
}
