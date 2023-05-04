//=============================================================================
//	Project			:	sht30
//	Version			:	V1.0
//	File				:	myiic.c
//	Author			:	eonegh
//	Date				:	2023-04-20
//	Controller	:	STM32F103C6T6
//	IDE					:	Keil uVision5 V5.36.0.0
//	Compiler		:	eonegh
//	Brief				:	MCU的IIC通信功能API,软件模拟方式实现IIC
//=============================================================================

/******************************************************************************/
/* 引用的头文件（'#include'）                                                  */
/******************************************************************************/
#include "myiic.h"	//MCU的IIC通信功能API,软件模拟方式实现IIC



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
 ** \brief	：对IIC引脚进行配置，并完成初始化操作
 **
 ** \param	：空
 **
 ** \retval	：空
 **
 ******************************************************************************
 **/
void IIC_PinConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;										//定义一个GPIO操作类结构体，用于对其参数进行配置
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	//GPIO时钟：		使能PB端口时钟
	GPIO_InitStructure.GPIO_Pin = IIC_SCL_PIN|IIC_SDA_PIN;	//GPIO引脚：		指定SCL-->P10端口,SDA-->P11端口
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;				//GPIO输入输出：推挽输出(既可输出高电平又可低电平)
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;				//GPIO时钟：		IO口时钟速度为50MHz
	GPIO_Init(IIC_PORT, &GPIO_InitStructure);								//GPIO配置生效：根据设定参数初始化GPIOB.10和GPIOB.11
                                                          
	GPIO_SetBits(IIC_PORT,IIC_SCL_PIN|IIC_SDA_PIN);					//GPIO初始化：	PB10和PB11输出高电平，IIC空闲时输出高
}


/**
 ******************************************************************************
 ** \brief	：设置SDA引脚为数字引脚功能，且推挽输出
 **
 ** \param	：空
 **
 ** \retval	：空
 **
 ******************************************************************************
**/
void SDA_OUT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;										//定义一个GPIO操作类结构体，用于对其参数进行配置
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	//GPIO时钟：			使能PB端口时钟
	GPIO_InitStructure.GPIO_Pin = IIC_SDA_PIN;							//GPIO引脚：			SDA-->P11端口
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;				//GPIO时钟：			IO口时钟速度为50MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;				//GPIO输入输出：推挽输出(既可输出高电平又可低电平)
	GPIO_Init(IIC_PORT, &GPIO_InitStructure);								//GPIO配置生效：根据设定参数初始化GPIOB.11
}


/**
 ******************************************************************************
 ** \brief	：设置SCL引脚为模拟引脚功能，且浮空输入
 **
 ** \param	：空
 **
 ** \retval	：空
 **
 ******************************************************************************
**/
void SDA_IN(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;										//定义一个GPIO操作类结构体，用于对其参数进行配置
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	//GPIO时钟：				使能PB端口时钟
	GPIO_InitStructure.GPIO_Pin = IIC_SDA_PIN;							//GPIO引脚：				SDA-->P11端口
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;				//GPIO引脚时钟：	IO口时钟速度为50MHz（模拟输入模式时无须定义引脚时钟）
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		//GPIO输入输出：		浮空输入
	GPIO_Init(IIC_PORT, &GPIO_InitStructure);								//GPIO配置生效：		根据设定参数初始化GPIOB.11
}


/**
 ******************************************************************************
 ** \brief	：设置SDA引脚输出高低电平
 **
 ** \param	：high_low：输出高/低电平
 **
 ** \retval	：空
 **
 ******************************************************************************
**/
void SDA_DATA(Low_High_te high_low){
	if (HIGH_VOLTAGE == high_low)
		GPIO_SetBits(IIC_PORT,IIC_SDA_PIN);
	else 
		GPIO_ResetBits(IIC_PORT,IIC_SDA_PIN);
}	
			

/**
 ******************************************************************************
 ** \brief	：设置SCL引脚输出高低电平
 **
 ** \param	：high_low：输出高/低电平
 **
 ** \retval	：空
 **
 ******************************************************************************
**/
void SCL_OUT(Low_High_te high_low){
	if (HIGH_VOLTAGE == high_low)
		GPIO_SetBits(IIC_PORT,IIC_SCL_PIN);
	else 
		GPIO_ResetBits(IIC_PORT,IIC_SCL_PIN);
}													


