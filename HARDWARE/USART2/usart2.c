//=============================================================================
//	Project			:	GPS
//	Version			:	V1.0
//	File				:	usart.c
//	Author			:	eonegh
//	Date				:	2023-04-20
//	Controller	:	STM32F103C6T6
//	IDE					:	Keil uVision5 V5.36.0.0
//	Compiler		:	eonegh
//	Brief				:	UART2功能API，用于与GPS模块通信
//=============================================================================

/******************************************************************************/
/* 引用的头文件（'#include'）                                                  */
/******************************************************************************/
#include "usart2.h"



/******************************************************************************/
/* 本地变量的定义 ('static')                                                   */
/******************************************************************************/



/******************************************************************************/
/* 全局变量的定义 ('extern'、'static'、'volatile'、'const')                    */
/******************************************************************************/
u8	USART2_RX_BUF[USART2_REC_LEN] = {0};	//接收缓冲，最大USART2_REC_LEN个字节，不包含结束标识符0x0D及0x0A
u16	USART2_RX_STA = 0;										//接收状态标记。bit15，接收完成标志。bit14，接收到0x0d。bit13~0，接收到的有效字节数目



/******************************************************************************/
/* 函数原型：全局 ('extern') ，本地 ('static')                                 */
/******************************************************************************/

/**
 ******************************************************************************
 ** \brief  串口2（取值1、2等）的引脚配置。
 **					串口号对应于特定引脚，且与APBx2_peripheral(RCC_APB2Periph_GPIOA)也是
 **					特定对应关系，具体查stm32f10x_rcc.h文件L492～L559。
 **
 ** \param  空
 **
 ** \retval 空
 **
 ******************************************************************************
**/
void USART2_PinConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;										//定义一个GPIO操作类结构体，用于对其参数进行配置
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);		//GPIO端口时钟：配置/使能GPIOA时钟
	
	
//USART2_TX
	GPIO_InitStructure.GPIO_Pin		=	USART2_TXD_PIN;				//GPIO引脚：			指定串口2的TXD引脚
	GPIO_InitStructure.GPIO_Speed	=	GPIO_Speed_50MHz;				//GPIO引脚时钟：	IO口时钟速度为50MHz（模拟输入模式时无须定义引脚时钟）
	GPIO_InitStructure.GPIO_Mode	=	GPIO_Mode_AF_PP;				//GPIO输入输出：	推挽式复用功能（即可输出高电平还可以输出低电平）	
	GPIO_Init(USART2_PORT,&GPIO_InitStructure);						//GPIO配置生效：	根据设定参数初始化端口及TXD引脚
	
	
//USART2_RX
	GPIO_InitStructure.GPIO_Pin = USART2_RXD_PIN;					//GPIO引脚：			指定串口2的RXD引脚
//	GPIO_InitStructure.GPIO_Speed	=	GPIO_Speed_50MHz;				//GPIO引脚时钟：	IO口时钟速度为50MHz（模拟输入模式时无须定义引脚时钟）
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		//GPIO输入输出：	输入浮空	
	GPIO_Init(USART2_PORT, &GPIO_InitStructure);						//GPIO配置生效：	根据设定参数初始化端口及RXD引脚
}


/**
 ******************************************************************************
 ** \brief  串口2（取值1、2、3等）的自身参数及中断配置。
 **					串口号对应于特定引脚，且与APBx2_peripheral(RCC_APBx2Periph_GPIOA)也是
 **					特定对应关系，具体查stm32f10x_rcc.h文件L492～L559。
 **
** \param  bound：串口波特率，通常取值9600、115200
 **
 ** \retval 空
 **
 ******************************************************************************
**/
void USART2_Init(u32 bound)
{
	USART_InitTypeDef USART_InitStructure;										//定义一个USART操作类结构体，用于对其参数进行配置
	NVIC_InitTypeDef 	NVIC_InitStructure;											//定义一个NVIC（中断控制器）操作类结构体，用于对其参数进行配置

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);		//使能USART2
	RCC_APB1PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);			//串口属于复用功能，要开启复用时钟
	
//配置USART2的结构体
	USART_DeInit(USART2);																														//清空USART2的配置为默认值
	USART_InitStructure.USART_BaudRate = bound;																			//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;											//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;													//1个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;															//无奇偶校验位
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;									//收发模式		
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件数据流控制
	USART_Init(USART2, &USART_InitStructure);																				//初始化USART2

	
//配置中断控制器的结构体
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;				//USART2中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;	//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);													//根据指定的参数初始化VIC寄存器

	USART_ClearFlag(USART2, USART_FLAG_TXE);								//开启中断之前，先清空一下中断标志位
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);					//开启USART2接收中断
	USART_ITConfig(USART2, USART_IT_TC, DISABLE);						//关闭USART2发送中断。切记：初始化时不能使能串口发送中断，否则会导致程序运行时一直在进串口中断


	USART_Cmd(USART2, ENABLE);															//使能USART2
	
} 

/**
 ******************************************************************************
 ** \brief  串口2（取值1、2等）的中断服务程序。依次接收到标识符0x0d、0x0a表示数据帧接收完毕
 **
 ** \param  空
 **
 ** \retval 空
 **
 ******************************************************************************
**/
void USART2_IRQHandler(void)
{
	u8 Res;
	
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)	//判断结束标识符0x0D、0x0A
	{
		Res = USART_ReceiveData(USART2);						//读取接收到的数据
		if(0 == (USART2_RX_STA & 0x8000))						//接收未完成
		{
			if(USART2_RX_STA & 0x4000)								//上一次接收到了结束标识符0x0D
			{
				if(Res != 0x0A)	USART2_RX_STA=0;				//接收错误，重新开始
				else USART2_RX_STA |= 0x8000;						//接收完成了
			}
			else //上一次未收到0x0D
			{	
				if(0x0D == Res)	USART2_RX_STA |= 0x4000;				//标记本次接收到了0x0D
				else
				{
					USART2_RX_BUF[USART2_RX_STA & 0X3FFF]	=	Res;								//存储本次接收到的数据，数据帧长度不超过0x3FFF
					USART2_RX_STA++;
					if(USART2_RX_STA > (USART2_REC_LEN-1))	USART2_RX_STA = 0;	//已接收数据长度超过限制，丢弃本次数据，重新开始接收
				}
			}
		}
  } 
}


void USART2_SendData(float data_frame[],u16 len)
{
	u16 t;
	for(t=0;t<len;t++)
		{
			USART_SendData(USART2, data_frame[t]);									//向串口2发送数据
			while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);	//等待发送结束
		}
}