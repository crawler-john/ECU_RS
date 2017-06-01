#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 

typedef enum
{ 
    EN_RECV_ST_GET_HEAD         = 0,		//��������ͷ
    EN_RECV_ST_GET_LEN          = 1,	//�������ݳ���   �������ݲ��ֵĳ���Ϊ���յ����ȼ�ȥ12���ֽ�
    EN_RECV_ST_GET_DATA         = 2,	//�������ݲ�������
    EN_RECV_ST_GET_END          = 3		//����END��β��־
} eRecvSM;// receive state machin

#define USART_REC_LEN  			512  	//�����������ֽ��� 200
#define EN_USART2_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA;         		//����״̬���	


int WIFI_SendData(char *data, int num);
void uart_init(u32 bound);
#endif


