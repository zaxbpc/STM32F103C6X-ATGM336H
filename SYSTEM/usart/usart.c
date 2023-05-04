//=============================================================================
//	Project			:	GPS
//	Version			:	V1.0
//	File				:	usart.c
//	Author			:	eonegh
//	Date				:	2023-04-20
//	Controller	:	STM32F103C6T6
//	IDE					:	Keil uVision5 V5.36.0.0
//	Compiler		:	eonegh
//	Brief				:	串口debug（printf函数）及解析GPS数据API
//=============================================================================

/******************************************************************************/
/* 引用的头文件（'#include'）                                                  */
/******************************************************************************/
#include "usart.h"	//串口debug（printf函数）API



/******************************************************************************/
/* 本地变量的定义 ('static')                                                   */
/******************************************************************************/



/******************************************************************************/
/* 全局变量的定义 ('extern'、'static'、'volatile'、'const')                    */
/******************************************************************************/
u8	USART1_RX_BUF[USART1_REC_LEN] = {0};	//接收缓冲，最大USART1_REC_LEN个字节，不包含结束标识符0x0D及0x0A
volatile u16	USART1_RX_STA = 0;					//接收状态标记。bit15，接收完成标志。bit14，接收到0x0d。bit13~0，接收到的有效字节数目
_SaveData Save_Data;											//保存的GLL格式数据解析结构体变量


/******************************************************************************/
/* 函数原型：全局 ('extern') ，本地 ('static')                                 */
/******************************************************************************/

/**
 ******************************************************************************
 ** \brief  支持printf函数,而不需要选择use MicroLIB
 **
 ** \param  空
 **
 ** \retval 空
 **
 ******************************************************************************
**/
#if 1
#pragma import(__use_no_semihosting)

struct __FILE 											//标准库需要的支持函数
{ 
	int handle;
};

FILE __stdout;


void _sys_exit(int x)								//定义_sys_exit()以避免使用半主机模式
{ 
	x = x;
}


int fputc(int ch, FILE *f)					//重定义fputc函数
{
	while( RESET == USART_GetFlagStatus( USART1, USART_FLAG_TXE ));	//读取SR寄存器，等待数据寄存器为空
	USART_SendData(USART1,(u8)ch);
	while( RESET == USART_GetFlagStatus( USART1, USART_FLAG_TC ));	//等待数据从数据寄存器到转移寄存器，然后发送到TXD信号线
	
	return ch;
}
#endif


/**
 ******************************************************************************
 ** \brief  串口1的引脚配置。
 **					串口1对应于特定引脚，且与APBx2_peripheral(RCC_APB2Periph_GPIOA)也是
 **					特定对应关系，具体查stm32f10x_rcc.h文件L492～L559。
 **
 ** \param  空
 **
 ** \retval 空
 **
 ******************************************************************************
**/
void USART1_PinConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;										//定义一个GPIO操作类结构体，用于对其参数进行配置
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);		//GPIO端口时钟：配置/使能GPIOA时钟
	
	
//USARTx1_TX
	GPIO_InitStructure.GPIO_Pin		=	USART1_TXD_PIN;					//GPIO引脚：			指定串口1的TXD引脚
	GPIO_InitStructure.GPIO_Speed	=	GPIO_Speed_50MHz;				//GPIO引脚时钟：	IO口时钟速度为50MHz（模拟输入模式时无须定义引脚时钟）
	GPIO_InitStructure.GPIO_Mode	=	GPIO_Mode_AF_PP;				//GPIO输入输出：	推挽式复用功能（即可输出高电平还可以输出低电平）	
	GPIO_Init(USART1_PORT,&GPIO_InitStructure);							//GPIO配置生效：	根据设定参数初始化端口及TXD引脚
	
	
//USARTx1_RX
	GPIO_InitStructure.GPIO_Pin = USART1_RXD_PIN;						//GPIO引脚：			指定串口1的RXD引脚
//	GPIO_InitStructure.GPIO_Speed	=	GPIO_Speed_50MHz;				//GPIO引脚时钟：	IO口时钟速度为50MHz（模拟输入模式时无须定义引脚时钟）
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		//GPIO输入输出：	输入浮空	
	GPIO_Init(USART1_PORT, &GPIO_InitStructure);						//GPIO配置生效：	根据设定参数初始化端口及RXD引脚
}


/**
 ******************************************************************************
 ** \brief  串口1的自身参数及中断配置。
 **					串口1对应于特定引脚，且与APBx2_peripheral(RCC_APBx2Periph_GPIOA)也是
 **					特定对应关系，具体查stm32f10x_rcc.h文件L492～L559。
 **
** \param  bound：串口波特率，通常取值9600、115200
 **
 ** \retval 空
 **
 ******************************************************************************
**/
void USART1_Init(u32 bound)
{
	USART_InitTypeDef USART_InitStructure;										//定义一个USART操作类结构体，用于对其参数进行配置
	NVIC_InitTypeDef 	NVIC_InitStructure;											//定义一个NVIC（中断控制器）操作类结构体，用于对其参数进行配置

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);		//使能USART1
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);			//串口属于复用功能，要开启复用时钟
	
	
//配置USART1的结构体
	USART_DeInit(USART1);																														//清空USART1的配置为默认值
	USART_InitStructure.USART_BaudRate = bound;																			//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;											//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;													//1个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;															//无奇偶校验位
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;									//收发模式		
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件数据流控制
	USART_Init(USART1, &USART_InitStructure);																				//初始化USARTx

	
