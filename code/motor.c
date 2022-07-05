#include "motor.h"
#include "define.h"


u8 feed_error = 0;				//喂食错误 0x1(堵粮) 			0x2（设备异常） 0x4（无粮）
u8 send_error_again = 0;		//定时发送异常标志
u8 feed_detect_sw = 0;			////喂食完（5分钟）标志位
u16 task_feed_sw = 0;			//喂食完（5分钟）计时时间清0
u8 led_status_allow_feed = 0;	//喂食完成后允许状态灯开启标志
u8 feed_detect_again = 0;		//红外错误：		确保是喂食错误，执行再次喂食检测
u8 feed_required 	= 0; 		//需要执行喂食：			0(不执行) 1(执行) 2(执行)
u8 feed_type 		= 0; 		//喂食类型：		
#ifdef MOTOR_REVERSE
u8 reverse_flag = 0;
#endif

u8 food_empty_flag = 0;			//桶内余粮状态


struct feed_infos  feed_info;

void motor_contorl(enum MOTOR_CONTROL_E motor_c)
{
	switch(motor_c)
	{
		case MOTOR_PULL:
		{
			MOTOR_FEED_G = 1;
			#ifdef MOTOR_REVERSE
			MOTOR_FEED_B = 0;
			#endif
		}
		break;
		
		case MOTOR_PUSH:
		{
			MOTOR_FEED_G = 0;
			#ifdef MOTOR_REVERSE
			MOTOR_FEED_B = 1;
			#endif
		}
		break;
		
		case MOTOR_STOP:
		{
			MOTOR_FEED_G = 0;
			#ifdef MOTOR_REVERSE
			MOTOR_FEED_B = 0;
			#endif
		}
		break;
		
		default:
			break;			
	}
}


#ifdef DOOR_CONTROL
uint8_t door_open = 0;				//关门错误标志（=2 关门出错，停止关门）

void door_motor_contorl(enum DOOR_CONTROL_E door_c)
{
	switch(door_c)
	{
		case DOOR_PULL:
		{
			MOTOR_DOOR_G = 1;
			MOTOR_DOOR_B = 0;
		}
		break;
		
		case DOOR_PUSH:
		{
			MOTOR_DOOR_G = 0;
			MOTOR_DOOR_B = 1;
		}
		break;
		
		case DOOR_STOP:
		{
			MOTOR_DOOR_G = 0;
			MOTOR_DOOR_B = 0;
		}
		break;
		
		default:
			break;			
	}
}

uint8_t motor_door_open(void)
{
	uint16_t timeout = 400;
	
	task_run_motor = 1;
	door_open = 0;
	
	printf("open:%d, close:%d\r\n",(DOOR_DET_P == 1),(DOOR_DET_N == 1));
	//到位不在前到位位置		开门(门仓到位，压着为高电平)
	if(DOOR_DET_P != DOOR_CLOSE)
	{
		door_motor_contorl(DOOR_PULL);
	}

	while(timeout)
	{
		if(DOOR_DET_P == DOOR_CLOSE)				//到达开门到位位置
		{
			delay_ms(10);
			if(DOOR_DET_P == DOOR_CLOSE)
			{
				door_motor_contorl(DOOR_STOP);
				printf("door open success\r\n");
				feed_error &= ~ERROR_DOOR_CONTROL;				//仓门正常
				break;
			}				
		}
		delay_ms(10);
		
		timeout--;
		if(timeout == 0)
		{
			door_motor_contorl(DOOR_STOP);
			printf("open door timeout!!!\r\n");
						
			if((feed_error & ERROR_DOOR_CONTROL) == 0)
			{
				buzzer_flag = 1;				//蜂鸣器响一声
			}			
			feed_error |= ERROR_DOOR_CONTROL;				//仓门异常

			return 1;
		}
	}
	
	door_motor_contorl(DOOR_STOP);
	return 0;
}

