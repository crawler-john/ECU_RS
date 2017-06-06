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

#define USART_REC_LEN  			1024  	//�����������ֽ��� 200
#define EN_USART2_RX 				1		//ʹ�ܣ�1��/��ֹ��0������1����
	  	

extern unsigned char WIFI_RecvData[USART_REC_LEN];
extern  unsigned char WIFI_Recv_Event;

unsigned short packetlen(unsigned char *packet);
int WIFI_SendData(char *data, int num);
void WIFI_GetEvent(int *messageLen);
void uart_init(u32 bound);

int AT(void);
int AT_ENTM(void);
int AT_WAKEY(char *NewPasswd);
int WIFI_ChangePasswd(char *NewPasswd);
int WIFI_Reset(void);
int AT_Z(void);
#endif


