#ifndef __EVENT_H__
#define __EVENT_H__
#include "variation.h"


extern char ECUID12[13];
extern char ECUID6[7];
extern char Signal_Level;
extern char Signal_Channel[3];
extern char New_Signal_Channel[3];
extern char IO_Init_Status;			//IO初始状态
extern char ver;						//优化器版本号
extern int vaildNum;				//当前有效台数
extern int curSequence;		//心跳轮训机器号
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
void process_WIFIEvent(void);
void process_UART1Event(void);
void process_KEYEvent(void);;
#endif /*__EVENT_H__*/
