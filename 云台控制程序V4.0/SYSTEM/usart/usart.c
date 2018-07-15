#include "sys.h"
#include "usart.h"
#include "delay.h"
#include "gimbalcontrol.h"
#include "tim5message.h"
#include "as14ttl.h"
#include "pid.h"
#include "report.h"
u8 keyValue=0x01;  //Ĭ���Զ�ģʽ

////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_UCOS
#include "includes.h"					//ucos ʹ��	  
#endif


//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 


 
void uart_init(u32 bound)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;	  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ���Լ����ù���ʱ��
   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);  
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);  

  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART1, &USART_InitStructure); //��ʼ������
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�
	USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ��� 
	
}

u8 FirstFlag=0;
static u8 index=0;
static u8 USART_RX[20];
 void USART1_IRQHandler(void)  
{
	u8 Res;
	u8 i;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
		{
			Res =USART_ReceiveData(USART1);//(USART1->DR);	//��ȡ���յ�������
			if(Res==0x0a&&USART_RX[index-1]==0x0d)
			{				
				if(index-1==0x06)  //Sum_Check(USART_RX,6,USART_RX[5]
				{
					 if(USART_RX[2]==0x01&&USART_RX[4]==0x11)   //ͨ�ż��
								{
									index=0;
									Send_Check(USART_RX[2],USART_RX[5]);
									for(i=0;i<index+1;i++)  {USART_RX[i]=0;}
								}
							 else if(USART_RX[2]==0x03)     //��ȡ��ֵ
								{
									index=0;
									keyValue= USART_RX[4];  
                  Send_Check(USART_RX[2],USART_RX[5]);
			          	for(i=0;i<index+1;i++){USART_RX[i]=0;}
		            }
				}
				else if(index-1==0x0b)
				{
					FirstFlag=1;
					index=0;
					Send_Check(USART_RX[2],USART_RX[10]);
						 ADC_Value[0] = (USART_RX[4] & 0xff)<<8 | USART_RX[5];
						 ADC_Value[1] = (USART_RX[6] & 0xff)<<8 | USART_RX[7];
	           ADC_Value[2] = (USART_RX[8] & 0xff)<<8 | USART_RX[9];
				//	DEBUG_PRINT("%-5d %-5d %-5d\r\n", ADC_Value[0], ADC_Value[1], ADC_Value[2]);
					for(i=0;i<index+1;i++)  {USART_RX[i]=0;}        //������ڻ��� 
				}
				else  //����ң����Ϣ
				{
					index=0;
					for(i=0;i<index+1;i++)  {USART_RX[i]=0;}
				}
					
			//	for(;i<index;i++){USART_RX[i]=0;}
			//	index=0;
			}
			else
			{
				USART_RX[index]=Res;
				index++;
			}
		}
}


//��У��
//�ɹ�����1��ʧ�ܷ���0

u8 Sum_Check(u8 *buf,u8 len,u8 checksum)
{
	u8 i,sum=0;
  for(i=0;i<len;i++)  sum+=buf[i];
	if(checksum==sum) return 1;
	else return 0;
}

/*****************  ����һ���ֽ� **********************/
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
{
	/* ����һ���ֽ����ݵ�USART */
	USART_SendData(pUSARTx,ch);
		
	/* �ȴ��������ݼĴ���Ϊ�� */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

/****************** ����8λ������ ************************/
void Usart_SendArray( USART_TypeDef * pUSARTx, uint8_t *array, uint16_t num)
{
  uint8_t i;
	
	for(i=0; i<num; i++)
  {
	    /* ����һ���ֽ����ݵ�USART */
	    Usart_SendByte(pUSARTx,array[i]);	
  
  }
	/* �ȴ�������� */
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET);
}

/*****************  �����ַ��� **********************/
void Usart_SendString( USART_TypeDef * pUSARTx, char *str)
{
	unsigned int k=0;
  do 
  {
      Usart_SendByte( pUSARTx, *(str + k) );
      k++;
  } while(*(str + k)!='\0');
  
  /* �ȴ�������� */
  while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET)
  {}
}

/*****************  ����һ��16λ�� **********************/
void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch)
{
	uint8_t temp_h, temp_l;
	
	/* ȡ���߰�λ */
	temp_h = (ch&0XFF00)>>8;
	/* ȡ���Ͱ�λ */
	temp_l = ch&0XFF;
	
	/* ���͸߰�λ */
	USART_SendData(pUSARTx,temp_h);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
	
	/* ���͵Ͱ�λ */
	USART_SendData(pUSARTx,temp_l);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}



//pid���ڲ������Ժ���
/*
static u8 index=0;
u8 USART_RX[20];
void USART1_IRQHandler(void)  
{
	u8 Res;
	u8 i;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
		{
			Res =USART_ReceiveData(USART1);//(USART1->DR);	//��ȡ���յ�������
			if(Res==0x0a&&USART_RX[index-1]==0x0d)
			{
				//Usart_SendArray( USART1,USART_RX, index);
				if(index-1==0x09)
				{
					for(i=0;i<index-1;i++){ debugPara[i]=USART_RX[i];}
			    for(i=0;i<index+1;i++){USART_RX[i]=0;}		            
				}
				else  //����ң����Ϣ
				{
					index=0;
					for(i=0;i<index+1;i++)  {USART_RX[i]=0;}
				}
					
			//	for(;i<index;i++){USART_RX[i]=0;}
			//	index=0;
			}
			else
			{
				USART_RX[index]=Res;
				index++;
			}
		}
}
*/



