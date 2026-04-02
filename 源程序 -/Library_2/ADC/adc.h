#ifndef __ADC_H
#define __ADC_H	
#include "sys.h"

#define LSENS_READ_TIMES	10		//定义光敏传感器读取次数,读这么多次,然后取平均值
#define LSENS_ADC_CHX		ADC_Channel_0	//定义光敏传感器所在的ADC通道编号
#define LSENS_ADC_CHX2		ADC_Channel_1	//定义光敏传感器所在的ADC通道编号
void Adc_Init(void);
u16  Get_Adc(u8 ch); 
u16 Get_Adc_Average(u8 ch,u8 times); 

u8 Lsens_Get_Val(void);				//读取光敏传感器1的值
u8 Lsens_Get_Val2(void);				//读取光敏传感器2的值 
#endif 
