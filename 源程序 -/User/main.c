#include "stm32f10x.h"
#include "delay.h"
#include "sys.h"
#include "adc.h"
#include "Key.h"
#include "adc.h"
#include "oled_iic.h"
#include "string.h"
#include "stdio.h"
#include "usart2.h"
#include "timer.h"
#include "DS1302.h"
#include "ina226.h"

#define LED_RUN PCout(13)
char display_data[20];
u8 send_flag=0;

u8 LED_state=0;   //路灯状态

u8 Timed_flag=0;   //是否在定时中的标志位
struct sTime CurTime;   //当前时间

int Light_led=0;  //路灯光照强度变量  用于检测路灯是否正常亮起

int Light=0;  //光照强度变量
u8 mode=0;  //模式
u8 LED_colour=0;   //灯颜色   0=白  1=暖白  2黄  
u8 brightness=0;  //亮度
u8 Led_kz=0;    //亮度控制
u8 ds_dw=0;  //定时挡位
u16 ds_time=0;  //定时时间
u8 ds_kz=0;   //定时控制
u8 Someone_alarm=0;  //人体红外传感器有人标志位


#define Uid  "702b87a3181340b09b56fe4e5040e4c8" 
#define Topicdata "NBLDDATA" 
#define Topickz "NBLDKZ" 


char *strx,*extstrx;
extern char  RxBuffer[100],RxCounter;
unsigned char socketnum=0;//当前的socket号码
void Clear_Buffer(void)//清空缓存
{
		u8 i;
		for(i=0;i<100;i++)
		RxBuffer[i]=0;//缓存
		RxCounter=0;
	
}
void M5310A_Init(void)
{
    printf("AT\r\n"); 
    delay_ms(500);
    strx=strstr((const char*)RxBuffer,(const char*)"OK");//返回OK
    Clear_Buffer();	
    while(strx==NULL)
    {
        Clear_Buffer();	
        printf("AT\r\n"); 
        delay_ms(500);
        strx=strstr((const char*)RxBuffer,(const char*)"OK");//返回OK
    }
    printf("AT+CFUN=1\r\n");//获取卡号，类似是否存在卡的意思，比较重要。
    delay_ms(500);
    printf("AT+CIMI\r\n");//获取卡号，类似是否存在卡的意思，比较重要。
    delay_ms(500);
    strx=strstr((const char*)RxBuffer,(const char*)"460");//返460，表明识别到卡了
    Clear_Buffer();	
    while(strx==NULL)
    {
        Clear_Buffer();	
        printf("AT+CIMI\r\n");//获取卡号，类似是否存在卡的意思，比较重要。
        delay_ms(500);
        strx=strstr((const char*)RxBuffer,(const char*)"460");//返回OK,说明卡是存在的
			
    }
        printf("AT+CGATT=1\r\n");//激活网络，PDP
        delay_ms(500);
        strx=strstr((const char*)RxBuffer,(const char*)"OK");//返OK
        Clear_Buffer();	
        printf("AT+CGATT?\r\n");//查询激活状态
        delay_ms(500);
        strx=strstr((const char*)RxBuffer,(const char*)"+CGATT:1");//返1
        Clear_Buffer();	
				OLED_ShowCH(0,6,"检查M5310A联网..");
				while(strx==NULL)
				{
								Clear_Buffer();	
								printf("AT+CGATT?\r\n");//获取激活状态
								delay_ms(500);
								strx=strstr((const char*)RxBuffer,(const char*)"+CGATT:1");//返回1,表明注网成功
				}
     Clear_Buffer();
		
		OLED_ShowCH(0,6,"M5310A联网OK    ");
}



void network_init()    //网络初始化
{
	printf("AT+NSOCR=\"STREAM\",6,0,2\r\n");   //创建TCP Socket连接，开启自动上报接收数据
	DelayS(2);
	printf("AT+NSOCO=0,bemfa.com ,8344\r\n");   //连接TCP远程服务器
	DelayS(3);
	DelayS(3);
	printf("AT+NSOCFG=0,0,0\r\n");              //配置收发模式
	DelayS(3);
		printf("AT+NSOSD=0,1,\"cmd=1&uid=");    //订阅
		printf(Uid);
		printf("&topic=");
		printf(Topickz); 
		printf("\\r\\n\",,4\r\n");
}
void Post_data()    //发送数据
{
	char postData[200];	
	sprintf(postData,"#%d,%d,%.1f,%d,%d,*",Light,brightness,INA226_data.Total_Power/1000,LED_state,Timed_flag);

		delay_ms(100);
		printf("AT+NSOSD=0,1,\"cmd=2&uid=");    //发布
		printf(Uid);
		printf("&topic=");
		printf(Topicdata); 
		printf("&msg=");
		printf(postData); //发布的消息

		printf("\\r\\n\",,4\r\n");
}



