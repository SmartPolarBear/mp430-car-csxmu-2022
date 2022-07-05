#include <oled.h>
#include <stdint.h>

void oled_puts(uint8_t x, uint8_t y, const char *str)
{
    while (*str)
    {
        OLED_ShowChar(x += 8, y, *str++);
    }
}

void oled_putint(uint8_t x, uint8_t y, unsigned int num)
{
    while (num)
    {
        unsigned int digit = num % 10;
        num /= 10;
        OLED_ShowChar(x += 8, y, '0' + digit);
    }
}