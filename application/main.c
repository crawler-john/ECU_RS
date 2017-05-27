#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "Flash_24L512.h"
//ALIENTEK战舰STM32开发板实验4
//串口实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司 
 int main(void)
 {	

	 
  unsigned char sendbuff[11] = "qasdasdasd";
	unsigned char recvbuff[11];
	delay_init();	    	 //延时函数初始化	  
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(9600);	 //串口初始化为9600
	
	I2C_Init();
	Write_24L512_nByte(0x000000,10,sendbuff);
	 printf("\nWrite_24L512_nByte:%s \n",sendbuff);
	 
	Read_24L512_nByte(0x000000,10,recvbuff);
	printf("\nRead_24L512_nByte:%s \n",recvbuff);
	 
 }