void LED_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //使能端口时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;				 //端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOC, &GPIO_InitStructure);					 //根据设定参数初始化GPIO
 GPIO_SetBits(GPIOC,GPIO_Pin_13);					


}

u8 set_cs=0;   //设置参数变量
u8 set_cs_number=0;
int ks_hour=20,ks_min=0;
int gb_hour=1,gb_min=0;
void display_Set_parameters()
{
	if(set_cs_number<5)
	{
		OLED_ShowCH(8,0,"设置开灯时间段");
		
		OLED_ShowNum_c(20,4,ks_hour,2,1);  
		OLED_ShowCH(36,4,":");
		OLED_ShowNum_c(44,4,ks_min,2,1);   
		OLED_ShowCH(60,4,"-");		
		OLED_ShowNum_c(68,4,gb_hour,2,1);  
		OLED_ShowCH(84,4,":");
		OLED_ShowNum_c(92,4,gb_min,2,1);  
		
		OLED_ShowCH(set_cs_number*24,6,"↑");	
	}
}
void Set_parameters()   //设置参数界面
{
			if(KEY1_add==0)  //加
			{
				if(set_cs_number==1) 
				{
					if(ks_hour<23)
					ks_hour++;
				}
				if(set_cs_number==2) 
				{
					if(ks_hour<59)
					ks_min++;
				}
				if(set_cs_number==3) 
				{
					if(gb_hour<23)
					gb_hour++;
				}
				if(set_cs_number==4) 
				{
					if(gb_min<59)
					gb_min++;
				}
				delay_ms(150);
			}
			
			if(KEY1_del==0)  //减
			{
				if(set_cs_number==1) 
				{
					if(ks_hour>0)
						ks_hour--;
				}
				if(set_cs_number==2) 
				{
					if(ks_min>0)
						ks_min--;
				}
				if(set_cs_number==3) 
				{
					if(gb_hour>0)
						gb_hour--;
				}
				if(set_cs_number==4) 
				{
					if(gb_min>0)
						gb_min--;
				}
				delay_ms(150);
			}
	
	
	if(set_cs==1)
	{
		display_Set_parameters();
		if(KEY_time==0)   //按下确认按键
		{
			set_cs=0;
			set_cs_number=0;
			OLED_Clear();
			OLED_ShowCH(32,2,"设置成功");			
			while(!KEY_time);
			delay_ms(1000);
			OLED_Clear();
		}
	}
	
	if(KEY_set==0)
	{
		delay_ms(5);
		if(KEY_set==0)
		{
			if(set_cs_number==0)
			{
					OLED_Clear();
			}
			set_cs=1;
			set_cs_number++;
			if(set_cs_number>4)
				set_cs_number=1;
			OLED_clear_h(6);
			display_Set_parameters();
			while(!KEY_set);
		}
	}
}

