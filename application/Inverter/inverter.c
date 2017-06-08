#include "inverter.h"
#include "SEGGER_RTT.h"
#include "file.h"
#include "RFM300H.h"
#include "timer.h"
#include "string.h"
#include "led.h"

int init_ecu(void)
{

	Read_ECUID(ECUID6);						//��ȡECU ID
	transformECUID(ECUID6,ECUID12);		//ת��ECU ID
	
	Read_CHANNEL(Signal_Channel);
	
	SEGGER_RTT_printf(0, "ECU ID :%s        Signal_Channel:   %s    \n",ECUID12,Signal_Channel);
	return 0;
}

int init_inverter(inverter_info *inverter)
{
	int i;
	
	unsigned char UID_NUM[2] = {'\0'};

	inverter_info *curinverter = inverter;
	UID_NUM[0] = 0;
	UID_NUM[1] = 0;
	//???����D��???��??��????
	for(i=0; i<MAXINVERTERCOUNT; i++, curinverter++)
	{
		memset(curinverter->uid, 0xff, sizeof(curinverter->uid));			//??????��??��ID
		curinverter->heart_rate = 0;
		curinverter->off_times = 0;
		curinverter->mos_status = 0;
	}
	
	//�䨮EEPROM?D?����???��??��D??��

	Read_UID_NUM((char *)&UID_NUM);
	vaildNum = UID_NUM[0] *256 + UID_NUM[1];
	if(vaildNum > MAXINVERTERCOUNT)
	{
		vaildNum = 0;
	}
	//��?1?�̡�?�㨮??��?�¨�y��??a0��????�䨬����?��?3�ꨢ��
	if(vaildNum == 0)
		LED_on();
	SEGGER_RTT_printf(0, "vaildNum :%d     \n",vaildNum);
	curinverter = inverter;
	for(i=0; (i<MAXINVERTERCOUNT && i<vaildNum); i++, curinverter++)
	{
		Read_UID((char *)curinverter->uid,(i+1));
	}
	return 0;
}

int add_inverter(inverter_info *inverter,int num,char *uidstring)
{
	int i;
	
	unsigned char UID_NUM[2] = {'\0'};

	inverter_info *curinverter = inverter;
	UID_NUM[0] = num/256;
	UID_NUM[1] = num%256;

	
	//?��EEPROMD�䨨?��y?Y
	Write_UID_NUM((char *)&UID_NUM);

	curinverter = inverter;
	for(i=0; (i<MAXINVERTERCOUNT && i<vaildNum); i++, curinverter++)
	{
		Write_UID(&uidstring[0+(i*6)],(i+1));
	}
	return 0;
}


//�㨮?��??��??��
void bind_inverter(inverter_info *inverter)
{
	int i = 0;
	switchLed(0x01);
	for(i =0;i<vaildNum;i++)
	{
		RFM300_Bind_Uid(ECUID6,(char *)inverter[i].uid,0,0);
	}
	switchLed(0x00);
}

//��??��D?�̨�
void changeChannel_inverter(inverter_info *inverter,char channel)
{
	int i = 0;
	switchLed(0x01);
	for(i =0;i<vaildNum;i++)
	{
		RFM300_Bind_Uid(ECUID6,(char *)inverter[i].uid,channel,0);
	}
	switchLed(0x00);
}

void changeIOinit_inverter(inverter_info *inverter,char IO_Status)
{
	int i = 0;
	switchLed(0x01);
	for(i =0;i<vaildNum;i++)
	{
		RFM300_IO_Init(ECUID6,(char *)inverter[i].uid,IO_Status,(char *)&inverterInfo[curSequence].mos_status,&IO_Init_Status,&inverterInfo[curSequence].heart_rate,&inverterInfo[curSequence].off_times,&ver);
	}
	switchLed(0x00);
}