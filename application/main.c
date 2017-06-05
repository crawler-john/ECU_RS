#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "Flash_24L512.h"
#include "SEGGER_RTT.h"
#include "led.h"
#include "CMT2300.h"
#include "file.h"
#include "timer.h"
#include "appcomm.h"
#include "string.h"
#include "variation.h"

char ECUID12[13] = {'\0'};
char ECUID6[7] = {'\0'};
char Signal_Level[4] = {'\0'};
char Signal_Channel[3] = {'\0'};
char New_Signal_Channel[3] = {'\0'};
int vaildNum = 0;				//当前有效台数

inverter_info inverterInfo[MAXINVERTERCOUNT] = {'\0'};

int init_ecu(void)
{
	//	ECUID6[0] = 0x40;
//	ECUID6[1] = 0x60;
//	ECUID6[2] = 0x00;
//	ECUID6[3] = 0x00;
//	ECUID6[4] = 0x00;
//	ECUID6[5] = 0x09;	
//	Write_ECUID(ECUID6);						//读取ECU ID	
	Read_ECUID(ECUID6);						//读取ECU ID
	transformECUID(ECUID6,ECUID12);		//转换ECU ID
	
//	Signal_Level[0] ='0';
//	Signal_Level[1] ='9';
//	Signal_Level[2] ='9';
//	Write_SIGNAL_LV(Signal_Level);
	Read_SIGNAL_LV(Signal_Level);

//	Signal_Channel[0] ='0';
//	Signal_Channel[1] ='9';
//	Write_CHANNEL(Signal_Channel);
	Read_CHANNEL(Signal_Channel);
	
	SEGGER_RTT_printf(0, "ECU ID :%s     Signal_Level:%s     Signal_Channel:   %s    \n",ECUID12,Signal_Level,Signal_Channel);
	return 0;
}

int init_inverter(inverter_info *inverter)
{
	int i;
	
	unsigned char UID_NUM[2] = {'\0'};

	inverter_info *curinverter = inverter;
	UID_NUM[0] = 0;
	UID_NUM[1] = 0;
	//将所有的逆变器制空
	for(i=0; i<MAXINVERTERCOUNT; i++, curinverter++)
	{
		memset(curinverter->uid, 0xff, sizeof(curinverter->uid));			//清空逆变器ID
		curinverter->heart_rate = 0;
		curinverter->off_times = 0;
		curinverter->mos_status = 0;
	}
	
	//从EEPROM中读取逆变器信息

	Read_UID_NUM((char *)&UID_NUM);
	vaildNum = UID_NUM[0] *256 + UID_NUM[1];
	if(vaildNum > MAXINVERTERCOUNT)
	{
		vaildNum = 0;
	}
	SEGGER_RTT_printf(0, "UID_NUM %d \n",vaildNum);
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

	
	//向EEPROM写入数据
	Write_UID_NUM((char *)&UID_NUM);

	curinverter = inverter;
	for(i=0; (i<MAXINVERTERCOUNT && i<vaildNum); i++, curinverter++)
	{
		Write_UID(&uidstring[0+(i*6)],(i+1));
	}
	return 0;
}

int ResolveWifiPasswd(char *oldPasswd,int *oldLen,char *newPasswd,int *newLen,char *passwdstring)
{
	*oldLen = (passwdstring[0]-'0')*10+(passwdstring[1]-'0');
	memcpy(oldPasswd,&passwdstring[2],*oldLen);
	*newLen = (passwdstring[2+(*oldLen)]-'0')*10+(passwdstring[3+(*oldLen)]-'0');
	memcpy(newPasswd,&passwdstring[4+*oldLen],*newLen);
	
	return 0;
}

