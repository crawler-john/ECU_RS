#include "RFM300H.h"
#include "SEGGER_RTT.h"
#include "delay.h"
#include "string.h"

//#define CMT2300_DEBUG

int RFM300_Bind_Uid(char *ECUID,char *UID,char channel,char rate)
{
	int i,check = 0;
	unsigned char RF_leng = 0;
	char Senddata[32] = {'\0'};
	char Recvdata[32] = {'\0'};

	Senddata[0]=0xFB;
	Senddata[1]=0xFB;
	Senddata[2]=0x15;
	Senddata[3]=0x08;
	Senddata[4]=ECUID[0];
	Senddata[5]=ECUID[1];
	Senddata[6]=ECUID[2];
	Senddata[7]=ECUID[3];
	Senddata[8]=ECUID[4];
	Senddata[9]=ECUID[5];		
	Senddata[10]=UID[0];
	Senddata[11]=UID[1];
	Senddata[12]=UID[2];
	Senddata[13]=UID[3];
	Senddata[14]=UID[4];
	Senddata[15]=UID[5];
	Senddata[16]=channel;
	Senddata[17]=rate;
	Senddata[18]=0x00;
	Senddata[19]=0x00;
	Senddata[20]=0x00;
	Senddata[21]=0x00;
	Senddata[22]=0x00;
	Senddata[23]=0x00;
	
	for(i=3;i<(3+Senddata[2]);i++)
		check=check+Senddata[i];
		
	Senddata[24] = check/256;
	Senddata[25] = check%256;
	
	Senddata[26]=0xFE;
	Senddata[27]=0xFE;
#ifdef CMT2300_DEBUG
	for(i=0;i<28;i++)
	{
		SEGGER_RTT_printf(0, "%02x ",Senddata[i]);
	}
	SEGGER_RTT_printf(0, "\n",Senddata[i]);
#endif
	for(i=0;i<3;i++){
		SendMessage((unsigned char *)Senddata,28);
		RF_leng = GetMessage((unsigned char *)Recvdata);
		if((RF_leng==28)&&
			(Recvdata[3]==0xD8)&&		//表示绑定成功
			(!strncmp(&Senddata[4],&Recvdata[4],20)))
		{
			SEGGER_RTT_printf(0, "RFM300_Bind_Uid\n");
				return 1;
		}
		else
		{
			delay_ms(20);
			continue;
		}
			
	}
	return 0;
}

int RFM300_Heart_Beat(char *ECUID,char *UID,char *mos_Status,char *IO_InitStatus,unsigned short *TimeoutNum,unsigned short * offNum,char *Ver)
{
	int i,check = 0;
	unsigned char RF_leng = 0;
	char Senddata[32] = {'\0'};
	char Recvdata[32] = {'\0'};
	
	Senddata[0]=0xFB;
	Senddata[1]=0xFB;
	Senddata[2]=0x15;	
	Senddata[3]=0x00;
	Senddata[4]=ECUID[0];
	Senddata[5]=ECUID[1];
	Senddata[6]=ECUID[2];
	Senddata[7]=ECUID[3];
	Senddata[8]=ECUID[4];
	Senddata[9]=ECUID[5];		
	Senddata[10]=UID[0];
	Senddata[11]=UID[1];
	Senddata[12]=UID[2];
	Senddata[13]=UID[3];
	Senddata[14]=UID[4];
	Senddata[15]=UID[5];
	Senddata[16]=0x00;
	Senddata[17]=0x00;
	Senddata[18]=0x00;
	Senddata[19]=0x00;
	Senddata[20]=0x00;
	Senddata[21]=0x00;
	Senddata[22]=0x00;
	Senddata[23]=0x00;
	
	for(i=3;i<(3+Senddata[2]);i++)
		check=check+Senddata[i];
	Senddata[24] = check/256;
	Senddata[25] = check%256;
	Senddata[26]=0xFE;
	Senddata[27]=0xFE;

#ifdef CMT2300_DEBUG
	for(i=0;i<28;i++)
	{
		SEGGER_RTT_printf(0, "%02x ",Senddata[i]);
	}
	SEGGER_RTT_printf(0, "\n");
#endif
	
	for(i=0;i<3;i++){
		SendMessage((unsigned char *)Senddata,28);

		RF_leng = GetMessage((unsigned char *)Recvdata);
		if((RF_leng==28)&&
			(Recvdata[3]==0xD0)&&
		(!strncmp(&Senddata[4],&Recvdata[4],12)))
		{

			*mos_Status = Recvdata[16];
			*IO_InitStatus = Recvdata[17];
			*TimeoutNum = Recvdata[18]*256+Recvdata[19];
			*offNum = Recvdata[20]*256 +Recvdata[21];
			*Ver =Recvdata[22];
			SEGGER_RTT_printf(0, "RFM300_Heart_Beat  %d %d %d %d %d \n",*mos_Status,*IO_InitStatus,*TimeoutNum,*offNum,*Ver);
			return 1;
		}
		else
		{
			delay_ms(20);
			continue;
		}
			
	}
	return 0;
}