u8 Set_mode=0;  //0为正常 1为设置时间
u8 set_hour=0,set_min=0,set_sec=0;
u8 set_time_number=0; //设置按键次数
//设置时间
void Key_settime()
{
		if(Set_mode==1)   //设置时间
		{
			OLED_ShowNum_c(30,4,set_hour,2,1);  
			OLED_ShowCH(46,4,":");
			OLED_ShowNum_c(54,4,set_min,2,1);   
			OLED_ShowCH(70,4,":");		
			OLED_ShowNum_c(78,4,set_sec,2,1);
			if(KEY1_add==0)  //加
			{
				if(set_time_number==1)  //设置时
				{
					set_hour++;
					if(set_hour>=24)
						set_hour=0;
				}
				if(set_time_number==2)  //设置分
				{
					set_min++;
					if(set_min>=60)
						set_min=0;
				}	
				if(set_time_number==3)  //设置分
				{
					set_sec++;
					if(set_sec>=60)
						set_sec=0;
				}		
				delay_ms(200);		
			//	while(!KEY1_add);				
			}	
			if(KEY1_del==0)  //减
			{
				if(set_time_number==1)  //设置时
				{
					set_hour--;
					if(set_hour==255)
						set_hour=23;
				}
				if(set_time_number==2)  //设置分
				{
					set_min--;
					if(set_min==255)
						set_min=59;
				}	
				if(set_time_number==3)  //设置分
				{
					set_sec--;
					if(set_sec==255)
						set_sec=59;
				}				
				delay_ms(200);
				//while(!KEY1_del);		
			}			
		}
		if(KEY_time==0)   //设置时间
		{
			delay_ms(5);	
			if(KEY_time==0)
			{
				Set_mode=1;
				if(set_time_number==0)
				{
					OLED_Clear();
					OLED_ShowCH(32,0,"校准时间");
					set_hour=BCD_DEC(CurTime.hour);		  //读取当前时间
					set_min=BCD_DEC(CurTime.min);	
					set_sec=BCD_DEC(CurTime.sec);	
				}	
				if(set_time_number<3)
				{
					OLED_clear_h(6);
					OLED_ShowCH(32+set_time_number*22,6,"↑");	
				}
				set_time_number++; 	
				if(set_time_number>=4)
				{
					Set_mode=0;
					set_time_number=0;
					OLED_Clear(); 
					OLED_ShowCH(32,2,"校准成功");
					CurTime.hour=DEC_BCD(set_hour);		  //方向赋值时间
					CurTime.min=DEC_BCD(set_min);	
					CurTime.sec=DEC_BCD(set_sec);				
					SetRealTime(&CurTime);       //把设定时间写入实时时钟
					
					delay_ms(1000);
					OLED_Clear(); 
				}		
				while(!KEY_time);		
			}
		}
}

