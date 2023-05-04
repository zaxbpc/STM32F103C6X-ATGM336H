//=============================================================================
//	Project			:	GPS
//	Version			:	V1.0
//	File				:	usart.c
//	Author			:	eonegh
//	Date				:	2023-04-20
//	Controller	:	STM32F103C6T6
//	IDE					:	Keil uVision5 V5.36.0.0
//	Compiler		:	eonegh
//	Brief				:	����debug��printf������������GPS����API
//=============================================================================

/******************************************************************************/
/* ���õ�ͷ�ļ���'#include'��                                                  */
/******************************************************************************/
#include "usart.h"	//����debug��printf������API



/******************************************************************************/
/* ���ر����Ķ��� ('static')                                                   */
/******************************************************************************/



/******************************************************************************/
/* ȫ�ֱ����Ķ��� ('extern'��'static'��'volatile'��'const')                    */
/******************************************************************************/
u8	USART1_RX_BUF[USART1_REC_LEN] = {0};	//���ջ��壬���USART1_REC_LEN���ֽڣ�������������ʶ��0x0D��0x0A
volatile u16	USART1_RX_STA = 0;					//����״̬��ǡ�bit15��������ɱ�־��bit14�����յ�0x0d��bit13~0�����յ�����Ч�ֽ���Ŀ
_SaveData Save_Data;											//�����GLL��ʽ���ݽ����ṹ�����


/******************************************************************************/
/* ����ԭ�ͣ�ȫ�� ('extern') ������ ('static')                                 */
/******************************************************************************/

/**
 ******************************************************************************
 ** \brief  ֧��printf����,������Ҫѡ��use MicroLIB
 **
 ** \param  ��
 **
 ** \retval ��
 **
 ******************************************************************************
**/
#if 1
#pragma import(__use_no_semihosting)

struct __FILE 											//��׼����Ҫ��֧�ֺ���
{ 
	int handle;
};

FILE __stdout;


void _sys_exit(int x)								//����_sys_exit()�Ա���ʹ�ð�����ģʽ
{ 
	x = x;
}


int fputc(int ch, FILE *f)					//�ض���fputc����
{
	while( RESET == USART_GetFlagStatus( USART1, USART_FLAG_TXE ));	//��ȡSR�Ĵ������ȴ����ݼĴ���Ϊ��
	USART_SendData(USART1,(u8)ch);
	while( RESET == USART_GetFlagStatus( USART1, USART_FLAG_TC ));	//�ȴ����ݴ����ݼĴ�����ת�ƼĴ�����Ȼ���͵�TXD�ź���
	
	return ch;
}
#endif


/**
 ******************************************************************************
 ** \brief  ����1���������á�
 **					����1��Ӧ���ض����ţ�����APBx2_peripheral(RCC_APB2Periph_GPIOA)Ҳ��
 **					�ض���Ӧ��ϵ�������stm32f10x_rcc.h�ļ�L492��L559��
 **
 ** \param  ��
 **
 ** \retval ��
 **
 ******************************************************************************
**/
void USART1_PinConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;										//����һ��GPIO������ṹ�壬���ڶ��������������
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);		//GPIO�˿�ʱ�ӣ�����/ʹ��GPIOAʱ��
	
	
//USARTx1_TX
	GPIO_InitStructure.GPIO_Pin		=	USART1_TXD_PIN;					//GPIO���ţ�			ָ������1��TXD����
	GPIO_InitStructure.GPIO_Speed	=	GPIO_Speed_50MHz;				//GPIO����ʱ�ӣ�	IO��ʱ���ٶ�Ϊ50MHz��ģ������ģʽʱ���붨������ʱ�ӣ�
	GPIO_InitStructure.GPIO_Mode	=	GPIO_Mode_AF_PP;				//GPIO���������	����ʽ���ù��ܣ���������ߵ�ƽ����������͵�ƽ��	
	GPIO_Init(USART1_PORT,&GPIO_InitStructure);							//GPIO������Ч��	�����趨������ʼ���˿ڼ�TXD����
	
	
