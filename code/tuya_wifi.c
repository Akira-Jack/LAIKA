#include "cms32f033x.h"

#include "define.h"

u16	link_led_time = 0;				//网络灯闪烁时间
u8 wifi_error_flag = 0;			//网络灯AP模式
u8 link_led_open = 1;				//开启网络灯闪烁
u8 link_led_open_long = 0;			//网络灯常亮

u8 	wifi_test_flag = 0;				//wifi测试模式标志
u8 	wifi_test_bink_time = 0;		//wifi测试模式闪烁次数
u8 wifi_tset = 0;					//WiFi测试模式间隔发送标志

u8  wifi_test_start_flag = 0;		//上电进入WiFi测试模式延时标志
u8  wifi_get_mode_delay_flag = 1;	//上电获取WiFi模式延时标志		1(延时中)  2（延时结束或已经获取到WiFi模式）

u8 remaining_change = 0;			//干燥剂剩余时间更新标志

u8 SPK_EN_time = 10;
u8 reset_res_time = RESET_RECORD_TIME;
u16 rec_time_out = RECORD_TIME_MAX_U;			//录音超时
u8 key_dowm_time = 0;			//按键按下次数

u8 record_timeout_flag = 0;			//录音超时标志


u8 delay_flag 	= 0;			//延时标志位
u16 time_delay 	= 0;			//延时时间对比

int key_time 	= 1;			//按键时间	超过6秒复位
u8 key_time_flag = 0;			//开始记录按键时间标志
u8 reset_flag	= 0;			//复位标志位
u8 feed_one_flag_now = 0;		//手动喂食一次标志位
u8 SPK_EN_flag = 0;				//播音标志

u8 lock_key_task = 0;			//锁键成功闪烁标志

//feed_key_isr
#ifdef ONE_KEY_RECORD
void feed_key_isr()
{
	if(key_time_flag == 1)			//按键喂食检测
	{
		if(FEED_KEY)
		{
			if(key_time < 1 && key_dowm_time >= 2)
			{
				if(func_flag.lock_key == 0)			//没有锁键时
					feed_one_flag_now = 1;				//喂食
				
				key_time_flag = 0;
				key_dowm_time = 0;
			}	
//			else if(key_dowm_time == 1 && key_time >= 1 && record_flag == 0 && key_time <= 2)
//			{
//				if(func_flag.lock_key == 0)			//没有锁键时
//					feed_one_flag_now = 1;				//喂食
//				
//				key_time_flag = 0;
//				key_dowm_time = 0;
//			}
			else if(key_time >= 1)
			{
				key_time_flag = 0;
				key_dowm_time = 0;
			}
		}
	}
}
#else // THREE_KEY_FUNC
void feed_key_isr()
{
	if(key_time_flag == 1 && FEED_KEY == 1)
	{
		if(key_time <= 1)
		{
			if(func_flag.lock_key == 0)				//没有锁键时
				feed_one_flag_now = 1;				//按键喂食
		}	
		key_time_flag = 0;
		key_time = 0;
	}
}
#endif


