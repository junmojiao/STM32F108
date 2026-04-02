
#include "DS1302.h"
#include "delay.h"

        

/*=================================================
*函数名称：DS1302_W_Byte
*函数功能：DS1302写一字节数据
*输入：dat：要写入的数据
=================================================*/
void DS1302_W_Byte(u8 dat)
{
	u8 i,temp;
	 
	TSCLK = 0;
	delay_us(20);
	DS1302_IO_OUT();

	for(i = 0; i < 8; i++) //每次写1bit，写8次
	{
		
		temp= dat & 0x01;  //从一字节最低位开始写
		if(temp)
		GPIO_SetBits(GPIOB,GPIO_Pin_0);
			else
		GPIO_ResetBits(GPIOB,GPIO_Pin_0);
		dat >>= 1;		   //数据右移一位
			TSCLK = 1;		   //拉高时钟总线，DS1302把数据读走
		 	delay_us(20);
		TSCLK = 0;
	  delay_us(20);	
	}	 
}
/*=================================================
*函数名称：DS1302_R_Byte
*函数功能：DS1302读一字节
*输出：dat：读取的数据
=================================================*/
u8 DS1302_R_Byte()
{
	u8 i, dat;
	TSCLK = 0;			//拉低时钟总线，DS1302把数据放到数据总线上
	delay_us(20);
	DS1302_IO_IN();
	for(i = 0; i < 8; i++)  //每次写1bit，写8次
	{
		
		dat >>= 1; 			//数据右移一位，数据从最低位开始读
				if(TIO ==1)
		{
			dat|=0x80;
		}	
		TSCLK = 1;			//拉高时钟总线
		delay_us(20);
		TSCLK = 0;
		delay_us(20);
	}
	return dat;				//返回读取的数据
}
/*=================================================
*函数名称：DS1302_W_DAT
*函数功能：写DS1302数据一次写2个字节
*说明：先写命令后写数据
*调用：DS1302_W_Byte()
*输入：cmd：需要写的命令 ，dat：需要些的数据
=================================================*/
void DS1302_W_DAT(u8 cmd, u8 dat)
{
	TRST = 0;			 //
	delay_us(20);
	TRST = 1;			 //TRST上升沿开始读写数据
	delay_us(20);
	DS1302_W_Byte(cmd);	 //写命令
	DS1302_W_Byte(dat);	 //写数据
	TRST = 0;			 //禁止读写数据
	delay_us(20);
}
/*=================================================
*函数名称：DS1302_R_DAT
*函数功能：读DS1302数据
*说明：先写入命令字节后读出对应数据
*调用：	DS1302_W_Byte();DS1302_R_Byte();
*输入：	cmd：需要写的命令
*输出：	dat：读出的数据
=================================================*/
u8 DS1302_R_DAT(u8 cmd)
{
	u8 dat;
	TRST = 0;			 	//
	delay_us(20);
	TRST = 1;				//TRST上升沿开始读写数据
	delay_us(20);
	DS1302_W_Byte(cmd);		//写命令
	dat = DS1302_R_Byte();	//读出数据
	TRST = 0;			 	//禁止读写数据
	delay_us(20);
	GPIO_SetBits(GPIOB,GPIO_Pin_0);
	delay_us(20);
	GPIO_SetBits(GPIOB,GPIO_Pin_0);
	delay_us(20);
	return dat;				//返回读出数据
}  
/*=================================================
*函数名称：DS1302_Clear_WP
*函数功能：清除DS1302写保护
*说明：先写入命令0x8e（写控制寄存器）接着向该寄存器写0
*调用：DS1302_W_DAT()
=================================================*/
void DS1302_Clear_WP()
{
	DS1302_W_DAT(0x8e,0x00);  //把控制寄存器WP位置0
}
/*=================================================
*函数名称：DS1302_Set_WP
*函数功能：设置DS1302写保护
*说明：先写入命令0x8e（写控制寄存器）接着向该寄存器写0x80
*调用：DS1302_W_DAT()
=================================================*/
void DS1302_Set_WP()
{	
	DS1302_W_DAT(0x8e,0x80); //把控制寄存器WP位置1
	TRST = 0;				 //拉低使能端
	TSCLK = 0;				 //拉低数据总线
} 
/*=================================================
*函数名称：DS1302_Burst_Write
*函数功能：突发模式写DS1302时钟日历数据
*说明：写入8个字节的数据到DS1302时钟日历寄存器中
*调用：DS1302_Clear_WP();DS1302_W_Byte();DS1302_Set_WP();
=================================================*/
void DS1302_Burst_Write(u8 *dat)
{
	u8 i;
	DS1302_Clear_WP();		//清除写保护
	TRST = 0;			 	//拉低使能端
	TSCLK = 0;				//拉低数据总线
	TRST = 1;				//拉高使能端，开始写数据
	DS1302_W_Byte(0xbe); //写时钟突发模式寄存器
	for(i = 0; i < 8; i++)	//写入8个字节的时钟初始值
	{
		DS1302_W_Byte(dat[i]); //在突发模式下可以连续写数据
	}
	DS1302_Set_WP(); //开起写保护		
}
/*=================================================
*函数名称：DS1302_Burst_Read
*函数功能：突发模式读DS1302时钟日历数据
*说明：	读取8个字节的DS1302时钟日历数据
		数组TimeData（数据格式BCD码）
*调用：DS1302_Clear_WP();DS1302_R_Byte();DS1302_Set_WP();
=================================================*/
void DS1302_Burst_Read(u8 *dat)
{
	u8 i;
	DS1302_Clear_WP();    	//清除写保护
	TRST = 0;			 	//拉低使能端
	TSCLK = 0;				//拉低数据总线
	TRST = 1;				//拉高使能端，开始写数据
	DS1302_W_Byte(0xbf); //写 读突发模式寄存器命令
	for(i = 0; i < 8; i++)	//从DS1302读取7个字节的时钟日历数据
	{
		dat[i] = DS1302_R_Byte();//在突发模式下可以连续读数据
	}
	DS1302_Set_WP();   //开起写保护
}
/*=================================================
*函数名称：GetRealTime
*函数功能：突发模式读DS1302时钟日历数据，并转为时间格式
=================================================*/
void GetRealTime(struct sTime *time)
{
    u8 buf[8];
    
    DS1302_Burst_Read(buf);
    time->year = buf[6] + 0x2000;  //年，因为DS1302只能是0-99如显示2012年则通过2000这种方法显示年的千位和百位
    time->mon  = buf[4];
    time->day  = buf[3];
    time->hour = buf[2];
    time->min  = buf[1];
    time->sec  = buf[0];
    time->week = buf[5];
}
/*=================================================
*函数名称：SetRealTime
*函数功能：突发模式设置DS1302时钟日历数据
=================================================*/ 
void SetRealTime(struct sTime *time)
{
    u8 buf[8];
    
    buf[7] = 0;
    buf[6] = time->year;
    buf[5] = time->week;
    buf[4] = time->mon;
    buf[3] = time->day;
    buf[2] = time->hour;
    buf[1] = time->min;
    buf[0] = time->sec;
    DS1302_Burst_Write(buf);
}
/*=================================================
*函数名称：Init_DS1302
*函数功能：初始化DS1302并设置默认时间 ：2017-08-01 23:59:50 星期二
=================================================*/ 
void Init_DS1302()
{    u8 flag;
	
	
		GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

	//GPIO_SetBits(GPIOA,GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);
	
   	
	flag=DS1302_R_DAT(0x81);    //取秒寄存器数值，若为0则1302处于工作状态，若为1则1302处于非工作状态
	if(flag&0x80) 
		{    

 struct  sTime    InitTime[] = {0x2021 , 0x08, 0x08, 0x22, 0x57, 0x50, 0x07 };
        
    	
	
    SetRealTime( (void*)&InitTime);      //设置DS1302为默认的初始时间
  } 
}


/*BCD码转十进制*/
uint8_t BCD_DEC(uint8_t BCD)
{
	uint8_t DEC;
	DEC= BCD / 16;
	BCD %= 16;
	BCD += DEC * 10;

	return BCD;
}
/*十进制转BCD*/
uint8_t DEC_BCD(uint8_t DEC)
{
	uint8_t BCD;
	BCD = DEC / 10;
	DEC %= 10;
	DEC += BCD * 16;

	return DEC;
}


