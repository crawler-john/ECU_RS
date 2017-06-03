#include "RFM300H.h"
#include "SEGGER_RTT.h"
#include "delay.h"
#include "string.h"

int RFM300_Bind_Uid(char *ECUID,char *UID,int channel,int rate)
{
	int i;
	extern unsigned char RFM300H_SW;
	unsigned char RF_leng = 0;
	char eepromSenddata[32] = {'\0'};
	char eepromRecvdata[32] = {'\0'};

	eepromSenddata[0]=0xFB;
	eepromSenddata[1]=0xFB;
	eepromSenddata[2]=0x15;	
	eepromSenddata[3]=0x08;
	eepromSenddata[4]=ECUID[0];
	eepromSenddata[5]=ECUID[1];
	eepromSenddata[6]=ECUID[2];
	eepromSenddata[7]=ECUID[3];
	eepromSenddata[8]=ECUID[4];
	eepromSenddata[9]=ECUID[5];		
	eepromSenddata[10]=UID[0];
	eepromSenddata[11]=UID[1];
	eepromSenddata[12]=UID[2];
	eepromSenddata[13]=UID[3];
	eepromSenddata[14]=UID[4];
	eepromSenddata[15]=UID[5];
	eepromSenddata[16]=channel;
	eepromSenddata[17]=rate;
	eepromSenddata[18]=0x00;
	eepromSenddata[19]=0x00;
	eepromSenddata[20]=0x00;
	eepromSenddata[21]=0x00;
	eepromSenddata[22]=0x00;
	eepromSenddata[23]=0x00;		
	eepromSenddata[24]=0xFE;
	eepromSenddata[25]=0xFE;

	for(i=0;i<3;i++){
		SendMessage((unsigned char *)eepromSenddata,26);
		delay_ms(2000);
		RF_leng = GetMessage((unsigned char *)eepromRecvdata);
		if((RF_leng==26)&&(eepromRecvdata[3]==0xD8)&&(!strncmp(&eepromSenddata[4],&eepromRecvdata[4],22)))
		{
			RFM300H_SW = 0;
				return 1;
		}
		else
			continue;
	}
	return 0;
}

int RFM300_Heart_Beat(char *ECUID,char *UID,char *data)
{
	int i;
	extern unsigned char RFM300H_SW;
	unsigned char RF_leng = 0;
	char eepromSenddata[32] = {'\0'};
	char eepromRecvdata[32] = {'\0'};
	
	eepromSenddata[0]=0xFB;
	eepromSenddata[1]=0xFB;
	eepromSenddata[2]=0x0D;	
	eepromSenddata[3]=0x00;
	eepromSenddata[4]=ECUID[0];
	eepromSenddata[5]=ECUID[1];
	eepromSenddata[6]=ECUID[2];
	eepromSenddata[7]=ECUID[3];
	eepromSenddata[8]=ECUID[4];
	eepromSenddata[9]=ECUID[5];		
	eepromSenddata[10]=UID[0];
	eepromSenddata[11]=UID[1];
	eepromSenddata[12]=UID[2];
	eepromSenddata[13]=UID[3];
	eepromSenddata[14]=UID[4];
	eepromSenddata[15]=UID[5];
	eepromSenddata[16]=0xFE;
	eepromSenddata[17]=0xFE;

	for(i=0;i<3;i++){
		SendMessage((unsigned char *)eepromSenddata,18);
		delay_ms(2000);
		RF_leng = GetMessage((unsigned char *)eepromRecvdata);
		if((RF_leng==26)&&(eepromRecvdata[3]==0xD0)&&(!strncmp(&eepromSenddata[4],&eepromRecvdata[4],12)))
		{
			RFM300H_SW = 0;
			memcpy(data,&eepromSenddata[16],6);
			return 1;
		}
		else
			continue;
	}
	return 0;
}

int RFM300_IO_Init(char *ECUID,char *UID,char *data,int value)
{
	int i;
	extern unsigned char RFM300H_SW;
	unsigned char RF_leng = 0;
	char eepromSenddata[32] = {'\0'};
	char eepromRecvdata[32] = {'\0'};
	
	eepromSenddata[0]=0xFB;
	eepromSenddata[1]=0xFB;
	eepromSenddata[2]=0x15;	
	eepromSenddata[3]=value;
	eepromSenddata[4]=ECUID[0];
	eepromSenddata[5]=ECUID[1];
	eepromSenddata[6]=ECUID[2];
	eepromSenddata[7]=ECUID[3];
	eepromSenddata[8]=ECUID[4];
	eepromSenddata[9]=ECUID[5];		
	eepromSenddata[10]=UID[0];
	eepromSenddata[11]=UID[1];
	eepromSenddata[12]=UID[2];
	eepromSenddata[13]=UID[3];
	eepromSenddata[14]=UID[4];
	eepromSenddata[15]=UID[5];
	eepromSenddata[16]=0xFE;
	eepromSenddata[17]=0xFE;

	for(i=0;i<3;i++){
		SendMessage((unsigned char *)eepromSenddata,18);
		delay_ms(2000);
		RF_leng = GetMessage((unsigned char *)eepromRecvdata);
		if((RF_leng==26)&&(eepromRecvdata[3]==0xD1)&&(!strncmp(&eepromSenddata[4],&eepromRecvdata[4],12)))
		{
			RFM300H_SW = 0;
			memcpy(data,&eepromSenddata[16],6);
			return 1;			
		}
		else
			continue;
	}
	return 0;
}

int RFM300_Set_Uid(char *ECUID,char *UID,int channel,int rate,char *NewUid)
{
	int i;
	extern unsigned char RFM300H_SW;
	unsigned char RF_leng = 0;
	char eepromSenddata[32] = {'\0'};
	char eepromRecvdata[32] = {'\0'};

	eepromSenddata[0]=0xFB;
	eepromSenddata[1]=0xFB;
	eepromSenddata[2]=0x15;	
	eepromSenddata[3]=0x09;
	eepromSenddata[4]=ECUID[0];
	eepromSenddata[5]=ECUID[1];
	eepromSenddata[6]=ECUID[2];
	eepromSenddata[7]=ECUID[3];
	eepromSenddata[8]=ECUID[4];
	eepromSenddata[9]=ECUID[5];		
	eepromSenddata[10]=UID[0];
	eepromSenddata[11]=UID[1];
	eepromSenddata[12]=UID[2];
	eepromSenddata[13]=UID[3];
	eepromSenddata[14]=UID[4];
	eepromSenddata[15]=UID[5];
	eepromSenddata[16]=channel;
	eepromSenddata[17]=rate;
	eepromSenddata[18]=NewUid[0];
	eepromSenddata[19]=NewUid[1];
	eepromSenddata[20]=NewUid[2];
	eepromSenddata[21]=NewUid[3];
	eepromSenddata[22]=NewUid[4];
	eepromSenddata[23]=NewUid[5];		
	eepromSenddata[24]=0xFE;
	eepromSenddata[25]=0xFE;

	for(i=0;i<3;i++){
		SendMessage((unsigned char *)eepromSenddata,26);
		delay_ms(2000);
		RF_leng = GetMessage((unsigned char *)eepromRecvdata);
		if((RF_leng==26)&&(eepromRecvdata[3]==0xD9)&&(!strncmp(&eepromSenddata[4],&eepromRecvdata[4],6))
			&&(!strncmp(&eepromSenddata[18],&eepromRecvdata[10],6)))
		{
			RFM300H_SW = 0;		
				return 1;
		}
		else
				continue;
	}
	return 0;
}
