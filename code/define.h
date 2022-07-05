#ifndef _DEFINE_H_
#define _DEFINE_H_

#include "cms32f033x.h"


#define SEND_LAN_CONNECT_FUNC  	1			//发送局域网或蓝牙连接提醒功能
//######################### 项目宏 ####################################//
//#####		莱伊卡
#define LAIYK_001			1				//莱伊卡



#include "user_gpio.h"

//######################### 喂食类型 ####################################//
#define FEED_NEW 		0x1
#define FEED_AUTO 		0x1
#define FEED_OLD 		0x2
#define FEED_MANUAL 	0x3

#define FEED_AUTO_T 		0x1
#define FEED_MANUAL_T 		0x2
#define FEED_KEY_T 			0x3

//######################### 喂食错误类型 ####################################//
//涂鸦公版
#define ERROR_EMPTY				0x01		//余粮不足
#define ERROR_NOT_ENOUGH		0x01		//余粮不足
#define ERROR_IR				0x02		//红外错误(堵粮)
#define ERROR_MOTOR_TIMEOUT		0x04		//电机超时(设备异常)
#define ERROR_LOW_POWER			0x08		//电量低
#define ERROR_FEEDER_DUMP		0x10		//设备倾倒
#define ERROR_VACUUM_TIMEOUT	0x20		//真空检测异常超时
#define ERROR_DOOR_CONTROL		0x40		//仓门异常
#define ERROR_LAN_CONNT			0x80		//局域网通信 用于提示用户现在是局域网或蓝牙通信，设备离线

#define ERROR_DESICCANT_TIME	0x20		//干燥剂更换提醒
#define ERROR_KEY_COUNT			0x20		//按键次数达到最大值

#define NORMAL					0x00		//设备正常运行
#define LOW_POWER				0x03		//设备低电量
#define FEEDING					0x05		//设备正在喂食

#define VACUUM_FAULT 			0x20

//佳可协议版本
//#define ERROR_MOTOR_TIMEOUT		0x01		//电机超时
//#define ERROR_EMPTY				0x02		//无粮
//#define ERROR_IR				0x04		//红外错误
//#define ERROR_NOT_ENOUGH		0x08		//余粮不足
//#define ERROR_KEY_COUNT			0x10		//按键次数达到最大值

#ifdef IR_FOOD_TUBE
enum FOOD_STATE_E{
	LED_ENOUGH_FOOD 	= 0,		//粮桶粮食充足
    LED_NO_FOOD			= 1,		//粮桶无粮
    LED_NOT_ENOUGH_FOOD	= 2,		//粮桶粮食不多
};
#endif
//######################### 计时时间 ####################################//
#define LED_PERIOD 				40			//400ms	警报灯（红灯）闪烁时间
#define ERROR_PERIOD 			100			//1s	电机错误时间检测
#define FEED_AGAIN_PERIOD 		30000		//300s  喂食错误之后5分钟再试一次
#define MOTO_TIMEOUT			900			//9秒  	喂食电机检测到位超时时间
#define LEDTIME_ALL_TIME		30010		//红灯闪烁5分钟停止（多出来的10是确保LED灯最后是灭的状态）

#define  GET_TIME_START		300				//获取时间延时	3秒
#define  GET_TIME_STOP   	30000			//获取时间间隔	5分钟

//######################### 灯状态 ####################################//
#ifdef LED_HIGH_ON				//灯高电平亮
#define LED_OFF  				0			//
#define LED_ON   				1
#else
#define LED_OFF  				1			//
#define LED_ON   				0
#endif

#define VACUUM			0
#define NO_VACUUM		1

#define DOOR_CLOSE			0
#define DOOR_OPEN			1

//######################### 录用控制 ####################################//
#ifdef LOCAL_RECORD_LOW
#define RECORD_E					(RECORD = 0)
#define RECORD_END_E				(RECORD = 1)
#define RECORD_PLAY_E				(SPK_EN = 0)
#define RECORD_PLAY_END_E			(SPK_EN = 1)

#define GET_RECORD_E				(RECORD == 0)
#define GET_RECORD_END_E			(RECORD == 1)
#define GET_RECORD_PLAY_E			(SPK_EN == 0)
#define GET_RECORD_PLAY_END_E		(SPK_EN == 1)

#define RESET_RECORD_TIME		130
#else
#define RECORD_E				(RECORD = 1)
#define RECORD_END_E			(RECORD = 0)
#define RECORD_PLAY_E			(SPK_EN = 1)
#define RECORD_PLAY_END_E		(SPK_EN = 0)

#define GET_RECORD_E				(RECORD == 1)
#define GET_RECORD_END_E			(RECORD == 0)
#define GET_RECORD_PLAY_E			(SPK_EN == 1)
#define GET_RECORD_PLAY_END_E		(SPK_EN == 0)

