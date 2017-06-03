#ifndef __FILE_H__
#define __FILE_H__
#include "Flash_24L512.h"

#define VERSION_ECU_RS    					"101"
#define SOFEWARE_VERSION						"RS1.0"
//��ַ
#define  ADDRESS_Test 							0x000000			//���ڰ����շ�����    			10�ֽ�
#define  ADDRESS_ECUID 							0x000020			//ECU-RS ID   ��Ҫ����ת��	6�ֽ�
#define  ADDRESS_SIGNAL_LV 					0x000040			//�ź�ǿ��									3�ֽ�
#define  ADDRESS_CHANNEL 						0x000060			//�Ż���������Ϣ--�ŵ�			2�ֽ�
#define  ADDRESS_RATE 							0x000080			//�Ż���������Ϣ--����			2�ֽ�
#define  ADDRESS_IO_STATUS 					0x0000A0			//IO״̬										1�ֽ�
#define  ADDRESS_IO_INIT_STATUS 		0x0000C0			//IO�ϵ��ʼ��״̬					1�ֽ�
#define  ADDRESS_HEART_NUM 					0x0000E0			//��������������						2�ֽ�
#define  ADDRESS_HEART_TIMEOUT_NUM 	0x000100			//��������ʱ����						2�ֽ�
#define  ADDRESS_UID_NUM 						0x000110			//�Ż�������								2�ֽ�
#define  ADDRESS_WIFI_PW 						0x000120			//WIFI����									100�ֽ�
#define  ADDRESS_UID 								0x0001A0			//�Ż���ID									6�ֽ�һ���Ż���


int Write_Test(char *Data_Ptr,unsigned char Counter);				//����
int Read_Test(char *Data_Ptr,unsigned char Counter);

int Write_ECUID(char *ECUID);													  		//ECU ID
int Read_ECUID(char *ECUID);

//��6λECU IDת��Ϊ12λECU ID
void transformECUID(char * ECUID6,char *ECUID12);

int Write_SIGNAL_LV(char *Signal_LV);          							//�ź�ǿ��
int Read_SIGNAL_LV(char *Signal_LV);

int Write_CHANNEL(char *Channel);														//�ŵ�
int Read_CHANNEL(char *Channel);

int Write_RATE(char *Rate);																	//�ٶ�
int Read_RATE(char *Rate);

int Write_IO_STATUS(char *IO_Status);												//IO״̬
int Read_IO_STATUS(char *IO_Status);

int Write_IO_INIT_STATU(char *IO_InitStatus);								//IO�ϵ�״̬
int Read_IO_INIT_STATU(char *IO_InitStatus);

int Write_HEART_NUM(char *HeartNum);												//��������������
int Read_HEART_NUM(char *HeartNum);

int Write_HEART_TIMEOUT_NUM(char *Heart_TimeoutNum);				//��������ʱ����
int Read_HEART_TIMEOUT_NUM(char *Heart_TimeoutNum);

int Write_WIFI_PW(char *WIFIPasswd,unsigned char Counter);	//WIFI����
int Read_WIFI_PW(char *WIFIPasswd,unsigned char Counter);

int Write_UID_NUM(char *UID_NUM);														//UID_NUM
int Read_UID_NUM(char *UID_NUM);

int Write_UID(char *UID,int n);															//UID
int Read_UID(char *UID,int n);							

#endif /*__FILE_H__*/
