/*******************************************************************************
* Copyright (C) 2019 China Micro Semiconductor Limited Company. All Rights Reserved.
*
* This software is owned and published by:
* CMS LLC, No 2609-10, Taurus Plaza, TaoyuanRoad, NanshanDistrict, Shenzhen, China.
*
* BY DOWNLOADING, INSTALLING OR USING THIS SOFTWARE, YOU AGREE TO BE BOUND
* BY ALL THE TERMS AND CONDITIONS OF THIS AGREEMENT.
*
* This software contains source code for use with CMS
* components. This software is licensed by CMS to be adapted only
* for use in systems utilizing CMS components. CMS shall not be
* responsible for misuse or illegal use of this software for devices not
* supported herein. CMS is providing this software "AS IS" and will
* not be responsible for issues arising from incorrect user implementation
* of the software.
*
* This software may be replicated in part or whole for the licensed use,
* with the restriction that this Disclaimer and Copyright notice must be
* included with each copy of this software, whether used in part or whole,
* at all times.
*/

/****************************************************************************/
/** \file isr.c
**
**	History:
**	
*****************************************************************************/
/****************************************************************************/
/*	include files
*****************************************************************************/
#include "cms32f033x.h"
#include  "define.h"

u8 spk_reset_flag = 0;			//清录音标志
u8 record_flag = 0;				//准备录音标志

u8 pwr_led_flag = 0;			//红灯准备亮，绿灯灭标志

u8 wifi_test_bink_off_flag = 0;		//WiFi测试模式停止闪烁时间
u8 wifi_test_bink_time_count = 0;		//WiFi测试模式闪烁次数计数
u16 RES_KEY_time = RESET_KEY_TIME_U;

u8 record_key_flag = 0;			//录音按键按下标志
u8 lock_key_flag = 0;			//锁键触发标志

/****************************************************************************
 ** \brief	NMI_Handler
 **
 ** \param [in]  none   
 ** \return none
 ** \note
****************************************************************************/
void NMI_Handler(void)
{
	
}
/****************************************************************************
 ** \brief	HardFault_Handler
 **
 ** \param [in]  none   
 ** \return none
 ** \note
****************************************************************************/
void HardFault_Handler(void)
{
	
}

/****************************************************************************
 ** \brief	SVC_Handler
 **
 ** \param [in]  none   
 ** \return none
 ** \note
****************************************************************************/
void SVC_Handler(void)
{
	
}

/****************************************************************************
 ** \brief	PendSV_Handler
 **
 ** \param [in]  none   
 ** \return none
 ** \note
****************************************************************************/
void PendSV_Handler(void)
{
	
}
/****************************************************************************
 ** \brief	SysTick_Handler
 **
 ** \param [in]  none   
 ** \return none
 ** \note
****************************************************************************/
void SysTick_Handler(void)
{
	
}
           
/****************************************************************************
 ** \brief	GPIO0_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note
****************************************************************************/
void GPIO0_IRQHandler(void)
{

}

/****************************************************************************
 ** \brief	GPIO1_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note
****************************************************************************/
void GPIO1_IRQHandler(void)
{
	
}
/****************************************************************************
 ** \brief	GPIO2_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note
****************************************************************************/
void GPIO2_IRQHandler(void)
{
	//喂食按键中断
	if(GPIO_GetIntFlag(GPIO2,GPIO_PIN_6))
	{
		key_time_flag = 1;
		key_dowm_time++;
		
		GPIO_ClearIntFlag(GPIO2,GPIO_PIN_6);
	}		
}
/****************************************************************************
 ** \brief	GPIO3_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note
****************************************************************************/
void GPIO3_IRQHandler(void)
{
	
}
/****************************************************************************
 ** \brief	GPIO4_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note
****************************************************************************/
void GPIO4_IRQHandler(void)
{
	
}
/****************************************************************************
 ** \brief	CCP_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note
****************************************************************************/
void CCP_IRQHandler(void)
{
	
}

/****************************************************************************
 ** \brief	WWDT_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note
****************************************************************************/
void WWDT_IRQHandler(void)
{
		
}
/****************************************************************************
 ** \brief	EPWM_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note
****************************************************************************/
void EPWM_IRQHandler(void)
{
	
}
/****************************************************************************
 ** \brief	ADC1_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note
****************************************************************************/
void ADC1_IRQHandler(void)
{

}
/****************************************************************************
 ** \brief	ACMP_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note
****************************************************************************/
void ACMP_IRQHandler(void)
{
//	if(ACMP_GetIntFlag(ACMP0))
//	{
//		GPIO1->DO_f.P2 =  ~GPIO1->DO_f.P2;		
//		ACMP_ClearIntFlag(ACMP0);
//	}	
}
/****************************************************************************
 ** \brief	UART0_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note
****************************************************************************/
static volatile uint32_t U_IntID;
static volatile uint32_t U_LSRFlag;
static volatile uint32_t U_count;

