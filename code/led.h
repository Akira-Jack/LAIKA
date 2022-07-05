#ifndef	__LED_H
#define	__LED_H

#include "cms32f033x.h"

#define PWM_CHANNEL_0		0x12;			
#define PWM_CHANNEL_1		0x13;			
#define PWM_CHANNEL_2		0x14;			
#define PWM_CHANNEL_3		0x15;			
#define PWM_CHANNEL_4		0x16;			
#define PWM_CHANNEL_5		0x17;			

#define PWMP0				255		//周期
#define PWMP2				255
#define PWMP4				255
#define PWMP1				255		//周期
#define PWMP3				255
#define PWMP5				255

extern u8 task_run_led;//led任务
extern u8 task_delay_led;
//错误闪烁led任务
extern u8 task_delay_error;
//电机任务
extern u8 task_run_motor;
extern u16 task_feed_again;

extern u8 ledtime_flag;					//红灯闪烁1分钟关闭标志位
extern u8 led_task_static;				//红灯闪烁状态


extern void led_task(void);
extern void error_task(void);
extern void led_task_link(void);

extern void stop_color(void);
extern void color_set(u8 R, u8 G, u8 B);
extern void PWMInit(void);
extern void food_det_isr(void);

extern void buzzer_en_func(u8 en);
extern void device_state(void);

#endif
