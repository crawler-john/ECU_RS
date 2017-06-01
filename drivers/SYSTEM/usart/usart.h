#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 

typedef enum
{ 
    EN_RECV_ST_GET_HEAD         = 0,		//接收数据头
    EN_RECV_ST_GET_LEN          = 1,	//接收数据长度   其中数据部分的长度为接收到长度减去12个字节
    EN_RECV_ST_GET_DATA         = 2,	//接收数据部分数据
    EN_RECV_ST_GET_END          = 3		//接收END结尾标志
} eRecvSM;// receive state machin

#define USART_REC_LEN  			512  	//定义最大接收字节数 200
#define EN_USART2_RX 			1		//使能（1）/禁止（0）串口1接收
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;         		//接收状态标记	


int WIFI_SendData(char *data, int num);
void uart_init(u32 bound);
#endif


