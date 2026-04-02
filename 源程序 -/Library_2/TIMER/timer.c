#include "timer.h"
u8 cnt;

void TIM3_PWM_Init(u16 arr,u16 psc)
{
		GPIO_InitTypeDef GPIOInitStructure;
		TIM_TimeBaseInitTypeDef TIMInitStructure;
		TIM_OCInitTypeDef TIMOCInitStructure;
		
		//1.使能GPIO时钟、定时器时钟、复用时钟
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

		//3.配置GPIO
		GPIOInitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
		GPIOInitStructure.GPIO_Pin   = GPIO_Pin_6 ;
		GPIOInitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		
		GPIO_Init(GPIOA, &GPIOInitStructure);

		
		//4.配置定时器
		TIMInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
		TIMInitStructure.TIM_CounterMode   = TIM_CounterMode_Up;
		TIMInitStructure.TIM_Period=arr;
		TIMInitStructure.TIM_Prescaler=psc;
		TIM_TimeBaseInit(TIM3, &TIMInitStructure);
		
		//5.配置PWM
		TIMOCInitStructure.TIM_OCMode      = TIM_OCMode_PWM1;
		TIMOCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
		TIMOCInitStructure.TIM_OCPolarity  = TIM_OCPolarity_High;
		
		TIM_OC1Init(TIM3, &TIMOCInitStructure);
		TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
		
		 TIM_ARRPreloadConfig(TIM3, ENABLE);
		TIM_Cmd(TIM3, ENABLE);
}












