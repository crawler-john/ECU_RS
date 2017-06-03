#ifndef __FILE_H__
#define __FILE_H__
#include "Flash_24L512.h"

#define VERSION_ECU_RS    					"101"
#define SOFEWARE_VERSION						"RS1.0"
//地址
#define  ADDRESS_Test 							0x000000			//用于板子收发测试    			10字节
#define  ADDRESS_ECUID 							0x000020			//ECU-RS ID   需要我们转换	6字节
#define  ADDRESS_SIGNAL_LV 					0x000040			//信号强度									3字节
#define  ADDRESS_CHANNEL 						0x000060			//优化器配置信息--信道			2字节
#define  ADDRESS_RATE 							0x000080			//优化器配置信息--速率			2字节
#define  ADDRESS_IO_STATUS 					0x0000A0			//IO状态										1字节
#define  ADDRESS_IO_INIT_STATUS 		0x0000C0			//IO上电初始化状态					1字节
#define  ADDRESS_HEART_NUM 					0x0000E0			//接收心跳包次数						2字节
#define  ADDRESS_HEART_TIMEOUT_NUM 	0x000100			//心跳包超时次数						2字节
#define  ADDRESS_UID_NUM 						0x000110			//优化器数量								2字节
#define  ADDRESS_WIFI_PW 						0x000120			//WIFI密码									100字节
#define  ADDRESS_UID 								0x0001A0			//优化器ID									6字节一个优化器


int Write_Test(char *Data_Ptr,unsigned char Counter);				//测试
int Read_Test(char *Data_Ptr,unsigned char Counter);

int Write_ECUID(char *ECUID);													  		//ECU ID
int Read_ECUID(char *ECUID);

//将6位ECU ID转换为12位ECU ID
void transformECUID(char * ECUID6,char *ECUID12);

int Write_SIGNAL_LV(char *Signal_LV);          							//信号强度
int Read_SIGNAL_LV(char *Signal_LV);

int Write_CHANNEL(char *Channel);														//信道
int Read_CHANNEL(char *Channel);

int Write_RATE(char *Rate);																	//速度
int Read_RATE(char *Rate);

int Write_IO_STATUS(char *IO_Status);												//IO状态
int Read_IO_STATUS(char *IO_Status);

int Write_IO_INIT_STATU(char *IO_InitStatus);								//IO上电状态
int Read_IO_INIT_STATU(char *IO_InitStatus);

int Write_HEART_NUM(char *HeartNum);												//接受心跳包数量
int Read_HEART_NUM(char *HeartNum);

int Write_HEART_TIMEOUT_NUM(char *Heart_TimeoutNum);				//心跳包超时数量
int Read_HEART_TIMEOUT_NUM(char *Heart_TimeoutNum);

int Write_WIFI_PW(char *WIFIPasswd,unsigned char Counter);	//WIFI密码
int Read_WIFI_PW(char *WIFIPasswd,unsigned char Counter);

int Write_UID_NUM(char *UID_NUM);														//UID_NUM
int Read_UID_NUM(char *UID_NUM);

int Write_UID(char *UID,int n);															//UID
int Read_UID(char *UID,int n);							

#endif /*__FILE_H__*/
