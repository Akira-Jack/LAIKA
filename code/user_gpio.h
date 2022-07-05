#ifndef _USER_GPIO_H_
#define _USER_GPIO_H_

#include "cms32f033x.h"
#include "define.h"


//######################################################################//
//######################### 共用定义 ####################################//
extern void GPIO_Config(void);
//sbit SWDIO 			= P3^5;			//调试烧录串口（TXD）
//sbit SWCLK 			= P2^1;
//sbit MCU_TXD 		= P2^5;			//上位机通信串口
//sbit MCU_RXD 		= P2^6;
//######################################################################//
//######################################################################//
//######################################################################//

//######################### DU4L_WS_W03 ####################################//
#if defined LAIYK_001
#define LAIYK_001					1
#ifdef LAIYK_001
#define VERSION_F			"CF033-LAIYK_35.1.0"			//记录版本号
#define	VERSION_DATA		"20211220"				//版本更新时间
#define PRODUCT_KEY 		"trnkjvvd8b1qknnl"    	//开发平台创建产品后生成的16位字符产品唯一标识
#define MCU_VER 			"35.1.0"         		//用户的软件版本,用于MCU固件升级,MCU升级版本需修改

#endif

#define RTC_TIME			1		//时钟计时
#define MOTOR_REVERSE		1		//电机反转功能
#define NO_RECORD_FUNC		1		//没有录音功能
#define BATTERY_DETECT		1		//电池电量检测检测功能

#define LINK_LED_FUNC		1		//网络灯功能
#define KEY_BUZZER_FUNC		1		//按键蜂鸣器响功能
#define ONE_KEY_RECORD      1		//单按键录音功能 
#define SFN_BEEP			1		//无源蜂鸣器
#define DOOR_CONTROL		1		//门仓控制

#define IR_FEED_TUBE		1		//出食对射管
#define IR_FOOD_TUBE		1		//粮仓余粮检测对射管
#define MOTOR_DET_ADDR		1		//电机控制到位停止位置（选中就是松开停止）

#define ADC_CHANNEL			ADC1_CH_0		//ADC通道选择

#define DC_DET				GPIO0->DI_f.P1			//外电检测
#define BAT_DET				GPIO0->DI_f.P0			//电池电量ADC检测口

#define WIFI_PWR			GPIO4->DO_f.P0			//wifi电源控制
#define BUZZER_EN			GPIO0->DO_f.P4			//蜂鸣器控制脚
#define SWITCH_DRIVER		GPIO0->DO_f.P5			//电磁阀开关
#define MOTOR_DRIVER		GPIO0->DO_f.P6			//真空泵开关
#define IR_LED_CTRL			GPIO0->DO_f.P7			//红外对管检测（所有对管）

#define MOTOR_FEED_G		GPIO1->DO_f.P0			//喂食电机正转
#define MOTOR_FEED_B		GPIO1->DO_f.P2			//喂食电机反转
#define MOTOR_DOOR_G		GPIO1->DO_f.P4			//门仓电机正转
#define MOTOR_DOOR_B		GPIO1->DO_f.P3			//门仓电机反转

#define LED_DRIVER			GPIO3->DO_f.P6			//氛围灯
#define BAT_LED				GPIO1->DO_f.P5			//外电指示灯
#define LINK_LED			GPIO4->DO_f.P3			//网络指示灯
#define PUMP_LED			GPIO3->DO_f.P0			//气泵工作指示灯
#define FEED_LED			GPIO4->DO_f.P4			//卡粮指示灯
#define MOTOR_LED			GPIO3->DO_f.P1			//堵粮指示灯
#define FOOD_LED			GPIO3->DO_f.P2			//余粮指示灯

#define MOTOR_DET			GPIO2->DI_f.P1			//喂食到位
#define FEED_DET			GPIO3->DI_f.P4			//堵粮对管到位
#define FOOD_DET			GPIO3->DI_f.P5			//余粮对管到位
#define DOOR_DET_P			GPIO2->DI_f.P3			//门仓到位P
#define DOOR_DET_N			GPIO2->DI_f.P2			//门仓到位N
#define AIR_DET				GPIO2->DI_f.P4			//气泵到位

