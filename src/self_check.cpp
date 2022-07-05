#include <msp430g2553.h>
#include <in430.h>

#include <oled.h>
#include <oled_helper.h>

#include <utils.h>

#include <self_check.h>

void self_check()
{
#ifdef SELF_CHECK
    // Test OLED
    oled_puts(0, 0, "Hello, world!");

    delay_ms(1000);

    // Test buzzer
    BUZZER_SET_H;
    delay_ms(1000);
    BUZZER_SET_L;
    delay_ms(1000);
    BUZZER_SET_H;
    delay_ms(1000);
    BUZZER_SET_L;
    delay_ms(1000);
    oled_puts(0, 2, "Test Buzzer");

    delay_ms(1000);

    // Test LEDs
    P1OUT |= (BIT0 + BIT6);
    delay_ms(500);
    P1OUT &= ~(BIT0 + BIT6);
    delay_ms(500);
    P1OUT |= (BIT0 + BIT6);
    delay_ms(500);
    P1OUT &= ~(BIT0 + BIT6);
    oled_puts(0, 4, "Test LED");

    delay_ms(1000);

    // Test motors
    TA1CCR2 = 5000; //前进5秒
    TA1CCR1 = 5000;
    delay_ms(5000);
    TA1CCR2 = 0; //停止2秒
    TA1CCR1 = 0;
    delay_ms(2000);
    TA1CCR2 = 5000; //左轮转5秒
    TA1CCR1 = 0;
    delay_ms(5000);
    TA1CCR2 = 0; //停止2秒
    TA1CCR1 = 0;
    delay_ms(2000);
    TA1CCR2 = 0; //右轮转5秒
    TA1CCR1 = 5000;
    delay_ms(5000);
    TA1CCR2 = 0; //停止2秒
    TA1CCR1 = 0;
    delay_ms(2000);
    oled_puts(0, 6, "Test Motor");
#else
    oled_puts(0, 0, "Bypass self-checking");
#endif
    delay_ms(2000);
}
