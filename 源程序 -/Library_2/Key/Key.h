#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"

#define KEY_time  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15)		 //灯开关切换
#define KEY1_add  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13)		 //亮度切换
#define KEY1_del  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14)		 //颜色切换
#define KEY_set		GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12)		 //颜色切换
#define HC_SR501   GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7)

void Key_Init(void);//初始化

		 				    
#endif