int RFM300_IO_Init(char *ECUID,char *UID,char IO_Status,char *mos_Status,char *IO_InitStatus,unsigned short *TimeoutNum,unsigned short * offNum,char *Ver)
{
	int i,check = 0;
	unsigned char RF_leng = 0;
	char Senddata[32] = {'\0'};
	char Recvdata[32] = {'\0'};
	
	Senddata[0]=0xFB;
	Senddata[1]=0xFB;
	Senddata[2]=0x15;	
	Senddata[3]=IO_Status;
	Senddata[4]=ECUID[0];
	Senddata[5]=ECUID[1];
	Senddata[6]=ECUID[2];
	Senddata[7]=ECUID[3];
	Senddata[8]=ECUID[4];
	Senddata[9]=ECUID[5];		
	Senddata[10]=UID[0];
	Senddata[11]=UID[1];
	Senddata[12]=UID[2];
	Senddata[13]=UID[3];
	Senddata[14]=UID[4];
	Senddata[15]=UID[5];
	Senddata[16]=0x00;
	Senddata[17]=0x00;
	Senddata[18]=0x00;
	Senddata[19]=0x00;
	Senddata[20]=0x00;
	Senddata[21]=0x00;
	Senddata[22]=0x00;
	Senddata[23]=0x00;
	
	for(i=3;i<(3+Senddata[2]);i++)
		check=check+Senddata[i];
	Senddata[24] = check/256;
	Senddata[25] = check%256;
	Senddata[26]=0xFE;
	Senddata[27]=0xFE;
	
#ifdef CMT2300_DEBUG
	for(i=0;i<28;i++)
	{
		SEGGER_RTT_printf(0, "%02x ",Senddata[i]);
	}
	SEGGER_RTT_printf(0, "\n");
#endif
	
	for(i=0;i<3;i++){
		SendMessage((unsigned char *)Senddata,28);

		RF_leng = GetMessage((unsigned char *)Recvdata);
		if((RF_leng==28)&&
			(Recvdata[3]==0xD1)&&
			(!strncmp(&Senddata[4],&Recvdata[4],12)))
		{
			*mos_Status = Recvdata[16];
			*IO_InitStatus = Recvdata[17];
			*TimeoutNum = Recvdata[18]*256+Recvdata[19];
			*offNum = Recvdata[20]*256 +Recvdata[21];
			*Ver =Recvdata[22] ;
			SEGGER_RTT_printf(0, "RFM300_IO_Init  %d %d %d %d %d \n",*mos_Status,*IO_InitStatus,*TimeoutNum,*offNum,*Ver);

			return 1;			
		}
		else
			continue;
	}
	return 0;
}

int RFM300_Set_Uid(char *ECUID,char *UID,int channel,int rate,char *NewUid,char *SaveChannel,char *SaveRate)
{
	int i,check = 0;
	unsigned char RF_leng = 0;
	char Senddata[32] = {'\0'};
	char Recvdata[32] = {'\0'};

	Senddata[0]=0xFB;
	Senddata[1]=0xFB;
	Senddata[2]=0x15;	
	Senddata[3]=0x09;
	Senddata[4]=ECUID[0];
	Senddata[5]=ECUID[1];
	Senddata[6]=ECUID[2];
	Senddata[7]=ECUID[3];
	Senddata[8]=ECUID[4];
	Senddata[9]=ECUID[5];		
	Senddata[10]=UID[0];
	Senddata[11]=UID[1];
	Senddata[12]=UID[2];
	Senddata[13]=UID[3];
	Senddata[14]=UID[4];
	Senddata[15]=UID[5];
	Senddata[16]=channel;
	Senddata[17]=rate;
	Senddata[18]=NewUid[0];
	Senddata[19]=NewUid[1];
	Senddata[20]=NewUid[2];
	Senddata[21]=NewUid[3];
	Senddata[22]=NewUid[4];
	Senddata[23]=NewUid[5];
	
	for(i=3;i<(3+Senddata[2]);i++)
		check=check+Senddata[i];
	Senddata[24] = check/256;
	Senddata[25] = check%256;

	Senddata[26]=0xFE;
	Senddata[27]=0xFE;

#ifdef CMT2300_DEBUG
	for(i=0;i<28;i++)
	{
		SEGGER_RTT_printf(0, "%02x ",Senddata[i]);
	}
	SEGGER_RTT_printf(0, "\n",Senddata[i]);
#endif
	
	for(i=0;i<3;i++){
		SendMessage((unsigned char *)Senddata,28);

		RF_leng = GetMessage((unsigned char *)Recvdata);
		if((RF_leng==28)&&
			(Recvdata[3]==0xD9)&&
			(!strncmp(&Senddata[4],&Recvdata[4],6))
			&&(!strncmp(&Senddata[18],&Recvdata[10],6)))
		{
				*SaveChannel = Recvdata[16];
				*SaveRate = Recvdata[17];
				SEGGER_RTT_printf(0, "RFM300_Set_Uid  %d %d %d %d %d \n",*SaveChannel,*SaveRate);

				return 1;
		}
		else
				continue;
	}
	return 0;
}