//USARTx1_RX
	GPIO_InitStructure.GPIO_Pin = USART1_RXD_PIN;						//GPIO���ţ�			ָ������1��RXD����
//	GPIO_InitStructure.GPIO_Speed	=	GPIO_Speed_50MHz;				//GPIO����ʱ�ӣ�	IO��ʱ���ٶ�Ϊ50MHz��ģ������ģʽʱ���붨������ʱ�ӣ�
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;		//GPIO���������	���븡��	
	GPIO_Init(USART1_PORT, &GPIO_InitStructure);						//GPIO������Ч��	�����趨������ʼ���˿ڼ�RXD����
}


/**
 ******************************************************************************
 ** \brief  ����1������������ж����á�
 **					����1��Ӧ���ض����ţ�����APBx2_peripheral(RCC_APBx2Periph_GPIOA)Ҳ��
 **					�ض���Ӧ��ϵ�������stm32f10x_rcc.h�ļ�L492��L559��
 **
** \param  bound�����ڲ����ʣ�ͨ��ȡֵ9600��115200
 **
 ** \retval ��
 **
 ******************************************************************************
**/
void USART1_Init(u32 bound)
{
	USART_InitTypeDef USART_InitStructure;										//����һ��USART������ṹ�壬���ڶ��������������
	NVIC_InitTypeDef 	NVIC_InitStructure;											//����һ��NVIC���жϿ�������������ṹ�壬���ڶ��������������

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);		//ʹ��USART1
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);			//�������ڸ��ù��ܣ�Ҫ��������ʱ��
	
	
//����USART1�Ľṹ��
	USART_DeInit(USART1);																														//���USART1������ΪĬ��ֵ
	USART_InitStructure.USART_BaudRate = bound;																			//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;											//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;													//1��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;															//����żУ��λ
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;									//�շ�ģʽ		
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��Ӳ������������
	USART_Init(USART1, &USART_InitStructure);																				//��ʼ��USARTx

	
//�����жϿ������Ľṹ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);					//�����жϿ�����������ռ���飨���main()���������У���ע�ʹ˴���
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;				//USART1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;	//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;			//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);													//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
	USART_ClearFlag(USART1, USART_FLAG_TXE);								//�����ж�֮ǰ�������һ���жϱ�־λ
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);					//����USART1�����ж�
	USART_ITConfig(USART1, USART_IT_TC, DISABLE);						//�ر�USART1�����жϡ��мǣ���ʼ��ʱ����ʹ�ܴ��ڷ����жϣ�����ᵼ�³�������ʱһֱ�ڽ������ж�


	USART_Cmd(USART1, ENABLE);															//ʹ��USART1
}