int main(void)
{
	u8 t;
	u8 send_time=0;
	HZ=GB16_NUM();
	delay_init();	    	 //延时函数初始化	  
	delay_ms(100);
	OLED_Init();    //初始化
	delay_ms(100);
	OLED_Clear();
	delay_ms(100);
	TIM3_PWM_Init(99,7199);	//初始化PWM
	delay_ms(100);	
	//开机设置为关闭状态
	TIM_SetCompare1(TIM3, brightness*20);    //初始化亮度0
	delay_ms(100);	
	OLED_ShowCH(32,2,"初始化中");
	LED_Init();
	delay_ms(500);
	uart1_init(9600);
	delay_ms(50);
	OLED_ShowCH(0,6,"等待M5310A开机..");
	M5310A_Init();    //检查是否连接网络
	OLED_ShowCH(0,6,"连接服务器中....");
	network_init();   //连接服务器
	delay_ms(500);
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	Key_Init();
	delay_ms(100);
	Adc_Init();		  		//ADC初始化
	Init_DS1302(); 
	INA226_Init();    //初始化ina226模块
	OLED_Clear();
	while(1)
	{
		if(Set_mode==0&&set_cs==0)
		{
		GetRealTime(&CurTime); //获取当前日期/时间
			
			if(t!=CurTime.sec)   //每秒发送数据
			{
				LED_RUN=!LED_RUN;
				t=CurTime.sec;			
				GetPower();   //读取ina226模块数据
				Light=100-Lsens_Get_Val();   //ADC读取环境光照强度
				Light_led=100-Lsens_Get_Val2();   //ADC读取路灯下光照强度
				
				if(brightness>0)   //路灯开启时
				{
					if(Light_led>85)   //路灯开启情况下  光照强度要大于90  才表明路灯真正亮起
					{
						LED_state=0;   //状态正常
					}
					else
					{
						LED_state=1;   //状态异常
					}
				}
				else   //路灯关闭时
				{
					if(Light_led<85)   //路灯关闭情况下  光照强度要小于90  才表明路灯真正关闭
					{
						LED_state=0;   //状态正常
					}
					else
					{
						LED_state=1;   //状态异常
					}
				}
				if(LED_state==0)
					OLED_ShowCH(56,6,"状态:正常");
				else
					OLED_ShowCH(56,6,"状态:故障");
				
				sprintf(display_data,"%.1f W ",INA226_data.Total_Power/1000);
				OLED_ShowCH(0,6,(u8 *)display_data);
				
				OLED_ShowNum_c(28,0,BCD_DEC(CurTime.hour),2,1);  
				OLED_ShowCH(44,0,":");
				OLED_ShowNum_c(52,0,BCD_DEC(CurTime.min),2,1);   
				OLED_ShowCH(68,0,":");		
				OLED_ShowNum_c(76,0,BCD_DEC(CurTime.sec),2,1);  
				
				OLED_ShowCH(0,2,"环境光照:");
				OLED_ShowNum(72,2,Light,2,1); 
				if(Light<10)
				OLED_ShowCH(80,2,"%  ");
				else if(Light>=10&&Light<100)
				OLED_ShowCH(88,2,"% ");
				else if(Light>=100&&Light<1000)
				OLED_ShowCH(96,2,"%");
			
			
				OLED_ShowCH(0,4,"亮度档位:");			
				if(brightness==0)
				{
					OLED_ShowCH(72,4,"关闭");
				}
				if(brightness==1)
				{
					OLED_ShowCH(72,4,"一档");
				}
				if(brightness==2)
				{
					OLED_ShowCH(72,4,"二档");
				}
				if(brightness==3)
				{
					OLED_ShowCH(72,4,"三档");
				}
				
				
				if((gb_hour*60+gb_min)>(ks_hour*60+ks_min))
				{	
					if((BCD_DEC(CurTime.hour)*60+BCD_DEC(CurTime.min))>=(ks_hour*60+ks_min)&&
						(BCD_DEC(CurTime.hour)*60+BCD_DEC(CurTime.min))<(gb_hour*60+gb_min))   //在设定时间区间内
					{
							 Timed_flag=1;//开启
					}
					else   //不在设定时间内关闭
					{
						   Timed_flag=0;//关闭
					}
				}
				
				if((gb_hour*60+gb_min)<(ks_hour*60+ks_min))
				{	
					if((BCD_DEC(CurTime.hour)*60+BCD_DEC(CurTime.min))>=(ks_hour*60+ks_min)||
						(BCD_DEC(CurTime.hour)*60+BCD_DEC(CurTime.min))<=(gb_hour*60+gb_min))   //在设定时间区间内
					{
							Timed_flag=1;//开启
					}
					else   //不在设定时间内关闭
					{
						   Timed_flag=0;//关闭
					}
				}
				
				
					if(Timed_flag==0)   //不在设定时间端  
					{
						if(HC_SR501==1)   //有人的情况下  才根据光照开灯
						{
							if(Light>85)   //亮度充足
							{
								 //关灯
								brightness=0;
								TIM_SetCompare1(TIM3, brightness*20);    //设置亮度
							}
							else if(Light<=80&&Light>60)   //一档亮度
							{
								brightness=1;
								TIM_SetCompare1(TIM3, brightness*20);    //设置亮度
								
							}
							else if(Light<=60&&Light>30)   //2档亮度
							{
								brightness=2;
								TIM_SetCompare1(TIM3, brightness*20);    //设置亮度
								
							}
							else if(Light<=30&&Light>=0)   //3档亮度
							{
								brightness=3;
								TIM_SetCompare1(TIM3, brightness*20);    //设置亮度
								
							}
						}
						else
						{
							//关灯
							brightness=0;
							TIM_SetCompare1(TIM3, brightness*20);    //设置亮度
						}
					}
					else    //在设定时间段内
					{
							if(Light>60)   //一档亮度
							{
								brightness=1;
								TIM_SetCompare1(TIM3, brightness*20);    //设置亮度
								
							}
							else if(Light<=60&&Light>30)   //2档亮度
							{
								brightness=2;
								TIM_SetCompare1(TIM3, brightness*20);    //设置亮度
							}
							else if(Light<=30&&Light>=0)   //3档亮度
							{
								brightness=3;	
								TIM_SetCompare1(TIM3, brightness*20);    //设置亮度							
							}
					}
				
				
				send_time++;
				if(send_time>=3)    //3S上报一次数据
				{
					Post_data();    //发送数据   通过m5310发送至APP
					send_time=0;
				}
				
			}
				
		}
		if(set_cs==0)
			Key_settime();      //校准时间
		if(Set_mode==0)
			Set_parameters();   //设置参数界面
	}
}
