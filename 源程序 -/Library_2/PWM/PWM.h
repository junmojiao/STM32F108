#ifndef __PWM_H
#define	__PWM_H


#include "stm32f10x.h"
void PWM_Init(u16 arr,u16 psc);
void SetProportion(uint16_t arr,uint16_t P);


#endif /* __PWM_H */
