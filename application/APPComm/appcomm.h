#ifndef __APPCOMM_H__
#define __APPCOMM_H__

#include "usart.h"


typedef enum
{ 
    COMMAND_BASEINFO         	= 1,		//接收数据头
    COMMAND_SYSTEMINFO       	= 2,	//接收数据长度   其中数据部分的长度为接收到长度减去12个字节
    COMMAND_SETNETWORK      	= 3,	//接收数据部分数据
    COMMAND_SETCHANNEL       	= 4,		//接收END结尾标志
		COMMAND_SETWIFIPASSWORD 	= 5
} eCommandID;// receive state machin



//解析收到的数据
int Resolve_RecvData(char *RecvData,int* Data_Len,int* Command_Id);
//01 命令回应
void APP_Response_BaseInfo(char *ECU_NO,char *TYPE,char *SIGNAL_LEVEL,char *SIGNAL_CHANNEL,int Length,char * Version);
//02 命令回应

//03 命令回应
void APP_Response_SetNetwork(unsigned char result);

//04 命令回应
void APP_Response_SetChannel(unsigned char mapflag,char *SIGNAL_CHANNEL,char *SIGNAL_LEVEL);

//05 命令回应
void APP_Response_SetWifiPassword(unsigned char result);


#endif /*__APPCOMM_H__*/
