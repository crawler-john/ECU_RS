#include "appcomm.h"
#include "string.h"
#include "SEGGER_RTT.h"

//�����յ�������
int Resolve_RecvData(char *RecvData,int* Data_Len,int* Command_Id)
{
	//APS
	if(strncmp(RecvData, "APS", 3))
		return -1;
	//�汾�� 
	//����
	*Data_Len = packetlen((unsigned char *)&RecvData[5]);
	//ID
	*Command_Id = (RecvData[9]-'0')*10 + (RecvData[10]-'0');
	return 0;
}

//ECU-RS��ȡ������Ϣ��Ӧ
void APP_Response_BaseInfo(char *ECU_NO,char *TYPE,char *SIGNAL_LEVEL,char *SIGNAL_CHANNEL,int Length,char * Version)
{
	char SendData[100] = {'\0'};
	
	sprintf(SendData,"APS11%04d01%s%03d%s%s%03d%sEND",(37+Length),ECU_NO,101,SIGNAL_LEVEL,SIGNAL_CHANNEL,Length,Version);
	SEGGER_RTT_printf(0, "APP_Response_BaseInfo %s\n",SendData);
	WIFI_SendData(SendData, (37+Length));
}

//ECU-RSϵͳ��Ϣ��Ӧ
void APP_Response_SystemInfo(unsigned char mapflag,char * UID,char *Heart_Rate,char *Off_Times,char Mos_Status)
{
	
}

//ECU-RS����������Ӧ
void APP_Response_SetNetwork(unsigned char result)
{
	char SendData[20] = {'\0'};
	sprintf(SendData,"APS11001303%02d",result);
	WIFI_SendData(SendData, 13);
}

//ECU-RS�����ŵ���Ӧ
void APP_Response_SetChannel(unsigned char mapflag,char *SIGNAL_CHANNEL,char *SIGNAL_LEVEL)
{
	char SendData[22] = {'\0'};
	if(mapflag == 1)
	{
		sprintf(SendData,"APS1100130401");
		WIFI_SendData(SendData, 13);
	}else{
		sprintf(SendData,"APS1100210400%s%sEND",SIGNAL_CHANNEL,SIGNAL_LEVEL);
		WIFI_SendData(SendData, 21);
	}
}

//ECU-RS����WIFI����
void APP_Response_SetWifiPassword(unsigned char result)
{
	char SendData[20] = {'\0'};
	sprintf(SendData,"APS11001305%02d",result);
	WIFI_SendData(SendData, 13);
}
