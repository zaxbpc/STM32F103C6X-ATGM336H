//=============================================================================
//	Project			:	GPS
//	Version			:	V1.0
//	File				:	usart.h
//	Author			:	eonegh
//	Date				:	2023-04-20
//	Controller	:	STM32F103C6T6
//	IDE					:	Keil uVision5 V5.36.0.0
//	Compiler		:	eonegh
//	Brief				:	串口debug（printf函数）及解析GPS数据API
//=============================================================================
 
/******************************************************************************/
/* 条件编译（'#ifndef'）                                                       */
/******************************************************************************/
#ifndef __USART_H__
#define __USART_H__


/******************************************************************************/
/* 引用的头文件（'.h'）                                                        */
/******************************************************************************/
#include "sys.h"			//STM32基础库，但属于用户自定义而非系统自带文件
#include "stdio.h"		//标准输入输出库，此处使用了“FILE”数据类型
#include "string.h"		//字符串操作相关API


/******************************************************************************/
/* 全局预处理/宏定义 ('#define')                                               */
/******************************************************************************/
#define USART1_REC_LEN		100					//指定接收数据的长度（字节数），不包含结束标识符0x0D及0x0A

#define USART1_PORT				GPIOA				//指定串口1的端口号，取值GPIOA、GPIOB、GPIOC、GPIOD、GPIOE等
#define USART1_TXD_PIN		GPIO_Pin_9	//指定串口1的TXD引脚号，取值0、1、2、3、...、15
#define USART1_RXD_PIN		GPIO_Pin_10	//指定串口1的RXD引脚号，取值0、1、2、3、...、15

//GPS数据的相关格式
#define GPS_Buffer_Length 51	//中国地区，GLL格式的NMEA数据的总长度
#define UTCTime_Length 10			//UTC时间的长度
#define latitude_Length 10		//中国地区，维度的长度
#define N_S_Length 1					//南纬还是北纬
#define longitude_Length 11		//中国地区，经度的长度
#define E_W_Length 1					//东经还是西经


/******************************************************************************/
/* 全局类型定义 ('typedef')                                                    */
/******************************************************************************/
typedef struct SaveData
{
	char GPS_Buffer[GPS_Buffer_Length];	//将GLL格式的GPS数据缓存
	char isGetData;											//是否获取到GPS数据
	char isParseData;										//是否解析完成
	char UTCTime[UTCTime_Length];				//UTC时间
	char latitude[latitude_Length];			//纬度
	char N_S[N_S_Length];								//N还是S
	char longitude[longitude_Length];		//经度
	char E_W[E_W_Length];								//E还是W
	char isUsefull;											//定位是否有效
}_SaveData;



/******************************************************************************/
/* 全局变量声明 ('extern'、'static'、'volatile'、'const')                      */
/******************************************************************************/
extern u8		USART1_RX_BUF[USART1_REC_LEN];	//接收缓冲，最大USART1_REC_LEN个字节，不包含结束标识符0x0D及0x0A
extern volatile u16	USART1_RX_STA;					//接收状态标记。bit15，接收完成标志。bit14，接收到0x0d。bit13~0，接收到的有效字节数目
extern _SaveData Save_Data;									//保存的GLL格式数据解析结构体


/******************************************************************************/
/* 局部变量声明 ('static'、'volatile'、'const')                                */
/******************************************************************************/



/******************************************************************************/
/* 全局函数声明 ('extern')                                                     */
/******************************************************************************/
extern void USART1_PinConfig(void);		//串口1的引脚配置
extern void USART1_Init(u32 bound);		//串口1的自身参数及中断配置

extern void GPGBuffer_ParseNMEA(void);//对NMEA格式的GPS信号进行解析，拆成最基础的经纬度数据和UTC
extern void printps(char *head, char *ps, u16 length);	//将字符数组中特定长度的字符打印出来


#endif


/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
