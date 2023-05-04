//=============================================================================
//	Project			:	GPS
//	Version			:	V1.0
//	File				:	usart.c
//	Author			:	eonegh
//	Date				:	2023-04-20
//	Controller	:	STM32F103C6T6
//	IDE					:	Keil uVision5 V5.36.0.0
//	Compiler		:	eonegh
//	Brief				:	UART2����API��������GPSģ��ͨ��
//=============================================================================

/******************************************************************************/
/* ���õ�ͷ�ļ���'#include'��                                                  */
/******************************************************************************/
#include "usart2.h"



/******************************************************************************/
/* ���ر����Ķ��� ('static')                                                   */
/******************************************************************************/



/******************************************************************************/
/* ȫ�ֱ����Ķ��� ('extern'��'static'��'volatile'��'const')                    */
/******************************************************************************/
u8	USART2_RX_BUF[USART2_REC_LEN] = {0};	//���ջ��壬���USART2_REC_LEN���ֽڣ�������������ʶ��0x0D��0x0A
u16	USART2_RX_STA = 0;										//����״̬��ǡ�bit15��������ɱ�־��bit14�����յ�0x0d��bit13~0�����յ�����Ч�ֽ���Ŀ



/******************************************************************************/
/* ����ԭ�ͣ�ȫ�� ('extern') ������ ('static')                                 */
/******************************************************************************/

/**
 ******************************************************************************
 ** \brief  ����2��ȡֵ1��2�ȣ����������á�
 **					���ںŶ�Ӧ���ض����ţ�����APBx2_peripheral(RCC_APB2Periph_GPIOA)Ҳ��
 **					�ض���Ӧ��ϵ�������stm32f10x_rcc.h�ļ�L492��L559��
 **
 ** \param  ��
 **
 ** \retval ��
 **
 ******************************************************************************
**/
void USART2_PinConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;										//����һ��GPIO������ṹ�壬���ڶ��������������
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);		//GPIO�˿�ʱ�ӣ�����/ʹ��GPIOAʱ��
	
	
//USART2_TX
	GPIO_InitStructure.GPIO_Pin		=	USART2_TXD_PIN;				//GPIO���ţ�			ָ������2��TXD����
	GPIO_InitStructure.GPIO_Speed	=	GPIO_Speed_50MHz;				//GPIO����ʱ�ӣ�	IO��ʱ���ٶ�Ϊ50MHz��ģ������ģʽʱ���붨������ʱ�ӣ�
	GPIO_InitStructure.GPIO_Mode	=	GPIO_Mode_AF_PP;				//GPIO���������	����ʽ���ù��ܣ���������ߵ�ƽ����������͵�ƽ��	
	GPIO_Init(USART2_PORT,&GPIO_InitStructure);						//GPIO������Ч��	�����趨������ʼ���˿ڼ�TXD����
	
	
//USART2_RX
	GPIO_InitStructure.GPIO_Pin = USART2_RXD_PIN;					//GPIO���ţ�			ָ������2��RXD����
//	GPIO_InitStructure.GPIO_Speed	=	GPIO_Speed_50MHz;				//GPIO����ʱ�ӣ�	IO��ʱ���ٶ�Ϊ50MHz��ģ������ģʽʱ���붨������ʱ�ӣ�
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		//GPIO���������	���븡��	
	GPIO_Init(USART2_PORT, &GPIO_InitStructure);						//GPIO������Ч��	�����趨������ʼ���˿ڼ�RXD����
}


/**
 ******************************************************************************
 ** \brief  ����2��ȡֵ1��2��3�ȣ�������������ж����á�
 **					���ںŶ�Ӧ���ض����ţ�����APBx2_peripheral(RCC_APBx2Periph_GPIOA)Ҳ��
 **					�ض���Ӧ��ϵ�������stm32f10x_rcc.h�ļ�L492��L559��
 **
** \param  bound�����ڲ����ʣ�ͨ��ȡֵ9600��115200
 **
 ** \retval ��
 **
 ******************************************************************************
**/
void USART2_Init(u32 bound)
{
	USART_InitTypeDef USART_InitStructure;										//����һ��USART������ṹ�壬���ڶ��������������
	NVIC_InitTypeDef 	NVIC_InitStructure;											//����һ��NVIC���жϿ�������������ṹ�壬���ڶ��������������

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);		//ʹ��USART2
	RCC_APB1PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);			//�������ڸ��ù��ܣ�Ҫ��������ʱ��
	
//����USART2�Ľṹ��
	USART_DeInit(USART2);																														//���USART2������ΪĬ��ֵ
	USART_InitStructure.USART_BaudRate = bound;																			//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;											//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;													//1��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;															//����żУ��λ
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;									//�շ�ģʽ		
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��Ӳ������������
	USART_Init(USART2, &USART_InitStructure);																				//��ʼ��USART2

	
//�����жϿ������Ľṹ��
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;				//USART2�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;	//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);													//����ָ���Ĳ�����ʼ��VIC�Ĵ���

	USART_ClearFlag(USART2, USART_FLAG_TXE);								//�����ж�֮ǰ�������һ���жϱ�־λ
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);					//����USART2�����ж�
	USART_ITConfig(USART2, USART_IT_TC, DISABLE);						//�ر�USART2�����жϡ��мǣ���ʼ��ʱ����ʹ�ܴ��ڷ����жϣ�����ᵼ�³�������ʱһֱ�ڽ������ж�


	USART_Cmd(USART2, ENABLE);															//ʹ��USART2
	
} 

/**
 ******************************************************************************
 ** \brief  ����2��ȡֵ1��2�ȣ����жϷ���������ν��յ���ʶ��0x0d��0x0a��ʾ����֡�������
 **
 ** \param  ��
 **
 ** \retval ��
 **
 ******************************************************************************
**/
void USART2_IRQHandler(void)
{
	u8 Res;
	
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)	//�жϽ�����ʶ��0x0D��0x0A
	{
		Res = USART_ReceiveData(USART2);						//��ȡ���յ�������
		if(0 == (USART2_RX_STA & 0x8000))						//����δ���
		{
			if(USART2_RX_STA & 0x4000)								//��һ�ν��յ��˽�����ʶ��0x0D
			{
				if(Res != 0x0A)	USART2_RX_STA=0;				//���մ������¿�ʼ
				else USART2_RX_STA |= 0x8000;						//���������
			}
			else //��һ��δ�յ�0x0D
			{	
				if(0x0D == Res)	USART2_RX_STA |= 0x4000;				//��Ǳ��ν��յ���0x0D
				else
				{
					USART2_RX_BUF[USART2_RX_STA & 0X3FFF]	=	Res;								//�洢���ν��յ������ݣ�����֡���Ȳ�����0x3FFF
					USART2_RX_STA++;
					if(USART2_RX_STA > (USART2_REC_LEN-1))	USART2_RX_STA = 0;	//�ѽ������ݳ��ȳ������ƣ������������ݣ����¿�ʼ����
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
			USART_SendData(USART2, data_frame[t]);									//�򴮿�2��������
			while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);	//�ȴ����ͽ���
		}
}