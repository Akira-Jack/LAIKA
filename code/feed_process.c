#include "define.h"
#include "feed_process.h"
#include "mcu_api.h"

u8 cancel_send_feed_error = 0;
u8 rtc_date_change  = 0;

//u8 check_feed_info(u8 *buf, u8 buf_len)
//{
//	u8 i = 0;
//	u16 check_num = 0;
//	
//	for(i = 0; i < buf_len; i++)
//	{
//		check_num = check_num + buf[i];
//		check_num = check_num & 0x00ff;
//	}
//	
//	return check_num;
//}

void send_feed_state(u8 feed_state)
{
	switch(feed_state)
	{
		case NORMAL:
			mcu_dp_enum_update(DPID_FEED_STATE,0);			//设备正常运行
			break;
		
		case ERROR_EMPTY:
			mcu_dp_enum_update(DPID_FEED_STATE,1);			//余粮不足
			break;

		#ifndef SEND_LAN_CONNECT_FUNC  			//发送局域网或蓝牙连接提醒功能
		case ERROR_IR:
			mcu_dp_enum_update(DPID_FEED_STATE,2);			//红外错误(堵粮)
			break;
		#endif

		case ERROR_MOTOR_TIMEOUT:
			mcu_dp_enum_update(DPID_FEED_STATE,3);			//电机超时
			break;

		case FEEDING:
			mcu_dp_enum_update(DPID_FEED_STATE,4);			//设备正在喂食
			break;
		
//		case LOW_POWER:
//			mcu_dp_enum_update(DPID_FEED_STATE,5);			//设备低电量
//			break;
		
	}
}

#ifdef OFFLINE_RECORD
//void keep_offline_record(u8 feed_weight)
//{
//	record_buf[func_flag.record_buf_num].year_h 	= (2000 + rtc_date.year) / 100;
//	record_buf[func_flag.record_buf_num].year_L 	= (2000 + rtc_date.year) % 100;
//	record_buf[func_flag.record_buf_num].month 		= rtc_date.month;
//	record_buf[func_flag.record_buf_num].day 		= rtc_date.mday;
//	record_buf[func_flag.record_buf_num].hour 		= rtc_date.hour;
//	record_buf[func_flag.record_buf_num].minute 	= rtc_date.minute;
//	record_buf[func_flag.record_buf_num].weight 	= feed_weight;

//	func_flag.record_buf_num++;
//	
//	user_write_flash(FLASH_FUNC_ADDR, (uint8_t *) &func_flag, sizeof(FLASH_FUNC_E));	
//}
//void send_offline_buf()
//{
//	if(func_flag.record_buf_num > 0 && net_flag == 3)
//	{
//		mcu_dp_raw_update(DPID_OFFLINE_FEED_RECORD, func_flag.record_buf, func_flag.record_buf_num * sizeof(struct offline_record_buf)); //RAW型数据上报;
//		
//		func_flag.record_buf_num = 0;
//		my_memset(&func_flag.record_buf, 0, sizeof(struct offline_record_buf));
//		user_write_flash(FLASH_FUNC_ADDR, (uint8_t *) &func_flag, sizeof(FLASH_FUNC_E));
//	}
//}
#endif

void send_feed_info()
{
	u8 keep_buf[64];
	u8 a;
	
	IAP_read_user(FLASH_FEED_INFO_ADDR, (uint8_t *) &flash_feed_info, sizeof(FLASH_FEED_INFO_E));
		
	if(flash_feed_info.tuya_feed_num > (FEED_MAX_NUM * sizeof(struct feed_infos)))
	{
		flash_feed_info.tuya_feed_num = FEED_MAX_NUM * sizeof(struct feed_infos);
	}
	
	for(a = 0; a < (flash_feed_info.tuya_feed_num / sizeof(struct feed_infos)); a++)
	{
		memcpy(keep_buf + (a * 5), (u8 *) &flash_feed_info.feed_info[a], sizeof(struct feed_infos));
	}
		
	if(flash_feed_info.tuya_feed_num <= 1)
		mcu_dp_raw_update(DPID_MEAL_PLAN,0,1); //RAW型数据上报;
	else
		mcu_dp_raw_update(DPID_MEAL_PLAN,keep_buf,flash_feed_info.tuya_feed_num); //RAW型数据上报;	
}


