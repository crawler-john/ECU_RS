#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "Flash_24L512.h"
#include "SEGGER_RTT.h"
#include "led.h"
#include "CMT2300.h"
#include "file.h"


	unsigned char mode = 0;
int main(void)
{	
	unsigned char eepromSenddata[11] = "YUNENG APS";
	unsigned char eepromRecvdata[11] = {'\0'};

	unsigned char ret = 0;

	delay_init();	    	 //延时函数初始化	  
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	I2C_Init();
	LED_init();
	KEY_Init();//IO初始化
	EXTIX_Init();
	//CMT2300_init();
	//Spi3Init();
	uart_init(57600);	
	SEGGER_RTT_printf(0, "init OK \n");
	
	
	
	ret = Write_Test("yuneng APS",10);
	SEGGER_RTT_printf(0, "Write_Test:   %d\n",ret);
	/*
	while(1)
	{
		Write_24L512_nByte(0x000000,10,eepromSenddata);
		SEGGER_RTT_printf(0, "eepromSenddata:   %s\n",eepromSenddata);

		Read_24L512_nByte(0x000001,10,eepromRecvdata);

		eepromRecvdata[10] = '\0';
		SEGGER_RTT_printf(0, "eepromRecvdata:   %s\n",eepromRecvdata);
		delay_ms(1000);
		delay_ms(1000);
		delay_ms(1000);
		delay_ms(1000);
		
		
	}	
	*/
	
	/*
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
		SetOperaStatus(MODE_STA_SLEEP);
		delay_ms(1000);
		mode=Spi3ReadReg(CMT23_MODE_STA);
		SEGGER_RTT_printf(0, "mode = %x\n",mode);
		delay_ms(300);	

	}
	*/
	/*
	while(1)
	{	
		LED_on();	
		delay_ms(1000);
		LED_off();	
		delay_ms(1000);			
	}
	*/
	
	/*	
	while(1)
	{	
		ret = WIFI_SendData((char *)eepromSenddata, 10);
		SEGGER_RTT_printf(0, "ret = %d\n",ret);
		delay_ms(1000);		
	}
	*/	
	
 }

