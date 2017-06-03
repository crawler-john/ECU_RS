#ifndef __RFM300H_H__
#define __RFM300H_H__

#include "CMT2300.h"

int RFM300_Bind_Uid(char *ECUID,char *UID,int channel,int rate);
int RFM300_Heart_Beat(char *ECUID,char *UID,char *data);
int RFM300_IO_Init(char *ECUID,char *UID,char *data,int value);
int RFM300_Set_Uid(char *ECUID,char *UID,int channel,int rate,char *NewUid);
#endif /*__RFM300H_H__*/

