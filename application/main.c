#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "Flash_24L512.h"
#include "SEGGER_RTT.h"
#include "led.h"

//ALIENTEK战舰STM32开发板实验4
//串口实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司 
 int main(void)
 {	

	delay_init();	    	 //延时函数初始化	  
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
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

