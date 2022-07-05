#include "cms32f033x.h"

#include "define.h"


u8	F_transmit_0 = 0;							//UART0发送标志位

/****************************************************************************/
/*	Local variable  definitions('static')
*****************************************************************************/
void UART_UART0_Config(void)
{
	uint32_t  BuadRate =9600;
	/*
	(1)设置UARTx模式
	*/
	UART_ConfigRunMode(UART0, BuadRate, UART_WLS_8, UART_PARITY_NONE,UART_STOP_BIT_1);
	/*
	(2)开启UARTx时钟
	*/	
	SYS_EnablePeripheralClk(SYS_CLK_UART0_MSK);
	UART_DisableAuto(UART0);
	UART_EnableRBRInt(UART0);
	UART_EnableTHREInt(UART0);

	/*
	(3)开启UARTx输出
	*/	
	SYS_SET_IOCFG(IOP16CFG, SYS_IOCFG_P16_RXD0);	
	SYS_SET_IOCFG(IOP17CFG, SYS_IOCFG_P17_TXD0);

	NVIC_SetPriority(UART0_IRQn,3);	
	NVIC_EnableIRQ(UART0_IRQn); 
	
}

/*****************************************************************************
 ** \brief	UART_UART1_Config
 **			
 ** \param [in] none
 ** \return  none
 ** \note	
*****************************************************************************/
void UART_UART1_Config(void)
{
	uint32_t  BuadRate =9600;
	/*
	(1)设置UARTx模式
	*/
	UART_ConfigRunMode(UART1, BuadRate, UART_WLS_8, UART_PARITY_NONE,UART_STOP_BIT_1);
	/*
	(2)开启UARTx时钟
	*/	
	SYS_EnablePeripheralClk(SYS_CLK_UART1_MSK);
	/*
	(3)开启UARTx输出
	*/	
	SYS_SET_IOCFG(IOP47CFG, SYS_IOCFG_P47_TXD1);
//	SYS_SET_IOCFG(IOP46CFG, SYS_IOCFG_P46_RXD1);
	
//	NVIC_SetPriority(UART1_IRQn,3);	
//	NVIC_EnableIRQ(UART1_IRQn); 

	
}


/******************************************************************************
 ** \brief	 putchar
 ** \param [in] data
 **            	
 ** \return  none
 ** \note   <stdio.h>中需要的函数
 ******************************************************************************/
_ARMABI int (putchar)(int ch)
{
	com1_flag = 1;
	com1_timeout = 5;
	UART_WriteTHR(UART1, ch);					//开始发送数据
	while( !(((UART1->IIR & 0x0F) >> 1) & (1<<1)) && com1_timeout > 0);
	com1_flag = 0;
	return 0;
}

void USART0_send_byte(uint8_t Data)		//发送一个字节
{
	F_transmit_0 = 0;				//发送标志位置0
	com0_flag = 1;
	com0_timeout = 5;
	UART_WriteTHR(UART0, Data);					//开始发送数据
	while(F_transmit_0 == 0 && com0_timeout > 0);		//等待发送完成标志位置1退出等待
	F_transmit_0 = 0;				//发送标志位置0
	com0_flag = 0;
}

void UART_Config(void)
{
	UART_UART1_Config();
	UART_UART0_Config();
}