//执行喂食：按键手动 手动 定时
char feed_run(u8 dat, struct feed_infos * info, u8 feed_run_typ)
{
	u8 weight;
	u8 weight_already = 0;
	u8 res;
	#ifdef NEW_FEED_REPORT
	u16 feed_report = 0;
	#endif
	
	weight = info->weight;
	
	if(weight == 0)		//喂食份数为0，退出喂食函数
	{
		return 0;
	}
		
	send_feed_state(FEEDING);			//正在喂食
	if(AIR_DET == VACUUM)			//真空
		air_escape();					//放气	

	#ifdef DOOR_CONTROL
	motor_door_open();					//开仓门	
	#endif
		
	//启动电机
	weight_already = motor(weight, &res);

	printf("feeds = %d		already = %d\r\n",weight, weight_already);

	mcu_dp_value_update(DPID_FEED_REPORT,weight_already);
	mcu_dp_fault_update(DPID_FAULT,res);
	if(res > 0)
	{
		cancel_send_feed_error = 1;
	}
	
	#ifdef NEW_FEED_REPORT
	feed_report = feed_report | (weight_already << 8);
	feed_report = feed_report | res;
//	printf("report_feed:%ld \r\n",feed_report);	
	send_JK_feed_info(DPID_FEED_REPORT_JK, feed_run_typ, feed_report);
	#endif
	
	#ifdef OFFLINE_RECORD
//	if(net_flag != 3)
//		keep_offline_record(weight_already);
	#endif
	
	info->weight = (u8) (weight - weight_already);
		
//	send_feed_state(res);			//等待状态
	send_feed_state(0);			//等待状态
	
	return res;
}

void check_min_sys_time(u8 feed_min, u8 stop_time)
{
	u8 min, k;
	
	min = rtc_date.minute;
	for(k = 0; k < stop_time; k++)
	{
		if(min == feed_min)
		{
			rtc_date_change = 2;		
			return;
		}
		
		min++;
		if(min >= 60)
			min = 0;
	}
	
	min = rtc_date.minute;
	for(k = 0; k < stop_time; k++)
	{
		if(min == feed_min)
		{
			rtc_date_change = 2;		
			return;
		}
		
		if(min <= 0)
			min = 60;		

		min--;
	}
}

//扫描eeprom判断是否需要执行定时喂食
void feed_scan(struct feed_infos * info)
{
	u8 i;

//	week = Get_weeks(rtc_date.year + 1960, rtc_date.month, rtc_date.mday);

	IAP_read_user(FLASH_FEED_INFO_ADDR, (uint8_t *) &flash_feed_info, sizeof(FLASH_FEED_INFO_E));

	printf_feed_info();
	printf_rtc();
	for(i = 0; i < FEED_MAX_NUM; i++)
	{
		//如果重量为0（数据有问题）
		if(flash_feed_info.feed_info[i].weight == 0)
		{
//			printf("flash_feed_info.feed_info[i].weight == 0x00 \r\n");
			continue;
		}

		if(flash_feed_info.feed_info[i].allow_feed != 0x01)
		{
//			printf("flash_feed_info.feed_info[i].allow_feed != 0x01 \r\n");
			continue;
		}
		else if((flash_feed_info.feed_info[i].week & ((1 << (7 - rtc_date.wday)) & 0x00FF)) || flash_feed_info.feed_info[i].week == 0)			//星期判断
		{ 
			if(flash_feed_info.feed_info[i].hour == rtc_date.hour)
			{
				//喂食计划前后5分钟不同步时间
				check_min_sys_time(flash_feed_info.feed_info[i].minute, 5);

//				printf("minute =%d-%d  \r\n",flash_feed_info.feed_info[i].minute, rtc_date.minute);
				if(flash_feed_info.feed_info[i].minute == rtc_date.minute)
				{
					*info = flash_feed_info.feed_info[i];
					feed_required = FEED_AUTO;
					feed_type = FEED_AUTO_T;
					
					if(flash_feed_info.feed_info[i].week == 0)		//表示仅限一次，喂食完把喂食开关置0关闭
					{
						flash_feed_info.feed_info[i].allow_feed = 0;
						
						#ifdef FEED_INFO_ONE_DEL
						u8 a;
						my_memset(&flash_feed_info.feed_info[i], 0, sizeof(struct feed_infos));
						
						for(a = i; a < flash_feed_info.tuya_feed_num; a++)
						{
							memcpy(&flash_feed_info.feed_info[a], &flash_feed_info.feed_info[a + 1], sizeof(struct feed_infos));
						}
						flash_feed_info.tuya_feed_num--;
						#endif
						
						user_write_flash(FLASH_FEED_INFO_ADDR,(u8 *) &flash_feed_info, sizeof(FLASH_FEED_INFO_E));
						
						send_feed_info();
					}
//					printf("goto autofeed: week:\r\n");
					break;
				}
			}
		}
	}
	if(rtc_date_change == 2)
		rtc_date_change = 1;
	else
		rtc_date_change = 0;	
}

