#include "cms32f033x.h"

#include "define.h"

//led任务
u8 task_run_led = 0;
u8 task_delay_led = LED_PERIOD;

//错误闪烁led任务
u8 task_delay_error = ERROR_PERIOD;

//电机任务
u8 task_run_motor = 0;				//喂食中标志位1（喂食中） 0（空闲）
u16 task_feed_again = 0;

u8 ledtime_flag = 0;				//红灯闪烁1分钟关闭标志位
u8 led_task_static = 0;				//红灯闪烁状态

void led_task(void)
{
	led_task_static = 1;
//	PWR_LED = ~PWR_LED;
}

void led_task_link(void)
{
	LINK_LED = ~LINK_LED;
}


#ifdef IR_FEED_TUBE
void error_task(void)
{
	static u8 error_ir_time = 0;
	
	//判断红外对管是否一直被阻挡（gpio引脚电平0或1）
	if(IR_LED_CTRL)
	{
		if(FEED_DET == 1)			//堵粮
		{
			error_ir_time++;
			if(error_ir_time >= 150)
			{
				if((feed_error & ERROR_IR) == 0)
				{
					buzzer_flag = 1;				//蜂鸣器响一声
				}
				
				feed_error |= ERROR_IR;
				error_ir_time = 0;
			}
		}
		else
		{
			error_ir_time = 0;
			feed_error &= ~ERROR_IR;
		}
	}
}
#endif

#ifdef IR_FOOD_TUBE
void food_det_isr(void)
{
	if(FOOD_DET == 0)						//余粮不足事件
	{
		food_empty_flag = LED_NO_FOOD;				//余粮快没了；
		if((feed_error & ERROR_NOT_ENOUGH) == 0)
		{
			buzzer_flag = 1;				//蜂鸣器响一声
		}
		feed_error |= ERROR_NOT_ENOUGH;
		
		#ifdef IR_FOOD_TUBE_NOW
		feed_error |= ERROR_NOT_ENOUGH;		
		task_run_led = 1;
		#endif
	}
	#ifdef IR_FOOD_TUBE_TWO
	else if(IR_FOOD_DET2 == 0)
	{
		food_empty_flag = LED_NOT_ENOUGH_FOOD;		//余粮不充足
		#ifdef IR_FOOD_TUBE_NOW
		feed_error |= ERROR_NOT_ENOUGH;
//		task_run_led = 1;
		#endif
	}
	#endif
	else 
	{
		food_empty_flag = LED_ENOUGH_FOOD;
		if(feed_error & ERROR_NOT_ENOUGH)
		{
			feed_error &= ~ERROR_NOT_ENOUGH;
		}
	}
	
//	if(food_empty_flag > 0)		//余粮不足时，一直开着余粮红外灯检测
//	{
//		IR_LED_CTRL = 1;
//	}
//	#ifdef IR_FOOD_TUBE_TWO
//	else if(IR_LED_CTRL && food_empty_flag == LED_ENOUGH_FOOD)
//	{
//		if(task_run_motor == 0)
//			IR_LED_CTRL = 0;
//	}	
//	#endif
}
#endif


void device_state()
{
	static u8 atmosphere_lamp_state_old = 0xff;
	static u8 air_det_old = 0xff;
	
	//氛围灯状态上报
	if(atmosphere_lamp_state != atmosphere_lamp_state_old)
	{
		atmosphere_lamp_state_old = atmosphere_lamp_state;
		
		mcu_dp_bool_update(DPID_ATMOSPHERE_STATE, atmosphere_lamp_state);
	}
	
	//真空状态上报
	if(air_det_old != AIR_DET)
	{
		air_det_old = AIR_DET;
		
		mcu_dp_bool_update(DPID_VACUUM_STATE, AIR_DET);
	}	
}

