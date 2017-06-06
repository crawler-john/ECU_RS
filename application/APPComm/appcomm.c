#include "appcomm.h"
#include "string.h"
#include "SEGGER_RTT.h"

//解析收到的数据
int Resolve_RecvData(char *RecvData,int* Data_Len,int* Command_Id)
{
	//APS
	if(strncmp(RecvData, "APS", 3))
		return -1;
	//版本号 
	//长度
	*Data_Len = packetlen((unsigned char *)&RecvData[5]);
	//ID
	*Command_Id = (RecvData[9]-'0')*10 + (RecvData[10]-'0');
	return 0;
}

//ECU-RS获取基本信息回应
void APP_Response_BaseInfo(char *ECU_NO,char *TYPE,char SIGNAL_LEVEL,char *SIGNAL_CHANNEL,int Length,char * Version)
{
	char SendData[100] = {'\0'};
	
	sprintf(SendData,"APS11%04d01%s%03d%03d%s%03d%sEND",(37+Length),ECU_NO,101,SIGNAL_LEVEL,SIGNAL_CHANNEL,Length,Version);
	SEGGER_RTT_printf(0, "APP_Response_BaseInfo %s\n",SendData);
	WIFI_SendData(SendData, (37+Length));
}

//ECU-RS系统信息回应   mapflag   0表示匹配成功  1 表示匹配不成功
void APP_Response_SystemInfo(unsigned char mapflag,inverter_info *inverter,int vaildNum)
{
	char SendData[MAXINVERTERCOUNT*INVERTERLENGTH + 16] = {'\0'};
	inverter_info *curinverter = inverter;
	int i = 0,validNum = 0;
	int length = 0;
	if(mapflag == 1)
	{
		sprintf(SendData,"APS1100130201");
		WIFI_SendData(SendData, 13);
	}else{
		sprintf(SendData,"APS1100130200");   //13字节
		for(i=0; (i<MAXINVERTERCOUNT)&&(i < vaildNum); i++)			
		{
			memcpy(&SendData[13+validNum*INVERTERLENGTH],curinverter->uid,6);
			memcpy(&SendData[19+validNum*INVERTERLENGTH],&curinverter->heart_rate,2);
			memcpy(&SendData[21+validNum*INVERTERLENGTH],&curinverter->off_times,2);
			memcpy(&SendData[23+validNum*INVERTERLENGTH],&curinverter->mos_status,1);
			
			validNum ++;
		}
		
		if(validNum > 0)
		{		
			length = (16+(validNum*INVERTERLENGTH));
			
			//改变报文字节长度
			SendData[5] = (length/1000) + '0';
			SendData[6] =	((length/100)%10) + '0';
			SendData[7] = ((length/10)%10) + '0';
			SendData[8] = ((length)%10) + '0';
			
		}else
			length = 13;

		
		WIFI_SendData(SendData, length);
	}
}

//ECU-RS设置组网回应
void APP_Response_SetNetwork(unsigned char result)
{
	char SendData[20] = {'\0'};
	sprintf(SendData,"APS11001303%02d",result);
	WIFI_SendData(SendData, 13);
}

//ECU-RS设置信道回应
void APP_Response_SetChannel(unsigned char mapflag,char *SIGNAL_CHANNEL,char SIGNAL_LEVEL)
{
	char SendData[22] = {'\0'};
	if(mapflag == 1)
	{
		sprintf(SendData,"APS1100130401");
		WIFI_SendData(SendData, 13);
	}else{
		sprintf(SendData,"APS1100210400%s%03dEND",SIGNAL_CHANNEL,SIGNAL_LEVEL);
		WIFI_SendData(SendData, 21);
	}
}

//ECU-RS设置WIFI密码
void APP_Response_SetWifiPassword(unsigned char result)
{
	char SendData[20] = {'\0'};
	sprintf(SendData,"APS11001305%02d",result);
	WIFI_SendData(SendData, 13);
}