uint8_t motor_door_close(void)
{
	uint16_t timeout = 1300;
		
	task_run_motor = 1;
	door_open = 0;

	//到位不在后到位位置		关门
	if(DOOR_DET_N != DOOR_CLOSE)
	{
		door_motor_contorl(DOOR_PUSH);
	}
	
	while(timeout)
	{
		#ifdef IR_FEED_TUBE
		if(FEED_DET == 1)			//落粮对管堵塞
		{
			motor_door_open();
			delay_ms(1000);
			
			if(FEED_DET == 1)			//落粮对管堵塞
			{
				break;
			}
			else
			{
				door_motor_contorl(DOOR_PUSH);
			}
		}		
		#endif
		
		if(DOOR_DET_N == DOOR_CLOSE)				//到达后到位位置
		{
			delay_ms(10);
			if(DOOR_DET_N == DOOR_CLOSE)
			{
				door_motor_contorl(DOOR_STOP);	
				printf("close door success!!!\r\n");
				feed_error &= ~ERROR_DOOR_CONTROL;				//仓门正常
				break;
			}
		}
		
		if(timeout == 800)
		{
			printf("close door error first time, open the door\r\n");			
			motor_door_open();
			delay_ms(1000);
			timeout = 790;
			door_motor_contorl(DOOR_PUSH);
		}
		
		if(timeout == 300)
		{
			printf("close door error second time, open the door until next feed time\r\n");			
			motor_door_open();
			door_open = 2;
			task_run_motor = 0;
			
			if((feed_error & ERROR_DOOR_CONTROL) == 0)
			{
				buzzer_flag = 1;				//蜂鸣器响一声
			}			
			feed_error |= ERROR_DOOR_CONTROL;				//仓门异常
			
			return 1;
		}
		
		delay_ms(10);
		timeout--;
	}
	
	door_motor_contorl(DOOR_STOP);
	task_run_motor = 0;

	return 0;
}
#endif



u16 motor_run(void)
{
	u16 timeout1 = MOTO_TIMEOUT; //用于判断电机超时
	u16 i;
	u16 key_cont = 0;
	#ifdef MOTOR_DET_ADDR
	u8 feed_det_flag = 0;
	#endif
	
	//启动电机
	motor_contorl(MOTOR_PULL);

	//先让电机转过一定角度，退出到位开关压着的状态
	key_cont = 0;
	i = 200;
	printf("det out:i:%d \r\n",i);
	while(i)
	{
		delay_ms(10);

		if(MOTOR_DET == 0)
		{
			printf("MOTOR_DET \r\n");
			break;
		}
		i--;
		if(i == 0)
		{
			printf("i=0\r\n");
			timeout1 = 0;
		}
	}

	//等待电机中断(下降沿触发)到来，中断到来后判断中断引脚电平是否为低(并有延时去抖)
	//低表示限位开关还被电机的旋转机构压着，这时电机停止转动
	while(timeout1)
	{
		if(MOTOR_DET == 1) //喂食电机到位
		{			
			delay_ms(10);	//延时去抖
			
			if(MOTOR_DET == 1)
			{
				key_cont++;
				if(key_cont > 6)
				{
					printf("feed det and out \r\n");
					key_cont = 0;
					#ifndef MOTOR_DET_ADDR
					return timeout1;
					#else
					feed_det_flag = 1;					
					#endif
				}
			}
		}
		#ifdef MOTOR_DET_ADDR
		if(feed_det_flag && MOTOR_DET == 0)
		{
			feed_det_flag = 0;
			return timeout1;
		}
		#endif
		
		delay_ms(10);
		timeout1--;

	}
	//卡粮，电机抖动
	#ifdef MOTOR_REVERSE
	if(timeout1 == 0 && reverse_flag == 0)
	{
		reverse_flag++;
				
		motor_contorl(MOTOR_PUSH);
		delay_ms(500);
		motor_contorl(MOTOR_PULL);
		delay_ms(500);
		motor_contorl(MOTOR_PUSH);
		delay_ms(500);
		motor_contorl(MOTOR_PULL);
		delay_ms(500);
		motor_contorl(MOTOR_PUSH);
		delay_ms(500);
		motor_contorl(MOTOR_PULL);
		delay_ms(500);
		motor_run();
//		return 0xfe;
	}
	else
	{
		reverse_flag = 0;
	}
	#endif
	
	return timeout1;

}