#define FEED_KEY			GPIO2->DI_f.P6			//喂食按键
#define RESET_KEY			GPIO2->DI_f.P5			//复位按键


#define FEED_KEY_INT_PIN			0x10		//喂食按键中断PIN脚
//#define FEED_TUBE_INT_PIN			0x08		//落粮对管中断PIN脚
//#define LOCK_KEY_INT_PIN				0x10		//锁键按键PIN脚-P1
#define FEED_KEY_INT_EN			(P0EXTIE |=  FEED_KEY_INT_PIN)		//喂食按键中断使能
#define RECORD_KEY_INT_PIN			0x80		//喂食按键中断PIN脚
#define RECORD_KEY_INT_EN		(P1EXTIE |=  RECORD_KEY_INT_PIN)		//喂食按键中断使能

#define MOTOR_DELAY			delay_ms(0)			//喂食结束延时停止电机时间

#endif

//######################### AI_B20 ####################################//
//######################### AI_B20 ####################################//
//######################### AI_B20 ####################################//



//######################################################################//
//######################### 功能定义 ####################################//
//#define IR_FEED_TUBE			1		//出食对射管
//#define IR_CHECK_FOOD			1		//出食对射管检测落粮
//#define IR_FOOD_TUBE			1		//粮仓余粮检测对射管
//#define IR_FOOD_TUBE_TWO		1		//2对粮仓余粮检测对射管
//#define IR_FOOD_TUBE_NOW		1		//粮仓余粮检测马上闪红灯
//#define FOOD_TUBE_LED			1		//粮仓余粮提示灯
//#define BATTERY_DETECT		1		//电池电量检测检测功能
//#define BATTERY_DC_DETECT		1		//没有电池供电
//#define DC_POWER_CONTROL		1		//电池供电断WiFi电源
//#define RTC_TIME				1		//时钟计时
//#define MOTOR_DET_ADDR		1		//电机控制到位停止位置（选中就是松开停止）
//#define MOTOR_REVERSE			1		//电机反转功能
//#define DOOR_CONTROL			1		//门仓控制
//#define LOCAL_RECORD			1		//本地录音功能
//#define LOCAL_RECORD_LOW		1		//本地录音功能:新IC，低电平触发
//#define TY_DP_RECORD			1		//涂鸦DP点录音功能
//#define PLAY_KEY_RECORD		1		//短按录音播放录音功能
//#define ONE_KEY_RECORD		1		//单按键录音功能 
//#define NO_RECORD_FUNC		1		//没有录音功能
//#define LOCK_KEY_E			1		//喂食按键作为锁键功能
//#define LOCK_KEY_KEY_E		1		//锁键按键作为锁键功能
//#define RELOCK_KEY_KEY_E		1		//解锁键按键作为解锁功能
//#define TY_DP_LOCK			1		//涂鸦DP点锁键功能
//#define THREE_KEY_FUNC		1		//3按键功能
//#define LS_FEED_INFO			1		//林晟默认喂食计划
//#define LED_HIGH_ON			1		//灯高电平亮
//#define MOTOR_BU_FUNC			1		//云台步进电机控制功能
//#define MOTOR_BU_FUNC			1		//云台步进电机控制功能
//#define FEEDED_ALLOW_ERROR	1		//喂食后才允许亮红灯
//#define ERROR_LIGHT_ONE_MIN	1		//喂食异常1分钟后关闭红灯
//#define KEY_BUZZER_FUNC		1		//按键蜂鸣器响功能
//#define POWER_BEEP			1		//有源蜂鸣器
//#define SFN_BEEP				1		//无源蜂鸣器
//#define ERROR_LINK_LED_ON		1		//网络灯在错误红灯闪烁的时候还亮起
//#define MACHINE_DUMP_E		1		//检测设备倾倒功能
//#define OFFLINE_RECORD		1		//离线喂食记录
//#define LINK_LED_FUNC			1		//网络灯功能

