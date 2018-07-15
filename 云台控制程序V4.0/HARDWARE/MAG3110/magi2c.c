
#include "magi2c.h"

/**
  **********************************
  * @brief  MPU6050��ʼ��
  * @param  None
  * @retval None
  **********************************
*/
void MAG_IIC_Init(void)
{
	/*
	PB14 SCL ,PB15 SDA
	*/
	GPIO_InitTypeDef GPIO_InitStructure;//����ṹ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB, GPIO_Pin_14 | GPIO_Pin_15);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA, GPIO_Pin_15);
	
}

/**
  **********************************
  * @brief  ����IIC��ʼ�ź�
  * @param  None
  * @retval None
  **********************************
*/
void MAG_IIC_Start(void)
{
	MAG_SDA_OUT();//sda�����
	MAG_IIC_SDA = 1;
	MAG_IIC_SCL = 1;
	MAG_IIC_DELAY();
	MAG_IIC_SDA = 0;//START:when CLK is high,DATA change form high to low 
	MAG_IIC_DELAY();
	MAG_IIC_SCL = 0;//ǯסI2C���ߣ�׼�����ͻ��������
}

/**
  **********************************
  * @brief  ����IICֹͣ�ź�
  * @param  None
  * @retval None
  **********************************
*/
void MAG_IIC_Stop(void)
{
	MAG_SDA_OUT();//sda�����
	MAG_IIC_SCL = 0;
	MAG_IIC_SDA = 0;//STOP:when CLK is high DATA change form low to high
	MAG_IIC_DELAY();
	MAG_IIC_SCL = 1;
	MAG_IIC_SDA = 1;//����I2C���߽����ź�
	MAG_IIC_DELAY();					   	
}

/**
  **********************************
  * @brief  �ȴ�Ӧ���ź�
  * @param  None
  * @retval 1������Ӧ��ʧ��	0������Ӧ��ɹ�
  **********************************
*/
unsigned char MAG_IIC_Wait_Ack(void)
{
	unsigned char ucErrTime = 0;
	MAG_SDA_IN();      //SDA����Ϊ����  
	MAG_IIC_SDA = 1;
	MAG_IIC_DELAY();
	MAG_IIC_SCL = 1;
	MAG_IIC_DELAY();
	while (MAG_READ_SDA)
	{
		ucErrTime++;
		if (ucErrTime>250)
		{
			MAG_IIC_Stop();
			return 1;
		}
	}
	MAG_IIC_SCL = 0;//ʱ�����0
	return 0;
} 

/**
  **********************************
  * @brief  ����Ӧ���ź�
  * @param  None
  * @retval None
  **********************************
*/
void MAG_IIC_Ack(void)
{
	MAG_IIC_SCL = 0;
	MAG_SDA_OUT();
	MAG_IIC_SDA = 0;
	MAG_IIC_DELAY();
	MAG_IIC_SCL = 1;
	MAG_IIC_DELAY();
	MAG_IIC_SCL = 0;
}

/**
  **********************************
  * @brief  ������Ӧ���ź�
  * @param  None
  * @retval None
  **********************************
*/
void MAG_IIC_NAck(void)
{
	MAG_IIC_SCL = 0;
	MAG_SDA_OUT();
	MAG_IIC_SDA = 1;
	MAG_IIC_DELAY();
	MAG_IIC_SCL = 1;
	MAG_IIC_DELAY();
	MAG_IIC_SCL = 0;
}					 				     

/**
  **********************************
  * @brief  ����һ���ֽ�
  * @param  None
  * @retval None
  **********************************
*/
void MAG_IIC_Send_Byte(unsigned char txd)
{                        
	unsigned char t;
	MAG_SDA_OUT();
	MAG_IIC_SCL = 0;//����ʱ�ӿ�ʼ���ݴ���
	for (t = 0;t<8;t++)
	{
		MAG_IIC_SDA = (txd & 0x80) >> 7;
		txd <<= 1;
		MAG_IIC_SCL = 1;
		MAG_IIC_DELAY();
		MAG_IIC_SCL = 0;
		MAG_IIC_DELAY();
	}
} 	    

/**
  **********************************
  * @brief  ��һ���ֽ�
  * @param  None
  * @retval None
  **********************************
*/
unsigned char MAG_IIC_Read_Byte(unsigned char ack)
{
	unsigned char i, receive = 0;
	MAG_SDA_IN();//SDA����Ϊ����
	for (i = 0;i<8;i++)
	{
		MAG_IIC_SCL = 0;
		MAG_IIC_DELAY();
		MAG_IIC_SCL = 1;
		receive <<= 1;
		if (MAG_READ_SDA)receive++;
		MAG_IIC_DELAY();
	}
	if (!ack)
		MAG_IIC_NAck();//����nACK
	else
		MAG_IIC_Ack(); //����ACK   
	return receive;
}
/**
  **********************************
  * @brief  MPU6050дһ���ֽ�
	* @param  reg:�Ĵ�����ַ data:����
  * @retval None
  **********************************
*/
unsigned char MAG_Write_Byte(unsigned char reg, unsigned char data)
{
	MAG_IIC_Start();
	MAG_IIC_Send_Byte((MAG3110_IIC_ADDRESS << 1) | 0);//����������ַ+д����
	if (MAG_IIC_Wait_Ack())	//�ȴ�Ӧ��
	{
		MAG_IIC_Stop();
		return 1;
	}
	MAG_IIC_Send_Byte(reg);	//д�Ĵ�����ַ
	MAG_IIC_Wait_Ack();		//�ȴ�Ӧ�� 
	MAG_IIC_Send_Byte(data);//��������
	if (MAG_IIC_Wait_Ack())	//�ȴ�ACK
	{
		MAG_IIC_Stop();
		return 1;
	}
	MAG_IIC_Stop();
	return 0;
}

/**
  **********************************
  * @brief  MPU6050��һ���ֽ�
	* @param  reg:�Ĵ�����ַ
  * @retval None
  **********************************
*/
unsigned char MAG_Read_Byte(unsigned char reg)
{
	unsigned char res;
	MAG_IIC_Start();
	MAG_IIC_Send_Byte((MAG3110_IIC_ADDRESS << 1) | 0);//����������ַ+д����	
	MAG_IIC_Wait_Ack();		//�ȴ�Ӧ�� 
	MAG_IIC_Send_Byte(reg);	//д�Ĵ�����ַ
	MAG_IIC_Wait_Ack();		//�ȴ�Ӧ��
	MAG_IIC_Start();
	MAG_IIC_Send_Byte((MAG3110_IIC_ADDRESS << 1) | 1);//����������ַ+������	
	MAG_IIC_Wait_Ack();		//�ȴ�Ӧ�� 
	res = MAG_IIC_Read_Byte(0);//��ȡ����,����nACK 
	MAG_IIC_Stop();			//����һ��ֹͣ���� 
	return res;
}

