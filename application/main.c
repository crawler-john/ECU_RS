#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "Flash_24L512.h"
#include "SEGGER_RTT.h"
#include "led.h"

//ALIENTEKս��STM32������ʵ��4
//����ʵ��  
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾ 
 int main(void)
 {	

	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	I2C_Init();
	rt_hw_led_init();
	
	while(1)
	{
		rt_hw_led_on();
		SEGGER_RTT_printf(0,"rt_hw_led_on\n");
		delay_ms(1000);
		rt_hw_led_off();
		SEGGER_RTT_printf(0,"rt_hw_led_off\n");	
		delay_ms(1000);
		
	}

	

	

	

	 
	

	 
 }

