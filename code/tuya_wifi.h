#ifndef	_TUYA_WIFI_H_
#define	_TUYA_WIFI_H_

#include "define.h"

extern u8 	second;			//秒
extern u8 	minute;
extern u8 	hour;
extern u8 	delay_flag;		//延时标志位
extern u16 	time_delay;		//延时时间对比
//extern u8 	key_shake_flag;			//按键消抖成功标志

//extern u16 RES_KEY_time;

extern u8 RECODE_KEY_flag;			//录音请求标志

extern u8  SPK_EN_flag;				//播音标志
extern u8 	key_one_flag;			//按键一次标志位
extern u8	feed_one_flag_now;		//手动喂食一次标志位
extern u8 	key_time_flag;			//开始记录按键时间标志
extern u8 	key_flag;				//按键输入标志位
extern u8 	key_mode;				//按键响应模式 0：正常模式	1：手动喂食	2：复位
extern int 	key_time;				//按键时间	超过5秒复位
extern u8 	lock_key_task;			//锁键成功闪烁标志
extern u8 	reset_flag;				//复位标志位
extern u8 	set_AP_mode;			//设置AP模式
extern u8 	buzzer_flag;			//蜂鸣器响标志

extern u8 spk_reset_flag;			//清录音标志
extern u8 record_flag;				//准备录音标志

extern u8	battery_flag;				//电池标志
extern u8	battery_empty_flag;			//电量不足标志
extern u8	battery_empty_result;		//电量不足推送标志
extern s8 	battery_percent;			//电池电量百分比


extern u8 buzzer_enable_flag;

extern u8 atmosphere_lamp_state;


extern void battery_isr(void);

extern void link_led_isr(void);

/////////////////////////////////////////////////////////////
extern u16 link_led_time;				//网络灯闪烁时间
extern u8 wifi_error_flag;				//网络灯AP模式
extern u8 link_led_open;				//开启网络灯闪烁
extern u8 link_led_open_long;			//网络灯常亮

extern u8 wifi_test_flag;				//wifi测试模式标志
//extern bit wifi_strength_poor;			//WiFi信号强度差标志
//extern bit wifi_normal;					//wifi正常标志
//extern bit wifi_no_ssid;				//模块未扫描到指定路由器
//extern bit wifi_no_permission;			//模块未授权产测模式
extern u8 	wifi_test_bink_time;		//wifi测试模式闪烁次数
extern u8 wifi_tset;					//WiFi测试模式间隔发送标志
extern u8 	wifi_test_time;				//wifi测试次数
extern u8  wifi_test_start_flag;		//上电进入WiFi测试模式延时时间

extern u8 wifi_get_mode_delay_flag;		//上电获取WiFi模式延时标志

extern u8 key_dowm_time;				//按键按下次数
extern u8 record_key_flag;				//录音按键按下标志
extern u8 remaining_change; 			//干燥剂剩余时间更新标志




extern void link_led_control(void);

extern void feed_key_isr(void);
extern void record_key_isr(void);
extern void speak_reset_isr(void);
extern void buzzer_func_isr(void);
extern void desiccant_pull(void);


#endif

