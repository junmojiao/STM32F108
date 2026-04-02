#include "wave.h"
#include "delay.h"
#include "sys.h"

#define HCSR04_TRIG GPIO_Pin_8  //超声波发射引脚
#define HCSR04_ECHO GPIO_Pin_9  //超声波接受引脚
#define HCSR04_PORT GPIOB

#define TRIG_SEND PBout(8)
#define ECHO_RECEIVE PBin(9)

u8 temperature=20;
u16 msHcCount;

void HCSR04_Init(void)
{  
		NVIC_InitTypeDef NVIC_InitStructure;
	
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;     //定义定时器结构体
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
     
    //IO 初始化
    GPIO_InitStructure.GPIO_Pin = HCSR04_TRIG;       	//发送高点平引脚
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//推挽输出
    GPIO_Init(HCSR04_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(HCSR04_PORT,HCSR04_TRIG);
     
    GPIO_InitStructure.GPIO_Pin = HCSR04_ECHO;    	 //返回电平引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;	
    GPIO_Init(HCSR04_PORT, &GPIO_InitStructure);  
    GPIO_ResetBits(HCSR04_PORT,HCSR04_ECHO);	
	 
	//定时器初始化使用基本定时器2
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);   //打开TIM2定时器时钟

	TIM_DeInit(TIM2);
	TIM_TimeBaseStructure.TIM_Period = 999; //(1000-1); //设置在下一个更新时间装入活动的自动重装载寄存器周期的值         计数到1000为1ms
	TIM_TimeBaseStructure.TIM_Prescaler =(72-1); //设置用来作为TIMx时钟频率除数的预分频  1M的计时频率 1US计数
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;	//不分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位	 
		
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);   	//清除更新中断避免立即产生更新中断
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);    //启用计时器更新中断

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;            	//设置中断服务
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  	//主优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;         	//次优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;        		//启用中断
	NVIC_Init(&NVIC_InitStructure);
	
    TIM_Cmd(TIM2,DISABLE);     
}

//定时器中断服务程序
void TIM2_IRQHandler(void)   //TIM2中斷
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //检查TIM更新中断是否发生
    {
         TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  //清除TIM更新中断标志位
         msHcCount++;
    }
}

//开始计数
static void StartTimer()        //打开定时器
{
    TIM_SetCounter(TIM2,0);	//清除计数
    msHcCount = 0;
    TIM_Cmd(TIM2, ENABLE);  //启用TIM2外设
}
 
static void StopTimer() 	//关闭定时器外设
{
    TIM_Cmd(TIM2, DISABLE); //关闭TIM定时器外设
}
 
//获取计时器时间
float GetEchoTimer(void)
{
    u32 t = 0;
    t = msHcCount*1000;			//得到ms
    t += TIM_GetCounter(TIM2);	//得到us
	TIM2->CNT = 0;  			//将TIM2定时计数器的寄存器清零
	delay_ms(50);
    return t;
}

//取五次数据平均值，为了使测距所得到的值更加精确可采取多次测距的平均值来使用应用中
float HCSR04GetLength(void)
{
	u32 t = 0;
	int i = 0;
	float lengthTemp = 0;  
	float sum = 0; 	
	float distance;

	for(i=0;i<5;i++){		
		
		TRIG_SEND = 1;      //发送高电平输出
		delay_us(20);
		TRIG_SEND = 0;
			
		while(ECHO_RECEIVE == 0); 	//等待接收高电平输出
		StartTimer();        	//打开计时器

		while(ECHO_RECEIVE == 1);
		StopTimer();        	//关闭计时器
		
		t = GetEchoTimer();     //获取时间,解析度为1US

		lengthTemp = ((float)t*((331.45+temperature*0.61)-1)/10000/2);	//cm   带温度补偿
		//lengthTemp = ((float)t/58.2);	//cm 不带温度补偿
		sum = lengthTemp + sum ;
	}
	distance = sum/5.0;
	return distance;
}

