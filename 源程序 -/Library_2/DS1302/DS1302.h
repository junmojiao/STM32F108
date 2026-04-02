#ifndef _DS1302_H
#define _DS1302_H


#include "sys.h"

#define DS1302_IO_IN()  {GPIOB->CRL&=0XFFFFFFF0;GPIOB->CRL|=0x08;}//上拉/下拉输入模式
#define DS1302_IO_OUT() {GPIOB->CRL&=0XFFFFFFF0;GPIOB->CRL|=0x03;}//推挽输出模式，50MHZ

struct sTime {  //日期时间结构
    uint16_t year; //年
    uint8_t mon;   //月
    uint8_t day;   //日
    uint8_t hour;  //时
    uint8_t min;   //分
    uint8_t sec;   //秒
    uint8_t week;  //星期
};

#define	 	TSCLK  PBout(10) //数据端口	
#define	  TIO   GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0)
#define	  TRST  PBout(1)  //数据端口



void Init_DS1302(void); //初始化DS1302，并设置默认时间
void GetRealTime(struct sTime *time);  //获取DS1302时钟日历数据
void SetRealTime(struct sTime *time);  //设置DS1302时钟日历数据
uint8_t BCD_DEC(uint8_t BCD);
uint8_t DEC_BCD(uint8_t DEC);

#endif

