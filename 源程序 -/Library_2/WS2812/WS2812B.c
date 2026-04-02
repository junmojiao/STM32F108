#include "WS2812B.h"


#define TIM2_CCR1_Address 0x40000034
#define TIMING_ONE  50
#define TIMING_ZERO 25
uint16_t LED_BYTE_Buffer[300];



void Timer2_init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  DMA_InitTypeDef DMA_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	/* GPIOA Configuration: TIM2 Channel 1 as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	/* Compute the prescaler value */
	//PrescalerValue = (uint16_t) (SystemCoreClock / 24000000) - 1;
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 90-1; // 800kHz 
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	/* PWM1 Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);
		
	/* configure DMA */
	/* DMA clock enable */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	/* DMA1 Channel6 Config */
	DMA_DeInit(DMA1_Channel2);

	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)TIM2_CCR1_Address;	// physical address of Timer 3 CCR1
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)LED_BYTE_Buffer;		// this is the buffer memory 
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;						// data shifted from memory to peripheral
	DMA_InitStructure.DMA_BufferSize = 42;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;					// automatically increase buffer index
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;							// stop DMA feed after buffer size is reached
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	
	DMA_Init(DMA1_Channel2, &DMA_InitStructure);

		/* TIM3 CC1 DMA Request enable */
	TIM_DMACmd(TIM2, TIM_DMA_Update, ENABLE);
}





void WS2812_send_single(uint8_t R,uint8_t G,uint8_t B,uint16_t len)
{
	uint8_t i;
	uint16_t memaddr;
	uint16_t buffersize;
	buffersize = (len*24)+43;	// number of bytes needed is #LEDs * 24 bytes + 42 trailing bytes
	memaddr = 0;				// reset buffer memory index

	while (len)
	{	
		if(len==1)
		{
			for(i=0; i<8; i++) // GREEN data
			{
				LED_BYTE_Buffer[memaddr] = ((G<<i) & 0x0080) ? TIMING_ONE:TIMING_ZERO;
				memaddr++;
			}
			for(i=0; i<8; i++) // RED
			{
					LED_BYTE_Buffer[memaddr] = ((R<<i) & 0x0080) ? TIMING_ONE:TIMING_ZERO;
					memaddr++;
			}
			for(i=0; i<8; i++) // BLUE
			{
				LED_BYTE_Buffer[memaddr] = ((B<<i) & 0x0080) ? TIMING_ONE:TIMING_ZERO;
				memaddr++;
			}
			len--;
		}
		
		else
		{
			for(i=0; i<8; i++) // GREEN data
			{
				LED_BYTE_Buffer[memaddr] = ((0<<i) & 0x0080) ? TIMING_ONE:TIMING_ZERO;
				memaddr++;
			}
			for(i=0; i<8; i++) // RED
			{
				LED_BYTE_Buffer[memaddr] = ((0<<i) & 0x0080) ? TIMING_ONE:TIMING_ZERO;
				memaddr++;
			}
			for(i=0; i<8; i++) // BLUE
			{
				LED_BYTE_Buffer[memaddr] = ((0<<i) & 0x0080) ? TIMING_ONE:TIMING_ZERO;
				memaddr++;
			}
			len--;
		}
		
		
	}

  	LED_BYTE_Buffer[memaddr] = (B & 0x0080) ? TIMING_ONE:TIMING_ZERO;
	  memaddr++;	
		while(memaddr < buffersize)
		{
			LED_BYTE_Buffer[memaddr] = 0;
			memaddr++;
		}

		DMA_SetCurrDataCounter(DMA1_Channel2, buffersize); 	// load number of bytes to be transferred
		DMA_Cmd(DMA1_Channel2, ENABLE); 			// enable DMA channel 6
		TIM_Cmd(TIM2, ENABLE); 						// enable Timer 3
		while(!DMA_GetFlagStatus(DMA1_FLAG_TC2)) ; 	// wait until transfer complete
		TIM_Cmd(TIM2, DISABLE); 	// disable Timer 3
		DMA_Cmd(DMA1_Channel2, DISABLE); 			// disable DMA channel 6
		DMA_ClearFlag(DMA1_FLAG_TC2); 				// clear DMA1 Channel 6 transfer complete flag
}





void WS2812_send_all(uint8_t R,uint8_t G,uint8_t B,uint16_t len)
{
	uint8_t i;
	uint16_t memaddr;
	uint16_t buffersize;
	buffersize = (len*24)+43;	// number of bytes needed is #LEDs * 24 bytes + 42 trailing bytes
	memaddr = 0;				// reset buffer memory index

	while (len)
	{	
		for(i=0; i<8; i++) // GREEN data
		{
			LED_BYTE_Buffer[memaddr] = ((G<<i) & 0x0080) ? TIMING_ONE:TIMING_ZERO;
			memaddr++;
		}
		for(i=0; i<8; i++) // RED
		{
				LED_BYTE_Buffer[memaddr] = ((R<<i) & 0x0080) ? TIMING_ONE:TIMING_ZERO;
				memaddr++;
		}
		for(i=0; i<8; i++) // BLUE
		{
			LED_BYTE_Buffer[memaddr] = ((B<<i) & 0x0080) ? TIMING_ONE:TIMING_ZERO;
			memaddr++;
		}
		len--;
	}

  	LED_BYTE_Buffer[memaddr] = (B & 0x0080) ? TIMING_ONE:TIMING_ZERO;
	  memaddr++;	
		while(memaddr < buffersize)
		{
			LED_BYTE_Buffer[memaddr] = 0;
			memaddr++;
		}

		DMA_SetCurrDataCounter(DMA1_Channel2, buffersize); 	// load number of bytes to be transferred
		DMA_Cmd(DMA1_Channel2, ENABLE); 			// enable DMA channel 6
		TIM_Cmd(TIM2, ENABLE); 						// enable Timer 3
		while(!DMA_GetFlagStatus(DMA1_FLAG_TC2)) ; 	// wait until transfer complete
		TIM_Cmd(TIM2, DISABLE); 	// disable Timer 3
		DMA_Cmd(DMA1_Channel2, DISABLE); 			// disable DMA channel 6
		DMA_ClearFlag(DMA1_FLAG_TC2); 				// clear DMA1 Channel 6 transfer complete flag
}