#ifdef IR_FEED_TUBE
u32 motor(u32 count, u8 * err_type)
{
	u8 errs = 0;
	int feed_cont = count;		//用于判断电机转动次数，每转动一次，喂食一份（大概10g）
	int timeout = MOTO_TIMEOUT; //用于判断电机超时
	int feed_already = count;		//时间喂食份数（大概10g）
	u8 time_ir_out = 50;		//堵粮超时，连续堵粮5s

	ledtime_flag = 1;
	task_run_led = 0;
	
	task_run_motor = 1;
	
	IR_LED_CTRL	= 1;
	
	delay_ms(40);
	*err_type = 0;
	
	SPK_EN_flag = 1;					//播音
	#ifdef TY_DP_RECORD
	voice_time_time = flash_feed_info.voice_times;
	#endif
	
//	P0EXTIE |=  0x08;			//开启红外中断
	
	//判断是否完成所有的喂食分量，电机每转动一次喂食一份（大概10g）
	while(feed_cont > 0)
	{
again:
		WDT_time = 0;
		
		if(FEED_DET == 0)
			timeout = motor_run();
		else
		{
			feed_cont++;
			feed_already++;
			timeout = 5;
		}
		//电机超时，电机没转或到位开关探测错误(到位开关没中断)
		if(timeout == 0)
		{
			printf("motor not move or det err\r\n");
			*err_type |= ERROR_MOTOR_TIMEOUT;
			feed_error |= ERROR_MOTOR_TIMEOUT;//卡粮
			buzzer_flag = 1;				//蜂鸣器响一声
			task_run_led = 1;
			goto err;
		}

		//延时让食物充分掉落
		MOTOR_DELAY;

		feed_cont--;
		feed_already--;

#ifdef	IR_CHECK_FOOD	
		if(food_count == 0)
			feed_already++;
		
		//食物空(食物计数0 且红外对管导通)余粮不足
		if((food_count == 0) && (FOOD_DET == 0))		//导通
		{
			*err_type = ERROR_EMPTY;
			errs++;

			if(errs <= 4)
			{
				*err_type = 0;
				feed_cont++;
				
				printf("no food ready\r\n");
				printf("feed_cont:%d - count:%d \r\n", feed_cont, count);
				
				goto again;
			}
			else
			{
				printf("no food\r\n");
				feed_error = ERROR_EMPTY;//无粮
				feed_cont++;
				*err_type = ERROR_EMPTY;
				errs = 0;
				goto err;
			}
		}
#endif		


		//红外对管被阻挡（送出的食物堆满托盘）
		if(FEED_DET == 1)
		{
			printf("ir_error ready\r\n");

			time_ir_out = 50;
			while(time_ir_out)
			{
				motor_contorl(MOTOR_STOP);
				time_ir_out--;
				delay_ms(100);
				
				if(FEED_DET == 0)
					break;
				else
				{
					if(time_ir_out == 0)
					{
						printf("ir error\r\n");
						*err_type |= ERROR_IR;		//堵粮
						
						goto err;
					}
				}
			}							
		}

		if(*err_type)
		{
			if(feed_cont != 0)
			{
				errs++;

				if(errs < 2)
				{
					*err_type = 0;
					goto again;
				}
				else
				{
					goto err;
				}
			}
		}

		//成功喂食一份
		*err_type = 0;
		errs = 0;
		feed_error = 0;
	}
err:
//	P0EXTIE &=	~0x08;			//关闭红外中断
	#ifdef MOTOR_REVERSE
		reverse_flag = 0;
	#endif

	#ifdef IR_FOOD_TUBE
	if(food_empty_flag > 0)
	{
		*err_type |= ERROR_NOT_ENOUGH;		//缺粮
		printf("food error \r\n");
	}
	#endif
	
	//停止电机
	motor_contorl(MOTOR_STOP);
	
//	IR_LED_CTRL = 0;
	task_run_motor = 0;			//喂食中标志位1（喂食中） 0（空闲）

	feed_detect_sw = 1;			//喂食完（5分钟）标志位
	task_feed_sw = 0;			//喂食完（5分钟）计时时间清0
	ledtime_flag = 0;			//喂食错误红灯闪烁1分钟标志
	led_status_allow_feed = 1;	//喂食完成后允许状态灯开启标志
	
	return (count - feed_already);
}
#else
u32 motor(u32 count, u8 * err_type)
{
	int feed_cont = count;		//用于判断电机转动次数，每转动一次，喂食一份（大概10g）
	int timeout = MOTO_TIMEOUT; //用于判断电机超时
	int feed_already = count;		//时间喂食份数（大概10g）
	
//	ledtime_flag = 1;
//	task_run_led = 0;
	
//	task_run_motor = 1;
	*err_type = 0;

//	SPK_EN_flag = 1;				//播音
	#ifdef TY_DP_RECORD
	voice_time_time = flash_feed_info.voice_times;
	#endif
	
	//判断是否完成所有的喂食分量，电机每转动一次喂食一份（大概10g）
	while(feed_cont > 0)
	{
//		food_count = 0; 		//饲料计数清零
//		WDT_time = 0;
		
		timeout = motor_run();

		//电机超时，电机没转或到位开关探测错误(到位开关没中断)
		if(timeout == 0)
		{
			printf("motor not move or det err\r\n");
			*err_type |= ERROR_MOTOR_TIMEOUT;
			feed_error |= ERROR_MOTOR_TIMEOUT;
//			task_run_led = 1;
			goto err;
		}

		//延时让食物充分掉落
		MOTOR_DELAY;

		
		feed_cont--;
		feed_already--;


		//成功喂食一份
		*err_type = 0;
		feed_error = 0;
	}
err:
	#ifdef IR_FOOD_TUBE
	if(food_empty_flag > 0)
	{
		*err_type |= ERROR_NOT_ENOUGH;
		feed_error |= ERROR_NOT_ENOUGH;
		printf("food error \r\n");
	}
	#endif
	#ifdef MOTOR_REVERSE
		reverse_flag = 0;
	#endif
	
	//停止电机
	motor_contorl(MOTOR_STOP);
//	task_run_motor = 0;			//喂食中标志位1（喂食中） 0（空闲）

	feed_detect_sw = 1;			//喂食完（5分钟）标志位
	task_feed_sw = 0;			//喂食完（5分钟）计时时间清0
//	ledtime_flag = 0;			//喂食错误红灯闪烁1分钟标志
	led_status_allow_feed = 1;	//喂食完成后允许状态灯开启标志
	
	return (count - feed_already);
}

