#ifndef __EVENT_H__
#define __EVENT_H__
#include "variation.h"


extern char ECUID12[13];
extern char ECUID6[7];
extern char Signal_Level;
extern char Signal_Channel[3];
extern char Channel_char;
extern char IO_Init_Status;			//IO��ʼ״̬
extern char ver;						//�Ż����汾��
extern int vaildNum;				//��ǰ��Ч̨��
extern int curSequence;		//������ѵ������
extern inverter_info inverterInfo[MAXINVERTERCOUNT];
extern int Data_Len;
extern int Command_Id;
extern int ResolveFlag ;
extern int messageLen;
extern int messageUsart1Len;
extern int UART1_Data_Len;
extern int UART1_Command_Id;
extern int UART1_ResolveFlag;


void process_HeartBeatEvent(void);
void process_WIFIEvent(unsigned char * ID);
void process_UART1Event(void);
void process_KEYEvent(void);
void process_WIFI_RST(void);

#endif /*__EVENT_H__*/
