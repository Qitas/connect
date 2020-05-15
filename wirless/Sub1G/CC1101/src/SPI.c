

/*******************************************************************************
* Name  :  Qitas soft for CC1101
*******************************************************************************/
#include "stm32f10x_spi.h"
#include  "SPI.h"

static u8 SPI_MODE=0; 
u8  GDO_Status=0;
uint8_t SPI2_Rx_Buf[4096];
uint8_t SPI2_Tx_Buf[4096];
#define SCLK_LOW()   			GPIO_ResetBits(SPI_PORT, SPI_SCK)
#define SCLK_HIGH()  			GPIO_SetBits(SPI_PORT, SPI_SCK)
#define SDI_IN()       		GPIO_ReadInputDataBit(SPI_PORT,SPI_MISO)
#define SDI_LOW_WAIT()   	while(1==SDI_IN()) 
#define SDI_HIGH_WAIT()  	while(0==SDI_IN())  
#define SDO_LOW()   			GPIO_ResetBits(SPI_PORT, SPI_MOSI)
#define SDO_HIGH()  			GPIO_SetBits(SPI_PORT, SPI_MOSI)
/*******************************************************************************
* Function Name  : RF_HalInit
*******************************************************************************/
void halSpi_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;	
	  SPI_InitTypeDef SPI_InitStructure;
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);//

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = SPI_CS;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPI_PORT, &GPIO_InitStructure);
	
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
		SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
		SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
		SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low; 
		SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge; 
		SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
		SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
		SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
		SPI_InitStructure.SPI_CRCPolynomial = 7;
		SPI_Init(SPI2, &SPI_InitStructure);
		SPI_Cmd(SPI2, ENABLE); 
    SPI_MODE=1; 		
}
/*******************************************************************************
* Function Name  : RF_HalInit
*******************************************************************************/
void SpiDMA_Init(void)
{
	      DMA_InitTypeDef  DMA_InitStructure;
        NVIC_InitTypeDef NVIC_InitStructure;
				DMA_DeInit(DMA1_Channel4);
        DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&SPI2->DR);
        DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)SPI2_Rx_Buf;
        DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
        DMA_InitStructure.DMA_BufferSize = 0;
        DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
        DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
        DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
        DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
        DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
        DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
        DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
        DMA_Init(DMA1_Channel4, &DMA_InitStructure);

        if(DMA_GetITStatus(DMA1_IT_TC4)!= RESET)
				{
            DMA_ClearITPendingBit(DMA1_IT_TC4);
        }
        DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);
        SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Rx, ENABLE);
        /* Enable DMA1 Channel4 SPI2_RX */
        DMA_Cmd(DMA1_Channel4, DISABLE);
        /* DMA1 Channel5 Configures for SPI2 Send */
        DMA_DeInit(DMA1_Channel5);
        DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&SPI2->DR);
        DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)SPI2_Tx_Buf;
        DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
        DMA_InitStructure.DMA_BufferSize = 0;
        DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
        DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
        DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
        DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
        DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
        DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
        DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
        DMA_Init(DMA1_Channel5, &DMA_InitStructure);

        if(DMA_GetITStatus(DMA1_IT_TC5)!= RESET)
				{
             DMA_ClearITPendingBit(DMA1_IT_TC5);
        }
        DMA_ITConfig(DMA1_Channel5, DMA_IT_TC, ENABLE);
        SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);

        /* Enable DMA1 Channel5 SPI1_TX */
        DMA_Cmd(DMA1_Channel5, DISABLE);

        /* Enable the DMA1_Channel4?DMA1_Channel5 Interrupt */
        NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);

        NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;
        NVIC_Init(&NVIC_InitStructure);
}
/*******************************************************************************
* Function Name  : RF_HalInit
*******************************************************************************/
void halRf_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); 
    // CSn + SCLK + MOSI
    GPIO_InitStructure.GPIO_Pin   = SPI_CS | SPI_SCK | SPI_MOSI;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPI_PORT, &GPIO_InitStructure);
    // MISO
    GPIO_InitStructure.GPIO_Pin   = SPI_MISO;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPI_PORT, &GPIO_InitStructure);
	  SPI_MODE=0; 	
}
/*******************************************************************************
* Function Name  :
*******************************************************************************/
u8 GDO0_IRQ(void) 
{	
	  GPIO_InitTypeDef GPIO_InitStructure;	
		EXTI_InitTypeDef EXTI_InitStructure;   
    NVIC_InitTypeDef NVIC_InitStructure; 	
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	  GPIO_InitStructure.GPIO_Pin   = GDO0_Pin;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GDO0_PORT, &GPIO_InitStructure);
	
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource10);
    EXTI_ClearITPendingBit(EXTI_Line10);
    EXTI_InitStructure.EXTI_Line = EXTI_Line10;    
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);  	
	
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);  													
		NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;	 
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	  
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure); 
		GDO_Status|=1;
}
/*******************************************************************************
* Function Name  :
*******************************************************************************/
u8 GDO1_IRQ(void) 
{	
	  GPIO_InitTypeDef GPIO_InitStructure;	
		EXTI_InitTypeDef EXTI_InitStructure;   
    NVIC_InitTypeDef NVIC_InitStructure; 	
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	  GPIO_InitStructure.GPIO_Pin   = GDO1_Pin;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GDO1_PORT, &GPIO_InitStructure);
	
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource14);
    EXTI_ClearITPendingBit(EXTI_Line14);
    EXTI_InitStructure.EXTI_Line = EXTI_Line14;    
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);  	
	
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);  													
		NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;	 
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	  
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure); 
		GDO_Status|=2;
}
/*******************************************************************************
* Function Name  :
*******************************************************************************/
u8 GDO2_IRQ(void) 
{	
		GPIO_InitTypeDef GPIO_InitStructure;	
		EXTI_InitTypeDef EXTI_InitStructure;   
    NVIC_InitTypeDef NVIC_InitStructure; 	
	
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		GPIO_InitStructure.GPIO_Pin   = GDO2_Pin;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GDO2_PORT, &GPIO_InitStructure);
	
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource11);	
    EXTI_ClearITPendingBit(EXTI_Line11);
    EXTI_InitStructure.EXTI_Line = EXTI_Line11;    
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);  	
	
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);  													
		NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;	 
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	  
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure); 
		GDO_Status|=4;
}


