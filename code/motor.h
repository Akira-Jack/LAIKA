#ifndef __MOTOR_H
#define __MOTOR_H

#include "cms32f033x.h"
#include "define.h"


extern u32 motor(u32 count, u8 * err_type);	//喂食函数
extern u16 motor_door(u8 open);
extern void feed_scan(struct feed_infos * info);

extern uint8_t motor_door_close(void);
extern uint8_t motor_door_open(void);


extern void air_escape(void); //放气

extern void air_exhaust(void); //抽气



extern struct feed_infos  feed_info;						//不能用xdata修饰，不然收到数据混乱

extern u8 feed_error;				//喂食错误 1(堵粮) 2（设备异常） 4（无粮）
extern u8 send_error_again;			//定时发送异常标志
extern char food_count;				//红外检测粮食下落份数
extern u8 ERROR_IR_style;			//红外错误类型	
extern u8 feed_detect_sw;			////喂食完（5分钟）标志位
extern u8 led_status_allow_feed;	//喂食完成后允许状态灯开启标志
extern u8 feed_detect_again;		//红外错误：				确保是喂食错误，执行再次喂食检测
extern u32 already_number;			//已经喂食的份数
extern u32 number;					//需要喂食的份数
extern u16 task_feed_sw;			//喂食完（5分钟）计时时间清0

extern u8 feed_required;			//需要执行喂食：0(不执行) 1(执行) 2(执行)
extern u8 feed_type; 				//喂食类型：		

extern u8 door_open;				//关门错误标志（=2 关门出错，停止关门）
extern u8 food_empty_flag;			//桶内余粮状态

extern u8 reverse_flag;

#endif