//无录音功能
#ifndef NO_RECORD_FUNC
//speak_reset_isr
#ifdef TY_DP_RECORD
void speak_reset_isr()
{
	static u16 record_time_time = 0;			//录音时长计时

	if(SPK_EN_flag == 1)					//播音（拉高然后100ms在拉低）
	{
		if(func_flag.record_state && voice_time_time > 0)		//存在录音并且录音播放次数大于0
		{
			RECORD_PLAY_E;
			SPK_EN_time--;
			if(SPK_EN_time == 0)
			{	
				SPK_EN_flag = 2;
				SPK_EN_time = 10;
				RECORD_PLAY_END_E;
				voice_time_time--;
				record_time_time = func_flag.record_time;
			}
		}
	}
	
	if(SPK_EN_flag == 2)				//等待播放完成，播放下一次录音
	{
		record_time_time--;
		if(record_time_time == 0)
		{
			SPK_EN_flag = 1;
		}
	}
	else if(voice_time_time == 0)
	{
		SPK_EN_flag = 0;
	}
	
	if(spk_reset_flag == 1)				//清录音（录音然后隔600ms结束录音）
	{
		RECORD_E;
		reset_res_time--;
		if(reset_res_time == 0)
		{	
			func_flag.record_state = 0;
			spk_reset_flag = 0;
			RECORD_END_E;
			reset_res_time = RESET_RECORD_TIME;
//			spk_reset_flag = 2;
		}	
	}	
}
#else
void speak_reset_isr()
{
	if(SPK_EN_flag)					//播音（拉高然后100ms在拉低）
	{
		RECORD_PLAY_E;
		SPK_EN_time--;
		if(SPK_EN_time == 0)
		{	
			SPK_EN_flag = 0;
			RECORD_PLAY_END_E;
			SPK_EN_time = 10;
		}	
	}
	else
	{
		RECORD_PLAY_END_E;
	}
	
	if(spk_reset_flag == 1)				//清录音（录音然后隔600ms结束录音）
	{
		RECORD_E;
		reset_res_time--;
		if(reset_res_time == 0)
		{	
			spk_reset_flag = 0;
			RECORD_END_E;
			reset_res_time = RESET_RECORD_TIME;
//			spk_reset_flag = 2;
		}	
	}
}	
#endif

//record_key_isr
#ifdef TY_DP_RECORD
void record_key_isr()
{
	if(record_flag == 1 && RESET_KEY == 0 && record_timeout_flag == 0)		//开始录音（未锁时）
	{
		RECORD_E;
				
		rec_time_out--;
		if(rec_time_out == 0)
		{
			record_flag = 2;
			RECORD_END_E;
			rec_time_out = RECORD_TIME_MAX_U;
			record_timeout_flag = 1;
		}
	}
	else 
	{
		if(record_flag == 2)
		{
			func_flag.record_state = 1;
			record_flag = 0;
			func_flag.record_time = 1000;
			record_change_flag = 1;
		}
		else if(record_flag > 0 && rec_time_out != RECORD_TIME_MAX_U)
		{
			if((RECORD_TIME_MAX_U - rec_time_out) <= 60)			//触发时间小于600ms，相当于清录音
			{
				func_flag.record_state = 0;
				func_flag.record_time = 0;
			}
			else
			{
				func_flag.record_state = 1;
				func_flag.record_time = RECORD_TIME_MAX_U - rec_time_out;			//
			}		
			record_change_flag = 1;
		}
		
		if(spk_reset_flag == 0)
		{
			RECORD_END_E;
		}
		if(rec_time_out != RECORD_TIME_MAX_U)
		{
			rec_time_out = RECORD_TIME_MAX_U;
			record_flag = 0;
		}
		
		record_timeout_flag = 0;
	}	
}
#elif defined ONE_KEY_RECORD
void record_key_isr()
{
	if(record_flag && FEED_KEY == 0 && record_timeout_flag == 0)		//开始录音（未锁时）
	{
		RECORD_E;
				
		rec_time_out--;
		if(rec_time_out == 0)
		{
			record_flag = 0;
			RECORD_END_E;
			rec_time_out = RECORD_TIME_MAX_U;
			
			key_time_flag = 0;
			key_time = 0;
			record_timeout_flag = 1;
		}
	}
	else 
	{
		if(spk_reset_flag == 0)
			RECORD_END_E;
		if(rec_time_out != 1000)
		{
			rec_time_out = RECORD_TIME_MAX_U;
			record_flag = 0;
			
			key_time_flag = 0;
			key_time = 0;
		}
		
		record_timeout_flag = 0;
	}
}
#elif defined PLAY_KEY_RECORD
void record_key_isr()
{
	static u16	record_key_time = 0;			//录音按键计时

	if(record_key_flag && RECODE_KEY == 0)
	{
		if(record_key_time >= 100 && func_flag.lock_key == 0 && record_timeout_flag == 0)
		{
			RECORD_E;
				
			rec_time_out--;
			if(rec_time_out == 0)
			{
				RECORD_END_E;
				rec_time_out = RECORD_TIME_MAX_U;
				record_timeout_flag = 1;
			}
		}
	}
	else 
	{
		if(record_key_flag && record_key_time < 100)
		{
			SPK_EN_flag = 1;
		}
		record_key_flag = 0;
		
		if(spk_reset_flag == 0)
			RECORD_END_E;
		if(rec_time_out != RECORD_TIME_MAX_U)
			rec_time_out = RECORD_TIME_MAX_U;
		
		record_timeout_flag = 0;
	}	
	
	if(record_key_flag)
	{
		record_key_time++;
	}
	else if(record_key_time > 0)
	{
		record_key_time = 0;
	}	

}
#else
void record_key_isr()
{
	if(RECODE_KEY == 0 && func_flag.lock_key == 0 && record_timeout_flag == 0)//if(record_flag)		//开始录音（未锁时）
	{
		RECORD_E;
			
		rec_time_out--;
		if(rec_time_out == 0)
		{
			RECORD_END_E;
			rec_time_out = RECORD_TIME_MAX_U;
			record_timeout_flag = 1;
		}
	}
	else 
	{		
		if(rec_time_out > (RECORD_TIME_MAX_U - 30) && GET_RECORD_E)
		{
			rec_time_out--;
		}
		else
		{
			if(spk_reset_flag == 0)
				RECORD_END_E;
			if(rec_time_out != RECORD_TIME_MAX_U)
				rec_time_out = RECORD_TIME_MAX_U;
			
			record_timeout_flag = 0;
		}
	}
}
#endif