/**
 ******************************************************************************
 ** \brief	：产生IIC起始信号
 **
 ** \param	：空
 **
 ** \retval	：空
 **
 ******************************************************************************
**/
void IIC_Start(void)
{
	SDA_OUT();							//sda线输出
	SDA_DATA(HIGH_VOLTAGE);
	SCL_OUT(HIGH_VOLTAGE);
	delay_us(20);
	SDA_DATA(LOW_VOLTAGE);	//START:when CLK is high,DATA change form high to low 
	delay_us(20);
	SCL_OUT(LOW_VOLTAGE);		//钳住I2C总线，准备发送或接收数据 
}


/**
 ******************************************************************************
 ** \brief	：产生IIC停止信号
 **
 ** \param	：空
 **
 ** \retval	：空
 **
 ******************************************************************************
**/  
void IIC_Stop(void)
{
	SDA_OUT();							//sda线输出
	SCL_OUT(LOW_VOLTAGE);
	SDA_DATA(LOW_VOLTAGE);	//STOP:when CLK is high DATA change form low to high
 	delay_us(20);
 	SCL_OUT(HIGH_VOLTAGE);
	SDA_DATA(HIGH_VOLTAGE);	//发送I2C总线结束信号
	delay_us(20);
}


/**
 ******************************************************************************
 ** \brief	：IIC主设备产生ACK应答信号
 **
 ** \param	：空
 **
 ** \retval	：空
 **
 ******************************************************************************
**/
void IIC_Ack(void)
{
	SCL_OUT(LOW_VOLTAGE);
	SDA_OUT();
	SDA_DATA(LOW_VOLTAGE);
	delay_us(20);
	SCL_OUT(HIGH_VOLTAGE);
	delay_us(20);
	SCL_OUT(LOW_VOLTAGE);
}


/**
 ******************************************************************************
 ** \brief	：IIC主设备不产生ACK应答信号
 **
 ** \param	：空
 **
 ** \retval	：空
 **
 ******************************************************************************
**/	    
void IIC_NAck(void)
{
	SCL_OUT(LOW_VOLTAGE);
	SDA_OUT();
	SDA_DATA(HIGH_VOLTAGE);
	delay_us(20);
	SCL_OUT(HIGH_VOLTAGE);
	delay_us(20);
	SCL_OUT(LOW_VOLTAGE);
}


/**
 ******************************************************************************
 ** \brief	：IIC主设备等待从设备应答信号到来
 **
 ** \param	：空
 **
 ** \retval	：1，接收应答失败
 **						0，接收应答成功
 **
 ******************************************************************************
**/  
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();				//SDA设置为输入
	SDA_DATA(HIGH_VOLTAGE);
	delay_us(10);
	SCL_OUT(HIGH_VOLTAGE);
	delay_us(10);
	while(SDA_IO_IN)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			printf("no ack!\r\n");
			IIC_Stop();
			return 1;
		}
	}
	SCL_OUT(LOW_VOLTAGE);	//时钟输出0
	return 0;
} 


/**
 ******************************************************************************
 ** \brief	：IIC主设备发送一个字节数据
 **
 ** \param	：txd：要发送的数据
 **
 ** \retval	：1，应答失败
 **						0，应答成功
 **
 ******************************************************************************
**/
void IIC_Send_Byte(u8 txd)
{
	u8 t;
	SDA_OUT();
	SCL_OUT(LOW_VOLTAGE);	//拉低时钟开始数据传输
	for(t=0;t<8;t++)
	{
		if((txd&0x80)>>7)	SDA_DATA(HIGH_VOLTAGE);
		else							SDA_DATA(LOW_VOLTAGE); 
		txd<<=1;
		delay_us(20);
		SCL_OUT(HIGH_VOLTAGE);
		delay_us(20);
		SCL_OUT(LOW_VOLTAGE);
		delay_us(20);
	}
}


/**
 ******************************************************************************
 ** \brief	：IIC主设备读取一个字节数据
 **
 ** \param	：ack：接收数据时是否需要给从设备返回ACK应答信号
 **
 ** \retval	：接收到的数据
 **
 ******************************************************************************
**/ 
u16 IIC_Read_Byte(u8 ack)
{
	u8 i;
	u16 receive=0;
	SDA_IN();	//SDA设置为输入
	for(i=0;i<8;i++ )
	{
		SCL_OUT(LOW_VOLTAGE);
		delay_us(20);
		SCL_OUT(HIGH_VOLTAGE);
		receive<<=1;
		if(SDA_IO_IN)	receive++;
		delay_us(20);
	}		
	if(!ack)	IIC_NAck();	//发送nACK
	else			IIC_Ack();	//发送ACK
	return receive;
}

/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