int main(void)
{	
	int Data_Len = 0,Command_Id = 0,ResolveFlag = 0,messageLen = 0;
	
	delay_init();	    	 					//延时函数初始化	  
	NVIC_Configuration(); 				//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	I2C_Init();										//FLASH 芯片初始化
	LED_init();										//LED灯初始化
	KEY_Init();										//恢复出厂设置按键初始化
	EXTIX_Init();									//恢复出厂设置IO中断初始化
	CMT2300_init();
	uart_init(57600);							//串口初始化
	TIM2_Int_Init(14999,7199);    //心跳包超时事件定时器初始化
	SEGGER_RTT_printf(0, "init OK \n");
	init_ecu();										//初始化ECU
	init_inverter(inverterInfo);	//初始化逆变器
	
	while(1)
	{
		//判断是否有433模块心跳超时事件
		if(COMM_Timeout_Event == 1)
		{
			SEGGER_RTT_printf(0, "COMM_Timeout_Event \n");
			COMM_Timeout_Event = 0;
		}
		WIFI_GetEvent(&messageLen);
		//判断是否有WIFI接收事件
		if(WIFI_Recv_Event == 1)
		{
			ResolveFlag =  Resolve_RecvData((char *)WIFI_RecvData,&Data_Len,&Command_Id);
			if(ResolveFlag == 0)
			{
				switch(Command_Id)
				{
					case COMMAND_BASEINFO:				//获取基本信息			APS11001401END
						SEGGER_RTT_printf(0, "WIFI_Recv_Event%d %s\n",COMMAND_BASEINFO,WIFI_RecvData);
						//获取基本信息
						APP_Response_BaseInfo(ECUID12,VERSION_ECU_RS,Signal_Level,Signal_Channel,5,SOFEWARE_VERSION);
						break;
					
					case COMMAND_SYSTEMINFO:			//获取系统信息			APS11002602406000000009END
						SEGGER_RTT_printf(0, "WIFI_Recv_Event%d %s\n",COMMAND_SYSTEMINFO,WIFI_RecvData);
						//先对比ECUID是否匹配
						if(!memcmp(&WIFI_RecvData[11],ECUID12,12))
						{	//匹配成功进行相应的操作
							SEGGER_RTT_printf(0, "COMMAND_SYSTEMINFO  Mapping\n");
							APP_Response_SystemInfo(0x00,inverterInfo);
					
						}	else
						{	//不匹配
							SEGGER_RTT_printf(0, "COMMAND_SYSTEMINFO   Not Mapping");
							APP_Response_SystemInfo(0x01,inverterInfo);
						}
						break;
					
					case COMMAND_SETNETWORK:			//设置组网					APS11003503406000000009END111111END
						SEGGER_RTT_printf(0, "WIFI_Recv_Event%d %s\n",COMMAND_SETNETWORK,WIFI_RecvData);
						//先对比ECUID是否匹配
						if(!memcmp(&WIFI_RecvData[11],ECUID12,12))
						{	//匹配成功进行相应的操作
							int AddNum = 0;;
							SEGGER_RTT_printf(0, "COMMAND_SETNETWORK   Mapping");
							
							AddNum = (messageLen - 29)/6;
							//将数据写入EEPROM
							add_inverter(inverterInfo,AddNum,(char *)&WIFI_RecvData[26]);
							APP_Response_SetNetwork(0x00);
							
							//进行一些绑定操作
							
						}	else
						{	//不匹配
							APP_Response_SetNetwork(0x01);
							SEGGER_RTT_printf(0, "COMMAND_SETNETWORK   Not Mapping");
						}
						break;
						
					case COMMAND_SETCHANNEL:			//设置信道					APS11003104406000000009END22END
						SEGGER_RTT_printf(0, "WIFI_Recv_Event%d %s\n",COMMAND_SETCHANNEL,WIFI_RecvData);
						if(!memcmp(&WIFI_RecvData[11],ECUID12,12))
						{	//匹配成功进行相应的操作
							memcpy(New_Signal_Channel,&WIFI_RecvData[26],2);
							
							APP_Response_SetChannel(0x00,New_Signal_Channel,Signal_Level);
						}	else
						{	//不匹配
							APP_Response_SetChannel(0x01,NULL,NULL);
						}
						break;
						
					case COMMAND_SETWIFIPASSWORD:	//设置WIFI密码	OK		APS11004905406000000009END08123456780887654321END
						SEGGER_RTT_printf(0, "WIFI_Recv_Event%d %s\n",COMMAND_SETWIFIPASSWORD,WIFI_RecvData);
						//先对比ECUID是否匹配
						if(!memcmp(&WIFI_RecvData[11],ECUID12,12))
						{	//匹配成功进行相应的操作
							char OldPassword[100] = {'\0'};
							char NewPassword[100] = {'\0'};
							char EEPROMPasswd[100] = {'\0'};
							int oldLen,newLen;
							
							ResolveWifiPasswd(OldPassword,&oldLen,NewPassword,&newLen,(char *)&WIFI_RecvData[26]);
							
							SEGGER_RTT_printf(0, "COMMAND_SETWIFIPASSWORD %d %s %d %s\n",oldLen,OldPassword,newLen,NewPassword);
							
							
							//读取旧密码，如果旧密码相同，设置新密码
							Read_WIFI_PW(EEPROMPasswd,100);
							
							//if(!memcmp(EEPROMPasswd,OldPassword,oldLen))
							//{
								APP_Response_SetWifiPassword(0x00);
								WIFI_ChangePasswd(NewPassword);
								Write_WIFI_PW(NewPassword,newLen);
							//}else
							//{
								//APP_Response_SetWifiPassword(0x02);
							//}
							
						}	else
						{	//不匹配
							APP_Response_SetWifiPassword(0x01);
						}					
						break;
				}
			}
			
			WIFI_Recv_Event = 0;
		}
		delay_us(2);
	}
	
	/*
	while(1)
	{
		Write_24L512_nByte(0x000000,10,eepromSenddata);
		SEGGER_RTT_printf(0, "eepromSenddata:   %s\n",eepromSenddata);

		Read_24L512_nByte(0x000001,10,eepromRecvdata);

		eepromRecvdata[10] = '\0';
		SEGGER_RTT_printf(0, "eepromRecvdata:   %s\n",eepromRecvdata);
		delay_ms(1000);
		
	}	
	*/
	
#if 0	
	while(1)
	{		
		int RF_leng;
		extern int RFM300H_SW;
		unsigned char eepromRecvdata[32] = {'\0'};	
		//SendMessage(eepromSenddata,31);
		//SEGGER_RTT_printf(0, "SendMessage:%s\n",eepromSenddata);
		//delay_ms(2000);
		RF_leng = GetMessage(eepromRecvdata);
		if(RF_leng)
		{
			SEGGER_RTT_printf(0, "SendMessage:%s\n",eepromRecvdata);

			RFM300H_SW = 0;
			
		}
	}
#endif 
	/*
	while(1)
	{	
		LED_on();	
		delay_ms(1000);
		LED_off();	
		delay_ms(1000);			
	}
	*/
	
	/*	
	while(1)
	{	
		ret = WIFI_SendData((char *)eepromSenddata, 10);
		SEGGER_RTT_printf(0, "ret = %d\n",ret);
		delay_ms(1000);		
	}
	*/	
	
 }