void UART0_IRQHandler(void)
{
	U_IntID = (UART0->IIR & 0x0F);		//获取中断事件ID。此处不能打断点，会导致状态位丢失，读取后IIR寄存器值将会改变 
	U_IntID = U_IntID >>1;
	U_LSRFlag = UART0->LSR;
	
	switch(U_IntID)								//处理事件
	{
		case 0:										//Modem 状态改变
			break;
		case 0x1:									//THR 寄存器为空 （发送后 THR的值会自动清除）		
			F_transmit_0 = 1;		
		  break;
					
		case 0x2:									//接收数据有效中断
			uart_receive_input(UART_ReadRBR(UART0));
			break;		
		case 0x3:									//Rx line 状态变化 ->监测LSR[4:1]	

			break;		
		case 0x6:					//1帧数据的定时时间内未接收到新数据,触发FIFO 接收定时溢出中断	
			
			break;		
	}	
}
/****************************************************************************
 ** \brief	UART1_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note
****************************************************************************/
void UART1_IRQHandler(void)
{	

}
/****************************************************************************
 ** \brief	TMR0_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note
****************************************************************************/
void TMR0_IRQHandler(void)
{
	static u8 time_WDT = 100;
//	static u16 ledtime_all = LEDTIME_ALL_TIME;			//关闭红灯闪烁
	static u8 key_time_time = 100;
	static u16 key_exit_time = 200;					//上电或复位3s之后开启按键中断
	
	#if defined LOCAL_RECORD || defined LOCK_KEY_E
	static u8 lock_key_flag_time = 210;				//锁键提示红灯闪烁时间	
	static u16 lock_key_time = 0;					//锁键按下时间
	#endif
	static u16 get_time_time = 300;					//获取时间计时
	static u16 cancel_send_feed_error_time = 500;	//取消涂鸦APP喂食异常弹窗
//	static u16 get_error = 3000;					//获取时间失败11s计时，失败后重启WiFi模块，防止WiFi模块挂掉不恢复
	static u16 error_uart_time = 0;					//串口接收错误时间
	
	time_WDT--;
	if(time_WDT == 0 && WDT_time < 60)
	{
		WDT_time++;
		time_WDT = 100;
		WDT_ClearWDT();
	
		if(com1_flag)
		{
			com1_timeout--;
			if(com1_timeout == 0)
			{
				com1_flag = 0;
			}
		}
		if(com0_flag)
		{
			com0_timeout--;
			if(com0_timeout == 0)
			{
				com0_flag = 0;
			}
		}
	}
	
	#if 1
	if(key_exit_time > 0)			//上电或复位3秒之后才开启按键中断
	{
		key_exit_time--;
		if(key_exit_time == 0)
		{
			WIFI_PWR = 1;
		}
	}

	//30s后不能获取网络状态，重启MCU
	#ifdef BATTERY_DC_DETECT
	if(net_flag == 0 && wifi_test_flag == 0)							
	#else
	if(net_flag == 0 && DC_DET && wifi_test_flag == 0)						
	#endif
	{
		error_uart_time++;
		if(error_uart_time >= 3000)
		{
			SYS_ResetCPU();
		}
	}

	//取消喂食异常
	if(cancel_send_feed_error == 1)
	{
		cancel_send_feed_error_time--;
		if(cancel_send_feed_error_time == 0)
		{
			cancel_send_feed_error = 2;
			cancel_send_feed_error_time = 500;
		}
	}
	
//	#ifdef SEND_LAN_CONNECT_FUNC				////发送局域网或蓝牙连接提醒功能
//	if(feed_error && feed_error != ERROR_LAN_CONNT)
//	#else
//	if(feed_error)
//	#endif
//	{
//		task_run_led = 1;
//	}
//	else
//	{
//		task_run_led = 0;
//		#ifdef BATTERY_DC_DETECT
//		if(led_task_static && wifi_test_bink_time != 10)
//		#else
//		if(led_task_static && DC_DET && wifi_test_bink_time != 10)
//		#endif
////			PWR_LED = LED_OFF;
//			;
//	}	
	
/////******************************* 桶内余粮检测 **********************************//////	
#ifdef IR_FOOD_TUBE
	food_det_isr();
#endif
/////******************************* 电池电量 **********************************//////
#ifdef BATTERY_DETECT
	battery_isr();					//电池相关处理
#endif	
/////******************************* 按键处理 **********************************//////
	if(key_time_flag)					//按键时间计时，
	{
		key_time_time--;
		if(key_time_time == 0)
		{
			key_time++;
			key_time_time = 100;
		}
	}
	else if(key_time_time != 100 || key_time != 0)
	{
		key_time_time = 100;
		key_time = 0;
	}
		
	if(RESET_KEY == 0 && record_flag == 0)				//长按复位
	{
		RES_KEY_time--;
		if(RES_KEY_time == 0)
		{
			if(reset_flag == 0)
				reset_flag = 1;
				
			RES_KEY_time = RESET_KEY_TIME_U;
		}
	}
	else if(RES_KEY_time != RESET_KEY_TIME_U)
		RES_KEY_time = RESET_KEY_TIME_U;

#ifdef LOCK_KEY_KEY_E			//锁键按键功能
	if(LOCK_KEY == 0)
	{
		lock_key_time++;
		if(lock_key_time >= 600)
		{
			lock_key_flag = 1;
			lock_key_time = 0;
		}
	}

	#ifdef RELOCK_KEY_KEY_E			//解锁键按键作为解锁功能
	if(RELOCK_KEY == 0)
	{
		lock_key_time++;
		if(lock_key_time >= 600)
		{
			lock_key_flag = 2;
			lock_key_time = 0;
		}
	}
	
	if(lock_key_time != 0 && RELOCK_KEY && LOCK_KEY)
	{
		lock_key_time = 0;
	} 
	#else
	else if(lock_key_time != 0 && LOCK_KEY)
	{
		lock_key_time = 0;
	}
	#endif
#endif
	
	#ifdef KEY_BUZZER_FUNC
	buzzer_func_isr();
	#endif
/////******************************* 录音功能 **********************************//////
	feed_key_isr();
	#ifndef NO_RECORD_FUNC
	record_key_isr();
	speak_reset_isr();
	#endif
/////******************************* 灯处理 **********************************//////
	#ifdef LOCK_KEY_E
	if(lock_key_task == 1)
	{
		LINK_LED = LED_OFF;
		pwr_led_flag = 1;					//红灯准备亮，绿灯灭标志
		lock_key_flag_time--;
		if(lock_key_flag_time == 130)		
		{			
//			PWR_LED = LED_ON;
		}
		else if(lock_key_flag_time == 110)
		{			
//			PWR_LED = LED_OFF;
		}
		else if(lock_key_flag_time == 60)
		{			
//			PWR_LED = LED_ON;
		}		
		else if(lock_key_flag_time == 40)
		{
//			PWR_LED = LED_OFF;
			lock_key_flag_time = 210;
			lock_key_task = 0;
			pwr_led_flag = 0;
		}
	}
	#endif

//	#ifdef ERROR_LIGHT_ONE_MIN
//	if(task_run_led && ledtime_flag == 0)				//红灯错误闪烁任务
//	#else
//	if(task_run_led)									//红灯错误闪烁任务
//	#endif	
//	{
//		task_delay_led--;
//		ledtime_all--;

//		if(task_delay_led == 0)
//		{
//			#ifdef FEEDED_ALLOW_ERROR
//			if(wifi_test_bink_time != 10 && led_status_allow_feed)
//			#else
//			if(wifi_test_bink_time != 10)
//			#endif
//			{
//				if(lock_key_task != 1)					//锁键闪烁标志
//				{
//					led_task();
//				}
//				
//				#ifdef ERROR_LIGHT_ONE_MIN
//				if(ledtime_all <= 10)
//				{
////					PWR_LED = LED_OFF;
//					ledtime_flag = 1;
//					led_status_allow_feed = 0;
//				}
//				#endif
//			}
//			else
//			{
//				pwr_led_flag = 0;
////				PWR_LED = LED_OFF;
//			}
//			task_delay_led = LED_PERIOD;
//		}
//	}
//	else if(ledtime_all != LEDTIME_ALL_TIME)
//		ledtime_all = LEDTIME_ALL_TIME;

#ifdef IR_FEED_TUBE
//	if(task_run_motor == 0)
//	{
//		task_delay_error--;

//		if(task_delay_error == 1)
//		{
//			IR_LED_CTRL = 1;						//红外灯亮
//		}

//		if(task_delay_error == 0)
//		{
//			error_task();
//			task_delay_error = ERROR_PERIOD;
//			IR_LED_CTRL = 0;
//		}
//	}

	error_task();
	
	if(feed_detect_again)
	{
		task_feed_again++;

		if(task_feed_again > FEED_AGAIN_PERIOD)
		{
			feed_detect_again = 0;
			task_feed_again = 0;
		}
	}

//	if(feed_detect_sw)			//喂食5分钟之后关闭红外检测
//	{
//		task_feed_sw++;

//		if(task_feed_sw > FEED_AGAIN_PERIOD)		//5分钟
//		{
//			feed_detect_sw = 0;						//清0喂食后标志位
//			task_feed_sw = 0;
//			led_status_allow_feed = 0;
//		}
//	}
#endif

/////************************************************************************//////
/////******************************* 获取时间 **********************************//////
	if(get_time_flag > 0)
	{
		if(get_time_time == 0)
		{
			if(get_time_flag == 1)
			{
				get_time_time = GET_TIME_START ;
				get_time_flag = 0;
			}
			else if(get_time_flag == 2)
			{
				get_time_time = GET_TIME_STOP;
				get_time_flag = 1;
			}
		}
		if(get_time_flag > 2 || get_time_time == 0)
		{
			get_time_flag = 1;
			get_time_time = GET_TIME_START;
		}
		get_time_time--;
	}

//网络灯
#ifdef LINK_LED_FUNC	
	link_led_isr();
#endif
	
	//卡粮
	static u8 motor_led_time = 20;
	if(feed_error & ERROR_MOTOR_TIMEOUT)
	{
		motor_led_time--;
		if(motor_led_time == 0)
		{
			MOTOR_LED = ~MOTOR_LED;
			motor_led_time = 20;
		}
	}
	else
		MOTOR_LED = LED_OFF;
	//缺粮
	static u8 food_led_time = 20;
	if(feed_error & ERROR_NOT_ENOUGH)
	{
		food_led_time--;
		if(food_led_time == 0)
		{
			FOOD_LED = ~FOOD_LED;
			food_led_time = 20;
		}
	}
	else
		FOOD_LED = LED_OFF;

	//堵粮
	static u8 feed_led_time = 20;
	if(feed_error & ERROR_IR)
	{
		feed_led_time--;
		if(feed_led_time == 0)
		{
			FEED_LED = ~FEED_LED;
			feed_led_time = 20;
		}
	}
	else
		FEED_LED = LED_OFF;
	
	//气压异常
	static u8 air_led_time = 20;
	if(feed_error & ERROR_VACUUM_TIMEOUT)
	{
		air_led_time--;
		if(air_led_time == 0)
		{
			PUMP_LED = ~PUMP_LED;
			air_led_time = 20;
		}
	}
	else if(AIR_DET == VACUUM) //真空
	{
		PUMP_LED = LED_ON;
	}
	else 
	{
		PUMP_LED = LED_OFF;
	}
	//氛围灯
	static u8 led_driver_time = 50;
	if(feed_error > 0)
	{		
		led_driver_time--;
		if(led_driver_time == 0)
		{
			LED_DRIVER = ~LED_DRIVER;
			led_driver_time = 50;
		}
	}
	else
	{
		if(atmosphere_lamp_state)
		{
			LED_DRIVER = LED_ON;
		}
		else
		{
			LED_DRIVER = LED_OFF;
		}
	}		
/////************************************************************************//////
#endif	/**** #if 0 ***/	
	TMR_ClearOverflowIntFlag(TMR0);
	
}
/****************************************************************************
 ** \brief	TMR1_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note
****************************************************************************/
void TMR1_IRQHandler(void)
{
	rtc_task();
	
	TMR_ClearOverflowIntFlag(TMR1);
}
/****************************************************************************
 ** \brief	WDT_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note
****************************************************************************/
void WDT_IRQHandler(void)
{
	
}
/****************************************************************************
 ** \brief	I2C_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note
****************************************************************************/
void I2C_IRQHandler(void)
{
	
}
/****************************************************************************
 ** \brief	SPI_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note
****************************************************************************/
void SPI_IRQHandler(void)
{
	
}
/****************************************************************************
 ** \brief	GPIO0_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note
****************************************************************************/
void LVD_IRQHandler(void)
{	

}



