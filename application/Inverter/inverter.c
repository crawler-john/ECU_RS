#include "inverter.h"
#include "SEGGER_RTT.h"
#include "file.h"
#include "RFM300H.h"
#include "timer.h"
#include "string.h"
#include "led.h"

int init_ecu(void)
{

	Read_ECUID(ECUID6);						//¶ÁÈ¡ECU ID
	transformECUID(ECUID6,ECUID12);		//×ª»»ECU ID
	
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
	//???¨´¨®D¦Ì???¡À??¡Â????
	for(i=0; i<MAXINVERTERCOUNT; i++, curinverter++)
	{
		memset(curinverter->uid, 0xff, sizeof(curinverter->uid));			//??????¡À??¡ÂID
		curinverter->heart_rate = 0;
		curinverter->off_times = 0;
		curinverter->mos_status = 0;
	}
	
	//¡ä¨®EEPROM?D?¨¢¨¨???¡À??¡ÂD??¡é

	Read_UID_NUM((char *)&UID_NUM);
	vaildNum = UID_NUM[0] *256 + UID_NUM[1];
	if(vaildNum > MAXINVERTERCOUNT)
	{
		vaildNum = 0;
	}
	//¨¨?1?¦Ì¡À?¡ã¨®??¡¥?¡Â¨ºy¨¢??a0¡ê????¡ä¨¬¨¢¨º?¦Ì?3¡ê¨¢¨¢
	if(vaildNum == 0)
		LED_on();
	SEGGER_RTT_printf(0, "vaildNum :%d     \n",vaildNum);
	curinverter = inverter;
	for(i=0; (i<MAXINVERTERCOUNT && i<vaildNum); i++, curinverter++)
	{
		Read_UID((char *)curinverter->uid,(i+1));
		SEGGER_RTT_printf(0, "uid%d: %02x%02x%02x%02x%02x%02x\n",(i+1),curinverter->uid[0],curinverter->uid[1],curinverter->uid[2],curinverter->uid[3],curinverter->uid[4],curinverter->uid[5]);
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
	SEGGER_RTT_printf(0, "111111111111111 %x %x\n",UID_NUM[0],UID_NUM[1]);	
	
	//?¨°EEPROMD¡ä¨¨?¨ºy?Y
	Write_UID_NUM((char *)&UID_NUM);

	curinverter = inverter;
	for(i=0; (i<MAXINVERTERCOUNT && i<num); i++, curinverter++)
	{
		Write_UID(&uidstring[0+(i*6)],(i+1));
		SEGGER_RTT_printf(0, "add_inverter uid%d: %02x%02x%02x%02x%02x%02x  \n",(i+1),uidstring[0+(i*6)],uidstring[1+(i*6)],uidstring[2+(i*6)],uidstring[3+(i*6)],uidstring[4+(i*6)],uidstring[5+(i*6)]);		
	}
	return 0;
}


//¡ã¨®?¡§??¡À??¡Â
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

//¡À??¨¹D?¦Ì¨¤
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
