#include  "define.h"
#include  "rtc.h"

struct rtc_time_ascii rtc_date;

u8 task_delay_rtc = RTC_PERIOD;
u8 display_time = 0;				//0不显示；1显示
u8 get_time_flag = 0;				//获取时间（0：从未获取时间 1：获取时间 2：获取时间等待间隔）

static u8 rtc_days_in_month[] =
{
	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};


static u8 is_leap_year(u16 year)
{
	return (! (year % 4) && (year % 100)) || ! (year % 400);
}


/*
 * The number of days in the month.
 */
static u8 rtc_month_days(u8 month, u16 year)
{
	return rtc_days_in_month[month - 1] + (is_leap_year(year) && ((month - 1) == 1));
}


void rtc_init(void)
{
	memset(&rtc_date,0,sizeof(struct rtc_time_ascii));
}


void rtc_task(void)
{
	rtc_date.second++;
	display_time = 1;
		
	if(rtc_date.second >= 60)
	{
		rtc_date.second = 0;
		rtc_date.minute++;
		#ifdef SEND_LAN_CONNECT_FUNC				////发送局域网或蓝牙连接提醒功能
		if(send_error_again == 1)
			send_error_again = 2;
		#endif

		if(rtc_date.minute >= 60)
		{
			rtc_date.minute = 0;
			rtc_date.hour++;
			

			if(rtc_date.hour >= 24)
			{
				u8 month_days;

				rtc_date.hour = 0;
				rtc_date.mday++;
				rtc_date.wday++;
				if(rtc_date.wday > 7)
				{
					rtc_date.wday = 1;
				}
				
				#ifdef TY_DP_DESICCANT
				if(func_flag.remaining_time > 0)
				{
					func_flag.remaining_time--;
					remaining_change = 1;
				}
				#endif
				
				month_days = rtc_month_days(rtc_date.month, rtc_date.year + 1960);

				if(rtc_date.mday > month_days)
				{
					rtc_date.mday = 1;
					rtc_date.month++;

					if(rtc_date.month > 12)
					{
						rtc_date.month = 1;
						rtc_date.year++;
					}
				}
			}
		}
	}
}


//int Get_weeks(int y, u8 m, u8 d)
//{
//	int c, w;

//	// y=18, m=8, d=7;
//	if(m == 1 || m == 2)
//	{ //判断月份是否为1或2
//		y--;
//		m += 12;
//	}
//	c = y / 100;
//	y = y - c * 100;
//	w = (c / 4) - 2 * c + (y + y / 4) + (13 * (m + 1) / 5) +d - 1;

//	while(w < 0) w += 7;
//	w %= 7;

//	//	if (w == 0)  ;//printf ("星期日");
//	//
//	//	else ;//printf ("星期%d", w);
//	return w;
//}


void printf_rtc(void)
{
	printf("Set DATE: %d-%d-%d  ",rtc_date.year, rtc_date.month ,rtc_date.mday);
	printf("  TIME: %d:%d:%d\r\n", rtc_date.hour, rtc_date.minute ,rtc_date.second);
//	printf("	week: %d \r\n", rtc_date.wday);
//	printf("KEY_TIME: %d \r\n", key_time);

}


