#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "mpu6050.h"
#include "boardmpu.h"
#include "motor.h"
#include "gimbalmath.h" 
#include <stdio.h>
#include "gimbalcontrol.h" 
#include "tim5message.h"
#include "report.h"
#include "usart3.h"
#include "mag3110.h"
/************************************************
 ALIENTEKս��STM32������ʵ��32
 MPU6050���ᴫ���� ʵ��
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ���ߣ��人ΰ���Ƽ����޹�˾
************************************************/


int main(void)
{	
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(9600);	 	//���ڳ�ʼ��Ϊ9600
	//uart3_init(9600);
	delay_init();	//��ʱ��ʼ�� 
	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
//	TIM5_Init(4999,7199);//500ms����һ���ж�
	while(BoardMpu_Init(MPU_SAMPLE_RATE, MPU_DLPF_SWITCH))
	{
		delay_ms(200);
	}		 //50Hz��ʹ�ܵ�ͨ�˲�  
	while(Mpu6050_Init(MPU_SAMPLE_RATE, MPU_DLPF_SWITCH))
	{
		delay_ms(200);
	}
	/*
	while(MAG3110_Init())
	{
		delay_ms(10);
	}
	*/
	Motor_Init();
  BiasInit();
	positionInit();
  while(1)
	{	
	   GimbalControl();
	}
}

 