#endif		///* end of NO_RECORD_FUNC */

///
//buzzer_func_isr
#ifdef KEY_BUZZER_FUNC
u8 buzzer_flag = 0;			//蜂鸣器响标志
void buzzer_func_isr()
{
	static u8 buzzer_time = 0;
	static u8 buzzer_flag_1 = 0;
	static u8 key_flag_2 = 0;
	static u8 key_flag_3 = 0;
	static u8 reset_buzz_flag = 0;
//	static u8 reset_buzz_time = 0;
	static u8 reset_buzz_delay = 0;
	
	//蜂鸣器响一声
	if(buzzer_flag)
	{
		if(buzzer_time == 0 && buzzer_flag_1 == 0)
		{
			buzzer_time = 20;
			buzzer_flag_1 = 1;
			buzzer_flag = 0;
		}	
	}
	else
	{
		buzzer_flag_1 = 0;
	}
	
	if(FEED_KEY == 0)
	{
		if(buzzer_time == 0 && key_flag_2 == 0)
		{
			buzzer_time = 20;
			key_flag_2 = 1;
		}
	}
	else
	{
		key_flag_2 = 0;
	}
	
	if(RESET_KEY == 0)
	{
		if(buzzer_time == 0 && key_flag_3 == 0)
		{
			buzzer_time = 20;
			key_flag_3 = 1;
		}
	}
	else
	{
		key_flag_3 = 0;
	}
	
	#ifdef LOCK_KEY_KEY_E
	if(LOCK_KEY == 0 && func_flag.lock_key == 0)
	{
		if(buzzer_time == 0 && key_flag_4 == 0)
		{
			buzzer_time = 20;
			key_flag_4 = 1;
		}
	}
	else
	{
		key_flag_4 = 0;
	}
	#endif	
	
	if(spk_reset_flag && reset_buzz_flag == 0)
	{
		buzzer_time = 10;
		reset_buzz_flag = 1;
		reset_buzz_delay = 60;
	}
	else if(reset_buzz_delay > 0  && reset_buzz_flag < 3)	//&& buzzer_time == 0
	{
		reset_buzz_delay--;
		if(reset_buzz_delay == 0)
		{
			if(reset_buzz_flag == 1)
				reset_buzz_flag = 2;
			else if(reset_buzz_flag == 2)
			{
				reset_buzz_flag = 3;
			}
			reset_buzz_delay = 20;
			buzzer_time = 10;
		}
	}	
	
	if(buzzer_time > 0 && buzzer_enable_flag == 1)
	{
		buzzer_time--;
		#ifdef POWER_BEEP
		BUZZER_EN = 1;
		#elif SFN_BEEP
		buzzer_en_func(1);
		#endif
	}
	else
	{
		#ifdef POWER_BEEP
		BUZZER_EN = 0;	
		#elif SFN_BEEP
		buzzer_en_func(0);
		#endif	
	}
	
}
#endif