/*******************************************************************************
* Function Name  : halSpiByteWrite
* Description    : send 8 bit data to SDO line
*******************************************************************************/
void halSpiByteWrite(u8 cData)
{
	    u8 i=250;
      if(SPI_MODE)
			{
					while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET && i--) Delay_us(1);
//					Delay_us(2);
				  SPI2->DR = cData;
					//SPI_I2S_SendData(SPI2, cData);
					Delay_us(4);
			}
			else
			{
				for(i=0; i<8; i++)
				{ 
						 SCLK_LOW();  
						 if((cData&0x80)==0x80)SDO_HIGH(); 
						 else SDO_LOW(); 
						 cData<<=1;
						 Delay_us(3); 
						 SCLK_HIGH();    
						 Delay_us(2);
				}
				SCLK_LOW();     
			}		
}

/*******************************************************************************
* Function Name  : halSpiByteRead
* Description    : get 8 bit data from SDI line
*******************************************************************************/
u8 halSpiByteRead(void)
{
    u8 result,i=200;
	  if(SPI_MODE)
		{
				while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET && i--)Delay_us(1);
			  SPI2->DR = 0xff;
//				SPI_I2S_SendData(SPI2, 0xff);(SPIx->SR & 0x0001)
				while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET&& i--) Delay_us(1);
				return SPI2->DR;  //SPI_I2S_ReceiveData(SPI2); 
		}
		else
		{
			  result = 0;
				SCLK_LOW();  			   
				Delay_us(2);
				for(i=0; i<8; i++)
				{
						SCLK_HIGH();
						result<<=1;
						if(SDI_IN())result |= 0x01; 
						Delay_us(2);
						SCLK_LOW(); 
						Delay_us(2);
				}
				return result;
	  }
   
}
/*******************************************************************************
* Function Name  : halSpiByteRead
* Description    : get 8 bit data from SDI line
*******************************************************************************/
u8 halRfByteRead(void)
{
    u8 i, result;
    result = 0;
    SCLK_LOW();  
    Delay_us(3);
    for(i=0; i<8; i++)
    {
        SCLK_HIGH();
        result<<=1;
        if(SDI_IN())
        {
            result |= 0x01; 
        }
        Delay_us(2);
        SCLK_LOW(); 
        Delay_us(2);
    }
    return result;
}
/*******************************************************************************
* Function Name  : halRfByteWrite
* Description    : send 8 bit data to SDO line
*******************************************************************************/
void halRfByteWrite(u8 cData)
{
    u8 i;
    for(i=0; i<8; i++)
    { 
         SCLK_LOW();  
         if((cData&0x80)==0x80)
         {
             SDO_HIGH();    
         }
         else
         {
             SDO_LOW();  
         }
         cData<<=1;
         Delay_us(3); 
         SCLK_HIGH();    
         Delay_us(2);
    }
    SCLK_LOW();        
}