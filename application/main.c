#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "Flash_24L512.h"
//ALIENTEKս��STM32������ʵ��4
//����ʵ��  
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾ 
 int main(void)
 {	

	 
  unsigned char sendbuff[11] = "qasdasdasd";
	unsigned char recvbuff[11];
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(9600);	 //���ڳ�ʼ��Ϊ9600
	
	I2C_Init();
	Write_24L512_nByte(0x000000,10,sendbuff);
	 printf("\nWrite_24L512_nByte:%s \n",sendbuff);
	 
	Read_24L512_nByte(0x000000,10,recvbuff);
	printf("\nRead_24L512_nByte:%s \n",recvbuff);
	 
 }

