/*****************�ö�ʱ����ң����������̨��Ϣ*****************/

#include "tim5message.h"
u8 fun; //������
u8 len;  //����֡����
static u8 time_flag=0;  //��������ʱ���־��2s����һ��
 void TIM5_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM5��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM5�ж�,��������ж�

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;  //TIM5�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���


	TIM_Cmd(TIM5, ENABLE);  //ʹ��TIMx	
}

//��ʱ��5�жϷ������  
void TIM5_IRQHandler(void)   //TIM5�ж�
{
	if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)  //���TIM5�����жϷ������
		{
		TIM_ClearITPendingBit(TIM5, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
			time_flag++;
			if(time_flag==0x04)
			{
				time_flag=0;
				Send_Remote((Mpu6050_Roll*100),(Mpu6050_Pitch*100),(Mpu6050_Yaw*100));
			}
		}
}


//����Ӧ��ң����
void Send_Check(u8 check_head,u8 check_sum)
{
	u8 send_buf[9];
	u8 i;
	fun=0x02;
	len=0x02;
   for (i = 0;i<9;i++)send_buf[i] = 0;//��0
	send_buf[0]=0xaa;
	send_buf[1]=0xaa;
	send_buf[2]=fun;
	send_buf[3]=len;
	send_buf[4]=check_head;   //
	send_buf[5]=check_sum;
	for(i=0;i<6;i++) send_buf[6]+=send_buf[i]; //�����У��
	send_buf[7]=0x0d;
	send_buf[8]=0x0a;
	Usart_SendArray(USART1,send_buf,9);
}

/**
  **********************************
  * @brief  ͨ������1�ϱ���������̬���ݸ�����
  * @param  
						roll:�����.��λ0.01�ȡ� -18000 -> 18000 ��Ӧ -180.00  ->  180.00��
						pitch:������.��λ 0.01�ȡ�-9000 - 9000 ��Ӧ -90.00 -> 90.00 ��
						yaw:�����.��λΪ0.01�� 0 -> 3600  ��Ӧ 0 -> 360.0��
  * @retval None
  **********************************
*/
//������̨״̬��ϵ��ң����
void Send_Remote(short roll,short pitch,short yaw)
{
	u8 send_buf[13];
	u8 i;
	fun=0x01;
	len=0x06;
	for (i = 0;i<13;i++)send_buf[i] = 0;//��0
	send_buf[0]=0xaa;
	send_buf[1]=0xaa;
	send_buf[2]=fun;
	send_buf[3]=len;
	send_buf[4]=(roll>>8)&0xFF;
	send_buf[5]=roll&0xFF;
	send_buf[6]=(pitch>>8)&0xFF;
	send_buf[7]=pitch&0xFF;
	send_buf[8]=(yaw>>8)&0xFF;
	send_buf[9]=yaw&0xFF;
	for(i=0;i<10;i++) send_buf[10]+=send_buf[i];
	send_buf[11]=0x0d;
	send_buf[12]=0x0a;
	Usart_SendArray(USART1,send_buf,13);
} 