/**
 ******************************************************************************
 ** \brief  ����1���жϷ���������ν��յ���ʶ��0x0d��0x0a��ʾ����֡�������
 **
 ** \param  ��
 **
 ** \retval ��
 **
 ******************************************************************************
**/
void USART1_IRQHandler(void)
{
	u8 Res;
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		Res = USART_ReceiveData(USART1);						//��ȡ���յ�������
		if(0 == (USART1_RX_STA & 0x8000))						//����δ���
		{
			if(USART1_RX_STA & 0x4000)								//��һ�ν��յ��˽�����ʶ��0x0D
			{
				if(Res != 0x0A)	
				{
					USART1_RX_BUF[USART1_RX_STA & 0X3FFF]	=	Res;													//�洢���ν��յ������ݣ�����֡���Ȳ�����0x3FFF
					USART1_RX_STA++;
					if((USART1_RX_STA & 0X3FFF) > (USART1_REC_LEN-1))	USART1_RX_STA = 0;	//�ѽ������ݳ��ȳ������ƣ������������ݣ����¿�ʼ����
				}
				else {
					USART1_RX_STA |= 0x8000;	//���������
					if(USART1_RX_BUF[0] == '$' && USART1_RX_BUF[4] == 'L')								//ȷ���յ�"$GNGLL"��һ֡����
					{
						memcpy(Save_Data.GPS_Buffer, USART1_RX_BUF, GPS_Buffer_Length); 		//��������
						Save_Data.isGetData = 1;
					}	
				}
			}
			else	//��һ��δ�յ�0������ʶ��x0D
			{	
				if(0x0D == Res)	USART1_RX_STA |= 0x4000;																//��Ǳ��ν��յ���0x0D
				else
				{
					USART1_RX_BUF[USART1_RX_STA & 0X3FFF]	=	Res;													//�洢���ν��յ������ݣ�����֡���Ȳ�����0x3FFF
					USART1_RX_STA++;
					if((USART1_RX_STA & 0X3FFF) > (USART1_REC_LEN-1))	USART1_RX_STA = 0;	//�ѽ������ݳ��ȳ������ƣ������������ݣ����¿�ʼ����
				}
			}
		}
  } 
}


/**
 ******************************************************************************
 ** \brief  ��NMEA��ʽ��GPS�źŽ��н��������������ľ�γ�����ݺ�UTC
 **
 ** \param  ��
 **
 ** \retval ��
 **
 ******************************************************************************
**/
void GPGBuffer_ParseNMEA(void)
{
	char *subString =NULL;
	char *subStringNext =NULL;
	char i = 0;
	if (Save_Data.isGetData)	//���ݽ��ǰ��һ���ж��Ƿ���յ�������
	{
		for (i = 0; i <= 6; i++)
		{
			if (i == 0){
				if((subString = strstr(Save_Data.GPS_Buffer, ",")) == NULL) {
					printf("There is not a comma!\n");
					return;	//û���κζ��ţ���ʾ���յ�����������ֱ�Ӷ���
				}
				else{
					subString++;
					if((subStringNext = strstr(subString, ",")) != NULL){
						char usefullBuffer[2];
						switch(i)
						{
							case 1:memcpy(Save_Data.latitude, subString, subStringNext - subString);break;	//��ȡγ��
							case 2:memcpy(Save_Data.N_S, subString, subStringNext - subString);break;				//��ȡN/S
							case 3:memcpy(Save_Data.longitude, subString, subStringNext - subString);break;	//��ȡ������Ϣ
							case 4:memcpy(Save_Data.E_W, subString, subStringNext - subString);break;				//��ȡE/W
							case 5:memcpy(Save_Data.UTCTime, subString, subStringNext - subString);break;		//��ȡUTCʱ��
							case 6:memcpy(usefullBuffer, subString, subStringNext - subString);break;				//��ȡ������Ч��־
							default:;
						}
						subString = subStringNext;
						Save_Data.isParseData = 1;	//���ݽ����ɹ���־
						Save_Data.isUsefull = (usefullBuffer[0] == 'A'?1:0);//�ж���Ч��־
					}
					else printf("cut strings error!\n");
				}
			}
		}
	}
}


/**
 ******************************************************************************
 ** \brief  ���ַ��������ض����ȵ��ַ���ӡ����
 **
** \param  head��		��Ҫ��ӡ�ı�־��������ǿ���֣�һ������ַ������� "UTCtime:"
 **					ps��		�ַ���ָ����ַ�����
 **				 length��	��Ҫ��ӡ���ַ����ȣ����ַ���������ĵ�һλ��ʼ������0������
 **
 ** \retval ��
 **
 ******************************************************************************
**/
void printps(char *head, char *ps, u16 length)
{
	u16 i = 0;
	if(NULL != head) printf("%s",head);
	for(i = 0; i<length;i++)
		printf("%c",*(ps+i));
	printf("\n");	//��β�Ļ��з�
}

/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/
