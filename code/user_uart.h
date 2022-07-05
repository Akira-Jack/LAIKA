#ifndef _USER_UART_H_
#define _USER_UART_H_

#include "cms32f033x.h"

void UART_Config(void);
void USART0_send_byte(uint8_t Data);		//发送一个字节

extern u8	F_transmit_0;							//UART0发送标志位


#endif
