#include "PWM.h"
void PWM_Init(u16 arr,u16 psc) //频率=72M/(psc+1)*(arr+1)
{ 
      GPIO_InitTypeDef GPIO_InitStructure;
      TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
      TIM_OCInitTypeDef TIM_OCInitStructure;

      //使能定时器TIM4时钟，注意TIM4时钟为APB1，而非APB2
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
      //使能PWM输出GPIO口时钟
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE); 
                                                                          
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;//定时器TIM4的PWM输出通道1，TIM4_CH1
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//复用推挽输出
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
      GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIO

      TIM_TimeBaseStructure.TIM_Period = arr;//自动重装值
      TIM_TimeBaseStructure.TIM_Prescaler =psc; //时钟预分频数
      TIM_TimeBaseStructure.TIM_ClockDivision = 0;
      TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//TIM向上计数模式
      TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //初始化TIM4
     
      //初始化TIM4_CH1的PWM模式
      TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//设置PWM模式1
      TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//比较输出使能
      TIM_OCInitStructure.TIM_Pulse = 0; //
      TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//输出极性为高
      TIM_OC1Init(TIM4, &TIM_OCInitStructure);//初始化TIM4_CH1

      //使能4个通道的预装载寄存器
      TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);//OC1
      TIM_ARRPreloadConfig(TIM4, ENABLE); //使能重装寄存器

      TIM_Cmd(TIM4, ENABLE);//使能定时器TIM4，准备工作 
}

void SetProportion(uint16_t arr,uint16_t P)  //arr与上文相同，P为比例(P%)
{
	TIM_SetCompare4(TIM4,P*(arr+1)/100);
}
