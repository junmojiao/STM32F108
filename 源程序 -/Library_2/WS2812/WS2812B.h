#ifndef __WS2812B_H
#define	__WS2812B_H

#include "stm32f10x.h"
#include "delay.h"	

//#define WS2812_IN_PIN	PA0

void Timer2_init(void);
void WS2812_send_single(uint8_t R,uint8_t G,uint8_t B,uint16_t len);//点亮某一位的LED
void WS2812_send_all(uint8_t R,uint8_t G,uint8_t B,uint16_t len);//点亮所有的LED

#endif /* __LED_H */
