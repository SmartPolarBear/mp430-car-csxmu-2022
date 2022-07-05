#ifndef __OLED_HELPER_H__
#define __OLED_HELPER_H__

#include <stdint.h>

void oled_puts(uint8_t x, uint8_t y, const char *str);
void oled_putint(uint8_t x, uint8_t y, unsigned int num);

#endif