#include "INA226.h"

INA226 INA226_data;

/*
	Configuration Register - 0x00
	D15 - D12  -> 0 1 0 0
	D11 - D9		AVG2	AVG1	AVG0		AVG_RES
							0			0			0				1
							0			0			1				4
							0			1			0				16
							0			1			1				64
							1			0			0				128
							0			0			1				256
							1			1			0				512
							1			1			1				1024
	D8 - D6			VBUS2	VBUS1	VBUS0		Bus Voltage	-> us
							0			0			0				140
							0			0			1				204
							0			1			0				332
							0			1			1				588
							1			0			0				1100
							0			0			1				2116
							1			1			0				4156
							1			1			1				8244
	D5 - D3			VSH2	VSH1	VSH0		Shunt Voltage	-> us
							0			0			0				140
							0			0			1				204
							0			1			0				332
							0			1			1				588
							1			0			0				1100
							0			0			1				2116
							1			1			0				4156
							1			1			1				8244
	D2 - D0			MODE3	MODE2	MODE1		Operating Mode
							0			0			0				Power-Down (or Shutdown)
							0			0			1				Shunt Voltage, Triggered
							0			1			0				Bus Voltage, Triggered
							0			1			1				Shunt and Bus, Triggered
							1			0			0				Power-Down (or Shutdown)
							0			0			1				Shunt Voltage, Continuous
							1			1			0				Bus Voltage, Continuous
							1			1			1				Shunt and Bus, Continuous
							
*/

void INA226_Init(void)
{	
	IIC_Init_226();
	delay_ms(10);
	INA226_SendData(INA226_ADDR1,CFG_REG,0x4607);//设置转换时间140us,求平均值次数64，设置模式为分流和总线连续模式
	INA226_SendData(INA226_ADDR1,CAL_REG,0x0800);//设置分流电压转电流转换参数	电阻0.0025R，分辨率1mA
}
void INA226_SetRegPointer(u8 addr,u8 reg)
{
	IIC_Start_226();
 
	IIC_Send_Byte_226(addr);
	IIC_Wait_Ack_226();
 
	IIC_Send_Byte_226(reg);
	IIC_Wait_Ack_226();
 
	IIC_Stop_226();
}
 
//发送,写入
void INA226_SendData(u8 addr,u8 reg,u16 data)
{
	u8 temp=0;
	IIC_Start_226();
 
	IIC_Send_Byte_226(addr);
	IIC_Wait_Ack_226();
 
	IIC_Send_Byte_226(reg);
	IIC_Wait_Ack_226();
	
	temp = (u8)(data>>8);
	IIC_Send_Byte_226(temp);
	IIC_Wait_Ack_226();
 
	temp = (u8)(data&0x00FF);
	IIC_Send_Byte_226(temp);
	IIC_Wait_Ack_226();
	
	IIC_Stop_226();
}
 
//读取
u16 INA226_ReadData(u8 addr)
{
	u16 temp=0;
	IIC_Start_226();
 
	IIC_Send_Byte_226(addr+1);
	IIC_Wait_Ack_226();
	
	temp = IIC_Read_Byte_226(1);
	temp<<=8;	
	temp |= IIC_Read_Byte_226(0);
	
	IIC_Stop_226();
	return temp;
}
/*
u8 INA226_AlertAddr()
{
	u8 temp;
	IIC_Start();
	IIC_Send_Byte(INA226_GETALADDR);
	IIC_Wait_Ack();
	
	temp = IIC_Read_Byte(1);
	
	IIC_Stop();
	return temp;
}
*/
 
//1mA/bit
u16 INA226_GetShunt_Current(u8 addr)
{
	u16 temp=0;	
	INA226_SetRegPointer(addr,CUR_REG);
	temp = INA226_ReadData(addr);
	if(temp&0x8000)	temp = ~(temp - 1);	
	return temp;
}
 
//获取 id
u16  INA226_Get_ID(u8 addr)
{
	u32 temp=0;
	INA226_SetRegPointer(addr,INA226_GET_ADDR);
	temp = INA226_ReadData(addr);
	return (u16)temp;
}
 
//获取校准值
u16 INA226_GET_CAL_REG(u8 addr)
{	
	u32 temp=0;
	INA226_SetRegPointer(addr,CAL_REG);
	temp = INA226_ReadData(addr);
	return (u16)temp;
}
 
//1.25mV/bit
u16 INA226_GetVoltage(u8 addr)
{
	u32 temp=0;
	INA226_SetRegPointer(addr,BV_REG);
	temp = INA226_ReadData(addr);
	return (u16)temp;	
}

u16 INA226_GetShuntVoltage(u8 addr)
{
	int16_t temp=0;
	INA226_SetRegPointer(addr,SV_REG);
	temp = INA226_ReadData(addr);
	if(temp&0x8000)	temp = ~(temp - 1);	
	return (u16)temp;	
}
 
 
u16 INA226_Get_Power(u8 addr)
{
	int16_t temp=0;
	INA226_SetRegPointer(addr,PWR_REG);
	temp = INA226_ReadData(addr);
	return (u16)temp;
}

void GetVoltage(float *Voltage)//mV
{
	Voltage[0] = INA226_GetVoltage(INA226_ADDR1)*1.23f*1.01369f;
	if(Voltage[0] <= 50)	Voltage[0] = 0;
}
 
 
void Get_Shunt_voltage(float *Voltage)//uV
{
	Voltage[0] = (INA226_GetShuntVoltage(INA226_ADDR1)*2.5f);
}
 
 
void Get_Shunt_Current(float *Current)//mA
{
	Current[0] = (INA226_GetShunt_Current(INA226_ADDR1)* 1.0f)/4.318f;
	if(Current[0] <= 10)	Current[0] = 0;
}

void GetPower(void)//W
{
	GetVoltage(&INA226_data.Bus_Voltage);			//mV
	Get_Shunt_voltage(&INA226_data.Shunt_Voltage);	//uV
	Get_Shunt_Current(&INA226_data.Shunt_Current);	//mA
	INA226_data.Total_Power=INA226_data.Bus_Voltage*1.000f * INA226_data.Shunt_Current*0.001f;
	INA226_data.Equivalent_Resistance = (INA226_data.Bus_Voltage*1000)/INA226_data.Shunt_Current*1.00f;
}
 










