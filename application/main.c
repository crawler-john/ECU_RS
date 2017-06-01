#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "Flash_24L512.h"
#include "SEGGER_RTT.h"
#include "led.h"
#include "CMT2300.h"


 int main(void)
 {	
	unsigned char senddata[10] = {'\0'};
	unsigned char mode = 0;
	unsigned char ret = 0;
	senddata[0] = 0xAA;
	senddata[1] = 0xAA;
	senddata[2] = 0xAA;
	senddata[3] = 0xAA;
	senddata[4] = 0xAA;
	senddata[5] = 0xAA;
	senddata[6] = 0xAA;
	senddata[7] = 0xAA;
	senddata[8] = 0xAA;
	senddata[9] = 0xAA;
	delay_init();	    	 //延时函数初始化	  
	SEGGER_RTT_printf(0, "delay_init OK\n");
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	SEGGER_RTT_printf(0, "NVIC_Configuration OK\n");
	I2C_Init();
	SEGGER_RTT_printf(0, "I2C_Init OK \n");
	LED_init();
	//CMT2300_init();
	Spi3Init();
	SEGGER_RTT_printf(0, "CMT2300_init OK \n");
	uart_init(57600);	
	SEGGER_RTT_printf(0, "uart_init OK \n");
	while(1)
	{		
		//SEGGER_RTT_printf(0, "mode = %d\n",mode);
		//SendMessage(senddata,10);
		//SEGGER_RTT_printf(0, "SendMessage OK\n");
		//SetOperaStatus(MODE_STA_STBY);
		//delay_ms(100);
		//mode=Spi3ReadReg(CMT23_MODE_STA);
		//SEGGER_RTT_printf(0, "mode = %d\n",mode);
		//delay_ms(100);
		//SetOperaStatus(MODE_STA_STBY);
		//delay_ms(1000);
		//mode=Spi3ReadReg(CMT23_MODE_STA);
		//SEGGER_RTT_printf(0, "mode = %x\n",mode);
		//delay_ms(1000);	
		//LED_on();	
		//delay_ms(1000);
		//LED_off();
		//ret = WIFI_SendData((char *)senddata, 10);
		//SEGGER_RTT_printf(0, "ret = %d\n",ret);
		//delay_ms(1000);		
	}
	 
 }