#ifdef TY_DP_DESICCANT
void desiccant_pull()
{	
	if(func_flag.remaining_time == 0 && func_flag.remind_time > 0)
	{
		mcu_dp_fault_update(DPID_FAULT,ERROR_DESICCANT_TIME);
	}
	
	if(remaining_change)
	{
		remaining_change = 0;
		user_write_flash(FLASH_FUNC_ADDR, (uint8_t *) &func_flag, sizeof(FLASH_FUNC_E));
		mcu_dp_value_update(DPID_REMAINING_TIME, func_flag.remaining_time);
	}
}

#endif

#ifdef BATTERY_DETECT

u8	battery_flag = 0;				//电池标志
u8	battery_empty_flag = 0;			//电量不足标志
u8	battery_empty_result = 0;		//电量不足推送标志
s8 	battery_percent = 100;			//电池电量百分比


void battery_isr(void)
{
	static u16 battery_time = 0;			//电量推送时间
//	static u16 battery_empty_time = 500;	//电量不足灯闪烁时间5s间隔，亮200ms

	static u16 battery_led_time_slow = 50;
	static u16 battery_led_time = 20;
	static u16 battery_off_wifi = 0;
	
	//电量推送任务
	
	if(battery_flag)			
	{
		battery_time++;
		if(battery_time == 300)
		{
			battery_empty_result = 1;
		}
		if(battery_time > 30000)		//每隔5分钟推送一次电量
		{
			battery_time = 0;
		}
	}
	else if(battery_time != 0)
		battery_time = 0;
	
	//低电量推送5s后关闭WiFi
	if(battery_empty_flag && WIFI_PWR && net_flag == 3)
	{
		battery_off_wifi++;
		if(battery_off_wifi >= 500)
		{
			WIFI_PWR = 0;
			battery_off_wifi = 0;
		}
	}
	else
	{
		battery_off_wifi = 0;
	}
	
	if(DC_DET == 0)
	{
		if(battery_percent <= 20)
		{
			battery_led_time--;
			if(battery_led_time == 0)
			{
				BAT_LED = ~BAT_LED;
				battery_led_time = 20;
			}
		}
		else if(battery_percent <= 50)
		{
			battery_led_time_slow--;
			if(battery_led_time_slow == 0)
			{
				BAT_LED = ~BAT_LED;
				battery_led_time_slow = 50;
			}
		}
		else
		{
			BAT_LED = LED_ON;
		}
	}
	else
		BAT_LED = LED_OFF;
}
#endif


#ifdef LINK_LED_FUNC 
void link_led_isr(void)
{
	static u8 link_time = 100;
	static u16 link_time_error = 600;
	static u16 wifi_get_mode_delay_time = 600;
//	static u8 pwr_led_flag = 0;			//红灯准备亮，绿灯灭标志
	
//	if(link_led_open && wifi_test_bink_time != 10  && pwr_led_flag == 0 )
	if(link_led_open)
	{
		if(link_led_open_long)
		{
			#ifdef ERROR_LINK_LED_ON
			LINK_LED = LED_ON;
			#else
//			if(task_run_led == 0)
			{
				LINK_LED = LED_ON;
			}
//			else
//				LINK_LED = LED_OFF;
			#endif
		}
		else if(wifi_error_flag)
		{
			link_time_error--;

			if(link_time_error >= 500)
			{
				LINK_LED = LED_ON;
			}
			else if(link_time_error == 0)
			{
				LINK_LED = LED_OFF;
				link_time_error = 600;
			}
		}
		else
		{
			link_time--;
			if(link_time == 0)
			{
				led_task_link();
				link_time = link_led_time;
			}
		}
	}
	
	//上电白灯亮6s
	if(wifi_get_mode_delay_flag < 2)
	{
		wifi_get_mode_delay_time--;
		LINK_LED = LED_ON;
		if(wifi_get_mode_delay_time == 0)
		{
			wifi_get_mode_delay_flag = 2;
		}
	}
}

#endif

