//=============================================================================
//	Project			:	GPS
//	Version			:	V1.0
//	File				:	main.c
//	Author			:	eonegh
//	Date				:	2023-04-20
//	Controller	:	STM32F103C6T6
//	IDE					:	Keil uVision5 V5.36.0.0
//	Compiler		:	eonegh
//	Brief				:	主函数
//=============================================================================

#include "usart.h"	//串口debug及解析GPS数据API
#include "led.h"		//LED控制API，用于debug
#include "delay.h"	//延时控制API

int main(void)
{

	delay_init();			//延时函数初始化
	
//串口1波特率为115200，用于使用printf函数进行debug
	USART1_PinConfig();
	USART1_Init(115200);
	
	LED_PinConfig();	//初始化LED，用于调试

	while(1)
	{		
		printf("0,USART1_RX_STA:%d\n",(USART1_RX_STA & 0X3FFF));//打印接收到的数据大小
		
		if(0 != (USART1_RX_STA & 0x8000)){	//检测是否接收完数据
		GPGBuffer_ParseNMEA();									//对GPS模块输出的NMEA语句进行解包
			
//打印GPS信号解包后的数据
		printps("1,Save_Data.GPS_Buffer:", Save_Data.GPS_Buffer,sizeof(Save_Data.GPS_Buffer));
		printps("2,Save_Data.UTCTime:", Save_Data.UTCTime,sizeof(Save_Data.UTCTime));
		printps("3,Save_Data.latitude:", Save_Data.latitude,sizeof(Save_Data.latitude));
		printps("4,Save_Data.longitude:",Save_Data.longitude,sizeof(Save_Data.longitude));
		printf("5,Save_Data.isUsefull:%d\n",Save_Data.isUsefull);
			
		USART1_RX_STA =0;									//开启下一次的数据接收
		}

		LED0 = !LED0;		//LED每秒闪烁一次,用于debug

		delay_ms(1000);
	}
	
 return 0;
}

/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
