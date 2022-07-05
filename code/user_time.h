#ifndef _USER_TIME_H_
#define _USER_TIME_H_

#include "cms32f033x.h"
#include "define.h"

void delay_us(uint32_t nus);
void delay_ms(uint32_t ms);
void TMR_Config(void);
void delay(uint32_t nms);

#endif
