#include "cms32f033x.h"
#include "define.h"

u8 net_flag = 0;		
u8 send_feed_flag = 0;
u8 WDT_time = 0;

u8 com0_flag  = 0;
u8 com1_flag  = 0;
u8 com0_timeout  = 0;
u8 com1_timeout  = 0;

u8 get_time_error = 0;				//获取时间标志（1：获取时间 0：获取时间结果返回成功 2：获取时间超时失败，重启WiFi模块）

#ifdef TY_DP_RECORD		
u8 voice_time_time = 1;		//录音播放次数计数
u8 record_change_flag = 0;	//录音时长改变需要保存flash标志
#endif

void sys_init()
{
	SYS_DisableIOCFGProtect();			/*关闭IOCONFIG写保护*/
	SYS_DisableGPIO0Protect();			/*关闭GPIO0的相关寄存器写保护*/
	SYS_DisableGPIO1Protect();			/*关闭GPIO1的相关寄存器写保护*/
	SYS_DisableGPIO2Protect();			/*关闭GPIO2的相关寄存器写保护*/
	SYS_DisableGPIO3Protect();			/*关闭GPIO3的相关寄存器写保护*/
	SYS_DisableGPIO4Protect();			/*关闭GPIO4的相关寄存器写保护*/
	
	SYS_ConfigHSI(SYS_CLK_HSI_48M);		/*设置内部高速时钟为48Mhz*/
	SYS_EnableHSI();					/*开启高速时钟*/
	SYS_ConfigAHBClock(SYS_CLK_SEL_HSI,SYS_CLK_DIV_1);	/*设置AHB时钟为高速时钟的1分频*/
	SYS_ConfigAPBClock(AHB_CLK_DIV_1);					/*设置APB时钟为AHB时钟的1分频*/
}
/*****************************************************************************
 ** \brief	 main
 **
 ** \param [in]  none   
 **
 ** \return 0
*****************************************************************************/
int main(void)
{	
	delay_ms(300);			//延时，确保初始化成功
	
	#ifdef	BATTERY_DETECT
	float battery = 0;
//	u8 battery_buf[2];
	
    u32 vddVol = 0.0;
//    uint16_t adcRes = 0x0000;
//    float channelVol = 0.0;
	u8 battery_empty_flag_old = 0;
#endif
#ifdef MACHINE_DUMP_E
	u8 dump_send_flag = 99;
	u8 ret;
#endif
	
	u8 feed_error_old = 0;
	u8 old_min = 111;

	sys_init();

	WDT_ConfigClk(WDT_CLK_DIV_256);	
	WDT_ConfigPeriod(3200);				// 40KHZ/256 * 3200 = 5000ms
	WDT_EnableRestSystem();
	WDT_ClearOverflowIntFlag();
//	WDT_DisableRestSystem();
	WDT_ClearWDT();

	GPIO_Config();			//GPIO初始化
	TMR_Config();

	rtc_init();				//单片机时间初始化

#ifdef	BATTERY_DETECT
	ADCInit();
#endif	
	
#ifdef	SFN_BEEP
	PWMInit();
#endif	

	UART_Config();
	
	delay_ms(200);			//延时，确保初始化成功
	
	
	printf("  \r\n");
	printf("  Version: %s \r\n", VERSION_F);
	printf("  date: %s \r\n",VERSION_DATA);
	printf("PID: %s \r\n", PRODUCT_KEY);
	
	eeprom_data_init(0);
	wifi_protocol_init();		//wifi 初始化
	
	#ifdef LOCAL_RECORD_LOW
	IAP_read_user(FLASH_FEED_INFO_ADDR + 1, &a, 1);
	if(a != 2)
	{
		MX_RSTB			= 0;			//美心录音芯片复位脚
		delay_ms(5);
		{
			MX_RSTB			= 1;			//美心录音芯片复位脚
		}
//		delay_ms(1000);
//		MX_RSTB			= 0;			//美心录音芯片复位脚
//		delay_ms(5);
//		{
//			MX_RSTB			= 1;			//美心录音芯片复位脚
//		}
	}
	else
	{
		a = 0;
		user_write_flash(FLASH_FEED_INFO_ADDR + 1, &a, 1);
	}
		
	#endif


	if(RESET_KEY == 0)
	{
		wifi_test_start_flag = 1;	
	}
	
	key_time_flag = 0;
	key_time = 0;	
	
	while(1)
	{	
		wifi_uart_service();

		WDT_time = 0;
//		link_led_control();
		
		if(display_time)
		{
			display_time = 0;
			printf_rtc();		//显示时间
			if(wifi_test_flag == 0 && WIFI_PWR == 1)
			{
				mcu_get_wifi_connect_status();		//获取网络状态
								
				//5分钟间隔获取时间
				if( get_time_flag == 0)
				{
					mcu_get_system_time();				//获取时间
					if(get_time_error == 0)
						get_time_error = 1;					//获取时间标志
				}
			}
			
			if(feed_error != feed_error_old)
			{				
				send_feed_state(feed_error);
				feed_error_old = feed_error;
				mcu_dp_fault_update(DPID_FAULT,feed_error);
				printf("send error:%d \r\n",feed_error);
			}
			#ifdef SEND_LAN_CONNECT_FUNC				////发送局域网或蓝牙连接提醒功能
			if(send_error_again == 2)
			{
				printf("send ERROR_LAN_CONNT\r\n");
				mcu_dp_fault_update(DPID_FAULT,ERROR_LAN_CONNT);
				send_error_again = 1;
			}
			#endif
			
			if(wifi_test_flag == 1 && wifi_tset == 1)
			{
				mcu_start_wifitest();
			}

			if(net_flag == 3 && send_feed_flag == 1)
			{
				send_feed_flag = 2;
				send_feed_info();
			}
			
			//设备状态上报
			device_state();
			
			#ifdef MACHINE_DUMP_E
			if(DUMP_DET == 0 && dump_send_flag != 0)
			{
				delay_ms(10);
				if(DUMP_DET == 0)
				{
//					printf("send machine dump \r\n");
					dump_send_flag = 0;
					ret = ERROR_FEEDER_DUMP;
					mcu_dp_fault_update(DPID_FAULT,ret);
				}
			}
			else if(DUMP_DET && dump_send_flag != 1)
			{
				delay_ms(10);
				if(DUMP_DET)
				{
//					printf("send no machine dump \r\n");
					dump_send_flag = 1;
					ret = feed_error;
					mcu_dp_fault_update(DPID_FAULT,ret);
				}
			}
			#endif
			
			
			#ifdef OFFLINE_RECORD
//			send_offline_buf();
			#endif
			
//			printf("record_key_flag:%bd \r\n",record_key_flag);
//			printf("RECODE_KEY:%bd\r\n",(RECODE_KEY == 1));
			
//			printf("wifi_tset-wifi_tset_flag:%bd-%bd \r\n",wifi_tset,wifi_test_flag);

//			printf("DOOR_OPEN:%bd- DOOR_CLOSE:%bd \r\n",(IR_DOOR_OPEN == 1), (IR_DOOR_CLOSE == 1));
//			printf("FEED_KEY:%bd- RESET_KEY:%bd \r\n",(FEED_KEY == 1), (RESET_KEY == 1));
//			printf("wifi_test_flag:%bd- wifi_tset:%bd \r\n",wifi_test_flag, (wifi_tset == 1));
//			printf("WIFI_PWR:%bd \r\n",(WIFI_PWR == 1));
//			printf("RECORD:%bd- SPK_EN:%bd \r\n",(RECORD == 1), (SPK_EN == 1));

//			printf("voice_time_time-voice_times:%bd-%bd \r\n",voice_time_time,func_flag.voice_times);
//			printf("record_time:%hd \r\n",func_flag.record_time);
//			printf("record_state:%hd \r\n",func_flag.record_state);			
		}

		#ifdef LOCK_KEY_KEY_E			//锁键按键功能
		if(lock_key_flag > 0)		
		{
			#ifdef RELOCK_KEY_KEY_E			//解锁键按键作为解锁功能
			if(lock_key_flag == 2 && func_flag.lock_key > 0)			//
			{
				func_flag.lock_key = 0;
				PWR_LED = LED_OFF;
				lock_key_task = 1;					//锁键灯闪标志
			}
			else if(lock_key_flag == 1 && func_flag.lock_key == 0)
			{
				func_flag.lock_key = 1;
				PWR_LED = LED_OFF;
				lock_key_task = 1;					//锁键灯闪标志
			}
			#else
			if(func_flag.lock_key > 0)			//锁键标志
				func_flag.lock_key = 0;
			else if(func_flag.lock_key == 0)
				func_flag.lock_key = 1;
			
			PWR_LED = LED_OFF;
			lock_key_task = 1;					//锁键灯闪标志
			#endif
			
			printf("lock_key:%bd \n",func_flag.lock_key);
			lock_key_flag = 0;

			user_write_flash(FLASH_FUNC_ADDR,(u8 *) &func_flag, sizeof(FLASH_FUNC_E));		//写入锁键标志
		}
		#elif defined LOCK_KEY_E
		if(key_time >= 5 && record_flag == 0)				//长按6s锁喂食和录音按键
		{
			if(func_flag.lock_key > 0)			//锁键标志
				func_flag.lock_key = 0;
			else if(func_flag.lock_key == 0)
				func_flag.lock_key = 1;
			
			lock_key_task = 1;
			key_time_flag = 0;
			key_time = 0;
			key_dowm_time = 0;
			user_write_flash(FLASH_FUNC_ADDR,(u8 *) &func_flag, sizeof(FLASH_FUNC_E));		//写入锁键标志
		}
		#endif
		

		if(reset_flag == 1)			//开始复位标志
		{
			reset_flag = 2;
			mcu_reset_wifi();			//发送复位指令给WiFi模块
			
			spk_reset_flag = 1;			//清录音芯片
		}
		if(reset_flag == 2)
		{
			if(net_flag == 1)
			{
				printf(" reset the mcu 3\r\n");
				delay_ms(1000);
				eeprom_data_init(0x0b);		//初始化flash
				delay_ms(800);
//				if(spk_reset_flag == 2)			//录音芯片清除成功
				{
					SYS_ResetCPU();				//软件复位,系统重新从用户代码区开始运行程序
					spk_reset_flag = 0;
					reset_flag = 0;
				}
				reset_flag = 3;			
			}
		}

		#ifdef TY_DP_RECORD
		if(record_change_flag == 1)
		{
			printf("record_change_flag \r\n");
			record_change_flag = 0;
			user_write_flash(FLASH_FUNC_ADDR, (uint8_t *) &func_flag, sizeof(FLASH_FUNC_E));
			
			//mcu_dp_bool_update(DPID_RECORDING_STATUS, func_flag.record_state);
		}	
		#endif
		
//		if(wifi_test_flag == 10)					//10s超时未检测到产测热点
//		{
//			SYS_ResetMCU();				//软件复位,系统重新从用户代码区开始运行程序
//		}
		
		if(feed_one_flag_now == 1)
		{
			feed_one_flag_now = 0;
			printf("feed one on key !\r\n");
			feed_info.weight = 1;
			feed_required = FEED_MANUAL;
			feed_type = FEED_KEY_T;
		}
		
		if(rtc_date.minute != old_min)						//喂食检测：一分钟置1一次
		{
			old_min = rtc_date.minute;

			feed_scan(&feed_info);			//扫描eeprom判断是否需要执行定时喂食
		}
		else if(feed_detect_again)			//再次喂食检测
		{
			if((feed_error & ERROR_IR) == 0)		//不是红外错误，出粮口正常
			{
//				printf(" feed required : feed_old !\r\n");
				feed_required = FEED_OLD;			//饲料所需
			}
		}
		
		if(feed_required > 0)
		{
			feed_error = feed_run(feed_required, &feed_info, feed_type);
			
			feed_required = 0;
			
			printf("++++++++++++++ feed_error:%d \r\n",feed_error);

			if(feed_error & ERROR_IR)			//喂食红外错误
			{
				feed_detect_again = 1;			//红外错误：确保喂食错误，再次喂食检测
			}
			else
			{
				feed_detect_again = 0;
			}
			task_feed_again = 0;
		}

			#ifdef DOOR_CONTROL
			//没有再次喂食计划、关门没错误、门仓到位没在关门到位位置
			if(feed_detect_again == 0 && door_open == 0 && (DOOR_DET_N != DOOR_CLOSE) && (FEED_DET == 0))
			{
				motor_door_close();				//关仓门
					
				if(AIR_DET == NO_VACUUM)		//非真空
					air_exhaust();				//抽气
			}
			#endif	

		
		#ifdef TY_DP_DESICCANT
		desiccant_pull();
		#endif
		
//#ifndef BATTERY_DC_DETECT
//		if(DC_DET == 0)
//		{
//			if(task_run_led == 0 && wifi_test_bink_time != 10 && lock_key_task == 0)
//				BAT_LED = LED_ON;

//			DC_det_flag = 1;
//			
//			#ifdef DC_POWER_CONTROL
//			WIFI_PWR = 0;
//			#endif
//		}
//		else
//		{
//			#ifdef DC_POWER_CONTROL
//			if(DC_det_flag == 1 && WIFI_PWR == 0)	
//			#else
//			if(DC_det_flag == 1)
//			#endif
//			{
//				DC_det_flag = 0;
//				WIFI_PWR = 1;
//				BAT_LED = LED_OFF;
//			}
//		}
//#endif
		
#ifdef BATTERY_DETECT
		battery_flag = 1;				//电池标志
		if(DC_DET == 0)
		{			
			if(!ADC1_IS_BUSY())
			{ 
				ADC1_Go();
				while(ADC1_IS_BUSY());
				vddVol = ADC1_GetResult(ADC_CHANNEL);
				battery = 5.0 * vddVol / 4096.0;

				printf("ADCValue:0x%04X  \r\n",vddVol);
				printf("ADCValueLocal:%.2f V \r\n",battery);
			}
				
			battery_percent = ((battery - BATTERY_LOWEST) / (BATTERY_FULL - BATTERY_LOWEST)) * 100;
			
			if(battery_percent <= BATTERY_LOW_PC)
			{
				battery_empty_flag = 1;
			}
			else
			{
				battery_empty_flag = 0;
				WIFI_PWR = 1; 
			}
			
			if(battery_empty_result || (battery_empty_flag_old != battery_empty_flag))
			{
				battery_empty_flag_old = battery_empty_flag;

				mcu_dp_value_update(DPID_BATTERY_PERCENTAGE, battery_percent);
				battery_empty_result = 0;
			}
		}
		else if(battery_empty_result)
		{
			WIFI_PWR = 1; 
			battery_percent = 100;
			battery_empty_result = 0;
			mcu_dp_value_update(DPID_BATTERY_PERCENTAGE, battery_percent);			
		}
		else if(battery_empty_flag)
		{
			WIFI_PWR = 1; 
			battery_empty_flag = 0;
		}
#endif		
	}
}



//跳转到boot区执行
void Jump(void)
{
	FMC->LOCK = FMC_WRITE_KEY;
	FMC->CON &= ~FMC_CON_ISPS_Msk;	
	FMC->LOCK = 0x00;
	SYS_ResetCPU();
}

////跳转到主程序区执行(boot函数用)
//void Jump(void)
//{
//	FMC->LOCK = FMC_WRITE_KEY;
//	FMC->CON |= FMC_CON_ISPS_Msk;	
//	FMC->LOCK = 0x00;
//	SYS_ResetCPU();
//}










