//=============================================================================
//	Project			:	GPS
//	Version			:	V1.0
//	File				:	main.c
//	Author			:	eonegh
//	Date				:	2023-04-20
//	Controller	:	STM32F103C6T6
//	IDE					:	Keil uVision5 V5.36.0.0
//	Compiler		:	eonegh
//	Brief				:	������
//=============================================================================

#include "usart.h"	//����debug������GPS����API
#include "led.h"		//LED����API������debug
#include "delay.h"	//��ʱ����API

int main(void)
{

	delay_init();			//��ʱ������ʼ��
	
//����1������Ϊ115200������ʹ��printf��������debug
	USART1_PinConfig();
	USART1_Init(115200);
	
	LED_PinConfig();	//��ʼ��LED�����ڵ���

	while(1)
	{		
		printf("0,USART1_RX_STA:%d\n",(USART1_RX_STA & 0X3FFF));//��ӡ���յ������ݴ�С
		
		if(0 != (USART1_RX_STA & 0x8000)){	//����Ƿ����������
		GPGBuffer_ParseNMEA();									//��GPSģ�������NMEA�����н��
			
//��ӡGPS�źŽ���������
		printps("1,Save_Data.GPS_Buffer:", Save_Data.GPS_Buffer,sizeof(Save_Data.GPS_Buffer));
		printps("2,Save_Data.UTCTime:", Save_Data.UTCTime,sizeof(Save_Data.UTCTime));
		printps("3,Save_Data.latitude:", Save_Data.latitude,sizeof(Save_Data.latitude));
		printps("4,Save_Data.longitude:",Save_Data.longitude,sizeof(Save_Data.longitude));
		printf("5,Save_Data.isUsefull:%d\n",Save_Data.isUsefull);
			
		USART1_RX_STA =0;									//������һ�ε����ݽ���
		}

		LED0 = !LED0;		//LEDÿ����˸һ��,����debug

		delay_ms(1000);
	}
	
 return 0;
}

/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
