#ifndef __INVERTER_H__
#define __INVERTER_H__
#include "variation.h"
extern char ECUID12[13];
extern char ECUID6[7];
extern char Signal_Level;
extern char Signal_Channel[3];
extern char New_Signal_Channel[3];
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

int init_ecu(void);
int init_inverter(inverter_info *inverter);
int add_inverter(inverter_info *inverter,int num,char *uidstring);
void bind_inverter(inverter_info *inverter);
void changeChannel_inverter(inverter_info *inverter,char channel);
void changeIOinit_inverter(inverter_info *inverter,char IO_Status);
#endif /*__INVERTER_H__*/