//配置中断控制器的结构体
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);					//配置中断控制器优先抢占级组（如果main()函数中已有，则注释此处）
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;				//USART1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;	//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;			//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);													//根据指定的参数初始化VIC寄存器
	
	USART_ClearFlag(USART1, USART_FLAG_TXE);								//开启中断之前，先清空一下中断标志位
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);					//开启USART1接收中断
	USART_ITConfig(USART1, USART_IT_TC, DISABLE);						//关闭USART1发送中断。切记：初始化时不能使能串口发送中断，否则会导致程序运行时一直在进串口中断


	USART_Cmd(USART1, ENABLE);															//使能USART1
}


/**
 ******************************************************************************
 ** \brief  串口1的中断服务程序。依次接收到标识符0x0d、0x0a表示数据帧接收完毕
 **
 ** \param  空
 **
 ** \retval 空
 **
 ******************************************************************************
**/
void USART1_IRQHandler(void)
{
	u8 Res;
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		Res = USART_ReceiveData(USART1);						//读取接收到的数据
		if(0 == (USART1_RX_STA & 0x8000))						//接收未完成
		{
			if(USART1_RX_STA & 0x4000)								//上一次接收到了结束标识符0x0D
			{
				if(Res != 0x0A)	
				{
					USART1_RX_BUF[USART1_RX_STA & 0X3FFF]	=	Res;													//存储本次接收到的数据，数据帧长度不超过0x3FFF
					USART1_RX_STA++;
					if((USART1_RX_STA & 0X3FFF) > (USART1_REC_LEN-1))	USART1_RX_STA = 0;	//已接收数据长度超过限制，丢弃本次数据，重新开始接收
				}
				else {
					USART1_RX_STA |= 0x8000;	//接收完成了
					if(USART1_RX_BUF[0] == '$' && USART1_RX_BUF[4] == 'L')								//确认收到"$GNGLL"这一帧数据
					{
						memcpy(Save_Data.GPS_Buffer, USART1_RX_BUF, GPS_Buffer_Length); 		//缓存数据
						Save_Data.isGetData = 1;
					}	
				}
			}
			else	//上一次未收到0结束标识符x0D
			{	
				if(0x0D == Res)	USART1_RX_STA |= 0x4000;																//标记本次接收到了0x0D
				else
				{
					USART1_RX_BUF[USART1_RX_STA & 0X3FFF]	=	Res;													//存储本次接收到的数据，数据帧长度不超过0x3FFF
					USART1_RX_STA++;
					if((USART1_RX_STA & 0X3FFF) > (USART1_REC_LEN-1))	USART1_RX_STA = 0;	//已接收数据长度超过限制，丢弃本次数据，重新开始接收
				}
			}
		}
  } 
}


/**
 ******************************************************************************
 ** \brief  对NMEA格式的GPS信号进行解析，拆成最基础的经纬度数据和UTC
 **
 ** \param  空
 **
 ** \retval 空
 **
 ******************************************************************************
**/
void GPGBuffer_ParseNMEA(void)
{
	char *subString =NULL;
	char *subStringNext =NULL;
	char i = 0;
	if (Save_Data.isGetData)	//数据解包前进一步判断是否接收到了数据
	{
		for (i = 0; i <= 6; i++)
		{
			if (i == 0){
				if((subString = strstr(Save_Data.GPS_Buffer, ",")) == NULL) {
					printf("There is not a comma!\n");
					return;	//没有任何逗号，表示接收到的数据有误，直接丢弃
				}
				else{
					subString++;
					if((subStringNext = strstr(subString, ",")) != NULL){
						char usefullBuffer[2];
						switch(i)
						{
							case 1:memcpy(Save_Data.latitude, subString, subStringNext - subString);break;	//获取纬度
							case 2:memcpy(Save_Data.N_S, subString, subStringNext - subString);break;				//获取N/S
							case 3:memcpy(Save_Data.longitude, subString, subStringNext - subString);break;	//获取经度信息
							case 4:memcpy(Save_Data.E_W, subString, subStringNext - subString);break;				//获取E/W
							case 5:memcpy(Save_Data.UTCTime, subString, subStringNext - subString);break;		//获取UTC时间
							case 6:memcpy(usefullBuffer, subString, subStringNext - subString);break;				//获取数据有效标志
							default:;
						}
						subString = subStringNext;
						Save_Data.isParseData = 1;	//数据解析成功标志
						Save_Data.isUsefull = (usefullBuffer[0] == 'A'?1:0);//判断有效标志
					}
					else printf("cut strings error!\n");
				}
			}
		}
	}
}


/**
 ******************************************************************************
 ** \brief  将字符数组中特定长度的字符打印出来
 **
** \param  head：		需要打印的标志，用于增强区分，一般就是字符串，如 "UTCtime:"
 **					ps：		字符串指针或字符数组
 **				 length：	需要打印的字符长度，从字符串或数组的第一位开始，大于0的整数
 **
 ** \retval 空
 **
 ******************************************************************************
**/
void printps(char *head, char *ps, u16 length)
{
	u16 i = 0;
	if(NULL != head) printf("%s",head);
	for(i = 0; i<length;i++)
		printf("%c",*(ps+i));
	printf("\n");	//结尾的换行符
}

/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