//#define LOCK_KEY_E_P0			1		//锁键按键作为锁键功能-P0脚
//#define LOCK_KEY_E_P1			1		//锁键按键作为锁键功能-P1脚
//#define LOCK_KEY_E_P2			1		//锁键按键作为锁键功能-P2脚
//#define LOCK_KEY_E_P3			1		//锁键按键作为锁键功能-P3脚
//#define FEED_KEY_E_P0			1		//喂食按键功能-P0脚
//#define FEED_KEY_E_P1			1		//喂食按键功能-P1脚
//#define FEED_KEY_E_P2			1		//喂食按键功能-P2脚
//#define FEED_KEY_E_P3			1		//喂食按键功能-P3脚
//#define RECORD_KEY_E_P0		1		//录音按键中断-P0脚
//#define RECORD_KEY_E_P1		1		//录音按键中断-P1脚
//#define RECORD_KEY_E_P2		1		//录音按键中断-P2脚
//#define RECORD_KEY_E_P3		1		//录音按键中断-P3脚


//#define FEED_KEY_INT_PIN			0x10		//喂食按键中断PIN脚
//#define FEED_TUBE_INT_PIN			0x08		//落粮对管中断PIN脚
//#define LOCK_KEY_INT_PIN			0x80		//锁键按键中断PIN脚

//#define FEED_KEY_INT_EN			(P0EXTIE |=  FEED_KEY_INT_PIN)		//喂食按键中断使能
//#define RECORD_KEY_INT_EN			1		//录音按键中断使能

//#define FEED_INFO_ONE_DEL			1				//喂食计划仅一次，喂食完删除
//#define NEW_FEED_REPORT			1				//新增喂食结果推送DP
//#define TY_DP_DESICCANT			1				//涂鸦DP点干燥剂提醒功能

//#########################   引脚定义  ####################################//
//##########################################################################//
//sbit MOTOR_DOOR_G 	= P1^7;			//门仓电机推出
//sbit MOTOR_DOOR_B 	= P1^6;			//门仓电机拉回
//sbit MOTOR_FEED_G 	= P1^4;			//喂食电机正转
//sbit MOTOR_FEED_B 	= P1^5;			//喂食电机反转
//sbit MOTOR_DET 		= P2^4; 		//喂食行程开关检测   

//sbit IR_DET_F1 		= P3^1;			//余粮桶余粮检测对管到位1
//sbit IR_DET_F2		= P3^2;			//余粮桶余粮检测对管到位2

//sbit IR_DET 		= P0^3;			//红外对管检测管
//sbit IR_DET_D 		= P0^5;			//门仓关门到位（关门成功det脚触发）
//sbit IR_DET_D2 		= P0^6;			//门仓开门成功到位触发
//sbit IR_LED 		= P0^3;			//余粮和喂食红外对管灯

//sbit PWR_LED 		= P3^6;			//电源灯
//sbit LINK_LED 		= P3^2;			//网络状态灯

//sbit FEED_KEY 		= P3^1;			//按键喂食
//sbit RESET_KEY 		= P1^7;			//长按：复位
//sbit RECODE_KEY 	= P1^6;			//录音

//sbit DC_DET 		= P0^0;			//外电检测
//sbit BAT_DET 		= P0^1;			//电池电量ADC检测口

//sbit PLAYE	 		= P3^0;			//播音脚
//sbit RECORD 		= P3^1;			//录音脚

//sbit WIFI_PWR_EN	= P2^3;			//wifi电源控制

//sbit RGB_PWR_CTRL 	= P2^3;			//RGB灯电源控制
//sbit DATA 			= P2^2;			//RGB灯控制

//sbit SWDIO 			= P3^5;			//调试烧录串口（TXD）
//sbit SWCLK 			= P2^1;

//sbit MCU_TXD 		= P2^5;			//上位机通信串口
//sbit MCU_RXD 		= P2^6;
//##########################################################################//
//##########################################################################//


#ifndef RESET_KEY_TIME_U
#define RESET_KEY_TIME_U		700				//复位键触发复位长按时间
#endif

#ifndef RECORD_TIME_MAX_U
#define RECORD_TIME_MAX_U		1000				//录音超时时间
#endif






#endif

