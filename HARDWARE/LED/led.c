//=============================================================================
//	Project			:	GPS
//	Version			:	V1.0
//	File				:	led.c
//	Author			:	eonegh
//	Date				:	2023-04-20
//	Controller	:	STM32F103C6T6
//	IDE					:	Keil uVision5 V5.36.0.0
//	Compiler		:	eonegh
//	Brief				:	LED控制API，用于debug
//=============================================================================

/******************************************************************************/
/* 引用的头文件（'#include'）                                                  */
/******************************************************************************/
#include "led.h"	//LED控制API，用于debug



/******************************************************************************/
/* 本地变量的定义 ('static')                                                   */
/******************************************************************************/



/******************************************************************************/
/* 全局变量的定义 ('extern'、'static'、'volatile'、'const')                    */
/******************************************************************************/



/******************************************************************************/
/* 函数原型：全局 ('extern') ，本地 ('static')                                 */
/******************************************************************************/

/**
 ******************************************************************************
 ** \brief  初始化PC13为数字输出口.这个引脚连接到外部的LED
 **
 ** \param  空
 **
 ** \retval 空
 **
 ******************************************************************************
**/
void LED_PinConfig(void)
{
 GPIO_InitTypeDef  GPIO_InitStructure;									//定义一个GPIO操作类结构体，用于对其参数进行配置
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	//GPIO时钟：		使能PC端口时钟
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;				 			//GPIO引脚：		指定LED0-->P13端口
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 	//GPIO输入输出：推挽输出(既可输出高电平又可低电平)
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 	//GPIO时钟：		IO口时钟速度为50MHz
 GPIO_Init(GPIOC, &GPIO_InitStructure);					 				//GPIO配置生效：根据设定参数初始化GPIOC.13
	
 GPIO_SetBits(GPIOC,GPIO_Pin_13);						 						//GPIO初始化：	PC.13 输出高电平，即关闭LED
}
 

/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