#ifdef SFN_BEEP
void PWMInit(void)
{	
	/*
	(1)设置EPWM运行模式
	*/
	EPWM_ConfigRunMode(  EPWM_COUNT_DOWN 		| 			/*向下计数模式(边沿对齐)*/
						 EPWM_OCU_SYMMETRIC 	|			/*对称模式*/
						 EPWM_WFG_INDEPENDENT   |			/*独立模式*/
						 EPWM_OC_INDEPENDENT);				/*独立输出模式*/
	/*
	(2)设置EPWM时钟周期
	*/
	SYS_EnablePeripheralClk(SYS_CLK_EPWM_MSK);				/*开启EPWM时钟*/
	
	EPWM_ConfigChannelClk(EPWM1, EPWM_CLK_DIV_1);			/*设置EPWM时钟为APB时钟的1分频*/
	EPWM_ConfigChannelPeriod(EPWM1,  17778);					/*EPWM0通道的周期 = 2400/Fepwm = 50us*/
	EPWM_ConfigChannelSymDuty(EPWM1, 8889);					/*EPWM0通道的占空比 = 50%*/
	/*
	(3)设置EPWM反向输出
	*/
	EPWM_DisableReverseOutput(EPWM_CH_1_MSK);				/*关闭EPWM0的反相输出*/

	/*
	(4)设置EPWM加载方式
	*/
	EPWM_EnableAutoLoadMode(EPWM_CH_1_MSK);						/*设置为自动加载模式*/
	EPWM_ConfigLoadAndIntMode(EPWM1, EPWM_EACH_PERIOD_ZERO);	/*加载点设置为每个周期点和零点，并产生周期与零点中断*/
	/*
	边沿对齐(向下计数)模式: 
	(1)EPWM_EACH_PERIOD_ZERO: 每个零点或周期点均加载与产生零点与周期点中断标志(不影响比较中断)
	(2)EPWM_EACH_ZERO:		 每隔两个零点加载一次，并产生零点中断。(不产生周期中断，不影响比较中断)
	(3)EPWM_FIRST_ZERO_NEXT_PERIOD:		 每隔三个零点(周期)加载一次，并产生零点、周期中断。(不影响比较中断)	
	(4)EPWM_EVERY_TWO_ZERO:		 每隔四个零点加载一次，并产生零点中断。(不产生周期中断，不影响比较中断)				

	中心对齐(上下计数)模式：
	(1)EPWM_EACH_PERIOD_ZERO: 每个零点或周期点均加载与产生零点与周期点中断标志(不影响比较中断)
	(2)EPWM_EACH_ZERO:		 每个零点加载与产生零点中断标志(不产生周期中断，不影响比较中断)
	(3)EPWM_FIRST_ZERO_NEXT_PERIOD:		 第一个零点与下一个周期点交替加载与产生零点与周期点中断标志(每隔3次零点(周期)产生一次零点（周期）中断)(不影响比较中断)	
	(4)EPWM_EVERY_TWO_ZERO:		 每隔2个零点加载一次，并产生零点中断。(不产生周期中断，不影响比较中断)		
	*/
	
//	/*
//	(5)设置中断
//	*/
//	EPWM_EnablePeriodInt( EPWM_CH_0_MSK);				/*开启周期点中断*/
//	EPWM_EnableDownCmpInt( EPWM_CH_0_MSK);				/*开启向下比较点中断*/
//	EPWM_EnableZeroInt( EPWM_CH_0_MSK);					/*开启零点中断*/
//	NVIC_EnableIRQ(EPWM_IRQn);
//	/*
//	(6)设置优先级
//	*/	
//	NVIC_SetPriority(EPWM_IRQn,3);					/*优先级0~3， 0最高、3最低*/
	
	/*
	(7)设置IO口输出
	*/	
	SYS_SET_IOCFG(IOP04CFG, SYS_IOCFG_P04_EPWM1);		
	EPWM_EnableOutput(EPWM_CH_1_MSK);
	/*
	(8)开启EPWM
	*/		
//	EPWM_Start(EPWM_CH_1_MSK );		
}

void buzzer_en_func(u8 en)
{
	if(en)
	{
		EPWM_Start(EPWM_CH_1_MSK);
	}
	else
	{
		EPWM_Stop(EPWM_CH_1_MSK);
	}
}
#endif


