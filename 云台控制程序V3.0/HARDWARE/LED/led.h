#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
#include "stm32f10x.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
#define BIT_SET(port, pin)		(port->BSRR = pin)
#define BIT_RESET(port, pin)	(port->BRR = pin)
#define BIT_REVERSE(port, pin)	(port->ODR ^= pin)

#define LED0_ON			BIT_SET(GPIOB, GPIO_Pin_12)
#define LED0_OFF		BIT_RESET(GPIOB, GPIO_Pin_12)
#define LED0_TOGGLE		BIT_REVERSE(GPIOB, GPIO_Pin_12)
#define LED1_ON			BIT_SET(GPIOB, GPIO_Pin_13)
#define LED1_OFF		BIT_RESET(GPIOB, GPIO_Pin_13)
#define LED1_TOGGLE		BIT_REVERSE(GPIOB, GPIO_Pin_13)

void LED_Init(void);//��ʼ��

		 				    
#endif
