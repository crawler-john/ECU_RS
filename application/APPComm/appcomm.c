#include "appcomm.h"
#include "string.h"
#include "SEGGER_RTT.h"

static char SendData[MAXINVERTERCOUNT*INVERTERLENGTH + 16] = {'\0'};

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
	//char SendData[100] = {'\0'};
	memset(SendData,'\0',MAXINVERTERCOUNT*INVERTERLENGTH + 16);	
	sprintf(SendData,"APS11%04d01%s%03d%03d%s%03d%sEND\n",(37+Length),ECU_NO,101,SIGNAL_LEVEL,SIGNAL_CHANNEL,Length,Version);
	SEGGER_RTT_printf(0, "APP_Response_BaseInfo %s\n",SendData);
	WIFI_SendData(SendData, (38+Length));
}

//ECU-RS系统信息回应   mapflag   0表示匹配成功  1 表示匹配不成功
void APP_Response_SystemInfo(unsigned char mapflag,inverter_info *inverter,int vaildNum)
{
	
	inverter_info *curinverter = inverter;
	int i = 0;
	int length = 0;
	memset(SendData,'\0',MAXINVERTERCOUNT*INVERTERLENGTH + 16);
	
	//SEGGER_RTT_printf(0, "SystemInfo %d %d  %d\n",mapflag,vaildNum,(MAXINVERTERCOUNT*INVERTERLENGTH + 16));

	if(mapflag == 1)
	{
		sprintf(SendData,"APS1100130201\n");
		WIFI_SendData(SendData, 14);
		return;
	}else{
		
		sprintf(SendData,"APS1100130200");   //13字节
		for(i=0; (i<MAXINVERTERCOUNT)&&(i < vaildNum); i++)			
		{
			memcpy(&SendData[13+i*INVERTERLENGTH],curinverter->uid,6);
			SEGGER_RTT_printf(0, "SystemInfo %02x%02x%02x%02x%02x%02x   ",curinverter->uid[0],curinverter->uid[1],curinverter->uid[2],curinverter->uid[3],curinverter->uid[4],curinverter->uid[5]);
			
			SendData[19+i*INVERTERLENGTH] = curinverter->heart_rate /256;
			SendData[20+i*INVERTERLENGTH] = curinverter->heart_rate %256;

			SEGGER_RTT_printf(0, "heart_rate %d    ",curinverter->heart_rate);

			SendData[21+i*INVERTERLENGTH] = curinverter->off_times/256;
			SendData[22+i*INVERTERLENGTH] = curinverter->off_times%256;
			SEGGER_RTT_printf(0, "off_times %d    ",curinverter->off_times);

			memcpy(&SendData[23+i*INVERTERLENGTH],&curinverter->mos_status,1);
			SEGGER_RTT_printf(0, "mos_status %d\n",curinverter->mos_status);

			
			curinverter++;
		}

		if(vaildNum > 0)
		{		
			length = (16+(vaildNum*INVERTERLENGTH));
			
			//改变报文字节长度
			SendData[5] = (length/1000) + '0';
			SendData[6] =	((length/100)%10) + '0';
			SendData[7] = ((length/10)%10) + '0';
			SendData[8] = ((length)%10) + '0';
			SendData[length-3] = 'E';
			SendData[length-2] = 'N';
			SendData[length-1] = 'D';
			SendData[length] = '\n';
		}else
		{
			length = 13;
			SendData[length] = '\n';
			
		}
		
#if 0
	for(i=0;i<length;i++)
	{
		SEGGER_RTT_printf(0, "%02x ",SendData[i]);
	}
	SEGGER_RTT_printf(0, "\n");
#endif	
		
		WIFI_SendData(SendData, (length+1));
	}


}

//ECU-RS设置组网回应
void APP_Response_SetNetwork(unsigned char result)
{
	//char SendData[20] = {'\0'};
	memset(SendData,'\0',MAXINVERTERCOUNT*INVERTERLENGTH + 16);	
	sprintf(SendData,"APS11001303%02d\n",result);
	WIFI_SendData(SendData, 14);
}

//ECU-RS设置信道回应
void APP_Response_SetChannel(unsigned char mapflag,char *SIGNAL_CHANNEL,char SIGNAL_LEVEL)
{
	//char SendData[22] = {'\0'};
	memset(SendData,'\0',MAXINVERTERCOUNT*INVERTERLENGTH + 16);
	if(mapflag == 1)
	{
		sprintf(SendData,"APS1100130401\n");
		WIFI_SendData(SendData, 14);
	}else{
		sprintf(SendData,"APS1100210400%s%03dEND\n",SIGNAL_CHANNEL,SIGNAL_LEVEL);
		WIFI_SendData(SendData, 22);
	}
}

//ECU-RS设置WIFI密码
void APP_Response_SetWifiPassword(unsigned char result)
{
	//char SendData[20] = {'\0'};
	memset(SendData,'\0',MAXINVERTERCOUNT*INVERTERLENGTH + 16);
	sprintf(SendData,"APS11001305%02d\n",result);
	WIFI_SendData(SendData, 14);
}


void APP_Response_IOInitStatus(unsigned char result)
{
	//char SendData[20] = {'\0'};
	memset(SendData,'\0',MAXINVERTERCOUNT*INVERTERLENGTH + 16);
	sprintf(SendData,"APS11001306%02d\n",result);
	WIFI_SendData(SendData, 14);
}