#define RESET_RECORD_TIME		40
#endif

//######################### 杂项 ####################################//
#define FEED_MAX_NUM 			10 			//定时喂食最大条数
#define OFF_RECORD_MAX 			30 			//最大存储离线喂食记录条数
#define FLASH_KEEP_VAL 			121 		//flash存储标识号

//######################### 电池电量 ####################################//
#define BATTERY_LOW 			3.3 		//电池低电压电压值（低于这个值就认为低电压）
#define BATTERY_LOWEST 			3.1 		//最低工作电压
#define BATTERY_FULL 			4.5 		//满电池电压
#define BATTERY_LOW_PC 			50 			//电池低电压百分比（低于这个值就认为低电量）


//######################### 电机功能定义 ####################################//
enum DOOR_CONTROL_E
{
	DOOR_PULL		= 0x1,			//推门
	DOOR_PUSH   	= 0x2,			//拉门
	DOOR_STOP	   	= 0x3			//停止
};
enum MOTOR_CONTROL_E
{
	MOTOR_PULL		= 0x4,			//电机正转
	MOTOR_PUSH   	= 0x5,			//电机反转
	MOTOR_STOP	   	= 0x6			//电机停止
};

//##########################################################################//
//######################  flash存储内容   ##################################//
//##########################################################################//
#ifdef OFFLINE_RECORD
struct offline_record_buf{
	u8 year_h;
	u8 year_L;
	u8 month;
	u8 day;
	u8 hour;
	u8 minute;
	
	u8 weight;	
};
#endif

typedef struct {
	uint8_t 	val;
	uint8_t 	led_switch;							//指示灯开关
	uint8_t 	food_led_switch;					//氛围灯开关
	uint8_t 	lock_key;							//按键锁标志
	#ifdef TY_DP_RECORD
	uint8_t	record_time;						//录音时长
	uint8_t	voice_times;						//录音播放次数
	uint8_t	record_state;						//录音状态
	#endif
	#ifdef TY_DP_DESICCANT
	uint8_t	remind_time;						//干燥剂提醒时间
	uint8_t	remaining_time;						//干燥剂剩余时间
	#endif
	#ifdef OFFLINE_RECORD
	uint8_t	record_buf_num;						//离线喂食记录条数
	#endif
} FLASH_FUNC_E;


struct feed_infos {
	u8 week;
	u8 hour;
	u8 minute;
	
	u8 weight;
	
	u8 allow_feed;
};

typedef struct {
	u8 gpio_group;
	u8 gpio_pin;

	
}PWM_INIT_INFO_E;

typedef struct {
	uint8_t val;
	uint8_t tuya_feed_num;							//涂鸦喂食现有餐数
	struct feed_infos feed_info[FEED_MAX_NUM];		//喂食列表
}FLASH_FEED_INFO_E;


extern FLASH_FUNC_E 			func_flag;			//存储flash的标志变量
extern FLASH_FEED_INFO_E		flash_feed_info;	//喂食计划
//	struct offline_record_buf record_buf[OFF_RECORD_MAX];			//离线喂食记录存储
//##########################################################################//
//##########################################################################//
//##########################################################################//

extern uint32_t Systemclock;		//系统时钟
extern u8 net_flag;		
extern u8 send_feed_flag;

#ifdef TY_DP_RECORD		
extern u8 voice_time_time;			//录音播放次数计数
extern u8 record_change_flag;		//录音时长改变需要保存flash标志
#endif

void Jump(void);
extern void send_JK_feed_info(unsigned char dpid,unsigned char value_h,unsigned short value_l);

extern u8 WDT_time;		
extern u8 com0_flag;
extern u8 com1_flag;
extern u8 com0_timeout;
extern u8 com1_timeout;

extern u8 lock_key_flag;				//锁键触发标志
extern u8 get_time_error;				//获取时间标志（1：获取时间 0：获取时间结果返回成功 2：获取时间超时失败，重启WiFi模块）

//##########################################################################//
//##########################################################################//
//##########################################################################//


#include "system.h"
//#include "intrins.h"
#include "string.h"
#include "stdio.h"

#include "led.h"
#include "adc1.h"
#include "rtc.h"
#include "motor.h"
#include "gpio.h"
#include "fmc.h"
#include "uart.h"

#include "user_gpio.h"
#include "user_time.h"
#include "user_uart.h"
#include "user_flash.h"

#include "tuya_wifi.h"
#include "feed_process.h"

//TUYA SDK
#include "wifi.h"
#include "mcu_api.h"
#include  "protocol.h"
#include  "system.h"


#endif

