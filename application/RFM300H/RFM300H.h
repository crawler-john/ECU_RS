#ifndef __RFM300H_H__
#define __RFM300H_H__

#include "CMT2300.h"

int RFM300_Bind_Uid(char *ECUID,char *UID,char channel,char rate);
int RFM300_Heart_Beat(char *ECUID,char *UID,char *mos_Status,char *IO_InitStatus,unsigned short *TimeoutNum,unsigned short * offNum,char *Ver);
int RFM300_IO_Init(char *ECUID,char *UID,char IO_Status,char *mos_Status,char *IO_InitStatus,unsigned short *TimeoutNum,unsigned short * offNum,char *Ver);
int RFM300_Set_Uid(char *ECUID,char *UID,int channel,int rate,char *NewUid,char *SaveChannel,char *SaveRate);
#endif /*__RFM300H_H__*/

