#ifndef __APPCOMM_H__
#define __APPCOMM_H__

#include "usart.h"


typedef enum
{ 
    COMMAND_BASEINFO         	= 1,		//��������ͷ
    COMMAND_SYSTEMINFO       	= 2,	//�������ݳ���   �������ݲ��ֵĳ���Ϊ���յ����ȼ�ȥ12���ֽ�
    COMMAND_SETNETWORK      	= 3,	//�������ݲ�������
    COMMAND_SETCHANNEL       	= 4,		//����END��β��־
		COMMAND_SETWIFIPASSWORD 	= 5
} eCommandID;// receive state machin



//�����յ�������
int Resolve_RecvData(char *RecvData,int* Data_Len,int* Command_Id);
//01 �����Ӧ
void APP_Response_BaseInfo(char *ECU_NO,char *TYPE,char *SIGNAL_LEVEL,char *SIGNAL_CHANNEL,int Length,char * Version);
//02 �����Ӧ

//03 �����Ӧ
void APP_Response_SetNetwork(unsigned char result);

//04 �����Ӧ
void APP_Response_SetChannel(unsigned char mapflag,char *SIGNAL_CHANNEL,char *SIGNAL_LEVEL);

//05 �����Ӧ
void APP_Response_SetWifiPassword(unsigned char result);


#endif /*__APPCOMM_H__*/