#endif


void air_escape(void) //放气
{
	u8 air_escape_timeout = 30;
	
	if(AIR_DET == VACUUM) //真空
	{
		while(air_escape_timeout)
		{
			SWITCH_DRIVER = 1;
			air_escape_timeout--;	
			
			if(AIR_DET == NO_VACUUM)
			{			
				SWITCH_DRIVER = 0;
				feed_error &= ~ERROR_VACUUM_TIMEOUT;				//气压正常
				return;
			}
			delay_ms(100);
		}
		SWITCH_DRIVER = 0;
		if(air_escape_timeout == 0)
		{
			if((feed_error & ERROR_VACUUM_TIMEOUT) == 0)
			{
				buzzer_flag = 1;				//蜂鸣器响一声
			}
			feed_error |= ERROR_VACUUM_TIMEOUT;				//气压异常			
		}
	}
}


void air_exhaust(void) //抽气
{
	u8 air_exhaust_timeout = 50;
	
	if(AIR_DET == NO_VACUUM) //非真空
	{
		while(air_exhaust_timeout)
		{
			MOTOR_DRIVER = 1;
			air_exhaust_timeout--;
			
			if(AIR_DET == VACUUM)
			{
				MOTOR_DRIVER = 0;
				feed_error &= ~ERROR_VACUUM_TIMEOUT;				//气压正常

				return;
			}
			delay_ms(100);
		}
		MOTOR_DRIVER = 0;
		if(air_exhaust_timeout == 0)
		{
			if((feed_error & ERROR_VACUUM_TIMEOUT) == 0)
			{
				buzzer_flag = 1;				//蜂鸣器响一声
			}			
			feed_error |= ERROR_VACUUM_TIMEOUT;				//气压异常
		}
	}
	
}
