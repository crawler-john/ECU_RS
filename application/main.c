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
#include "RFM300H.h"
#include "variation.h"
#include "USART1Comm.h"

char ECUID12[13] = {'\0'};
char ECUID6[7] = {'\0'};
char Signal_Level = 0;
char Signal_Channel[3] = {'\0'};
char New_Signal_Channel[3] = {'\0'};
char IO_Init_Status = 0;			//IO初始状态
char ver = 0;						//优化器版本号
int vaildNum = 0;				//当前有效台数
int curSequence = 0;		//心跳轮训机器号
inverter_info inverterInfo[MAXINVERTERCOUNT] = {'\0'};
int Data_Len = 0,Command_Id = 0,ResolveFlag = 0,messageLen = 0,messageUsart1Len = 0;
int UART1_Data_Len = 0,UART1_Command_Id = 0,UART1_ResolveFlag = 0;

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
	
//	Signal_Channel[0] ='0';
//	Signal_Channel[1] ='9';
//	Write_CHANNEL(Signal_Channel);
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
	//如果当前优化器数量为0，那么提示灯常亮
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


//绑定逆变器
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

//变更信道
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


//心跳事件处理
void process_HeartBeatEvent(void)
{
	//SEGGER_RTT_printf(0, "COMM_Timeout_Event \n");
	//发送心跳包
	if(	vaildNum >0	)
	{
		RFM300_Heart_Beat(ECUID6,(char *)inverterInfo[curSequence].uid,(char *)&inverterInfo[curSequence].mos_status,&IO_Init_Status,&inverterInfo[curSequence].heart_rate,&inverterInfo[curSequence].off_times,&ver);
		curSequence++;
		if(curSequence >= vaildNum)		//当轮训的序号大于最后一台时，更换到第0台
		{
			curSequence = 0;
		}
				
	}
}

//WIFI事件处理
void process_WIFIEvent(void)
{
	ResolveFlag =  Resolve_RecvData((char *)WIFI_RecvData,&Data_Len,&Command_Id);
	if(ResolveFlag == 0)
	{
		switch(Command_Id)
		{
			case COMMAND_BASEINFO:				//获取基本信息			APS11001401END
				SEGGER_RTT_printf(0, "WIFI_Recv_Event%d %s\n",COMMAND_BASEINFO,WIFI_RecvData);
				//获取基本信息
				//获取信号强度
				Signal_Level = ReadRssiValue(1);
				APP_Response_BaseInfo(ECUID12,VERSION_ECU_RS,Signal_Level,Signal_Channel,5,SOFEWARE_VERSION);
				break;
					
			case COMMAND_SYSTEMINFO:			//获取系统信息			APS11002602406000000009END
				SEGGER_RTT_printf(0, "WIFI_Recv_Event%d %s\n",COMMAND_SYSTEMINFO,WIFI_RecvData);
				//先对比ECUID是否匹配
				if(!memcmp(&WIFI_RecvData[11],ECUID12,12))
				{	//匹配成功进行相应的操作
					SEGGER_RTT_printf(0, "COMMAND_SYSTEMINFO  Mapping\n");
					APP_Response_SystemInfo(0x00,inverterInfo,vaildNum);
				}	else
				{	//不匹配
					SEGGER_RTT_printf(0, "COMMAND_SYSTEMINFO   Not Mapping");
					APP_Response_SystemInfo(0x01,inverterInfo,0);
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
					init_inverter(inverterInfo);
					//进行一些绑定操作
					bind_inverter(inverterInfo);
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
					char newChannel = 0;
					//获取新的信道
					memcpy(New_Signal_Channel,&WIFI_RecvData[26],2);
					//获取信号强度
					Signal_Level = ReadRssiValue(1);
					APP_Response_SetChannel(0x00,New_Signal_Channel,Signal_Level);
					//将其转换为1个字节
					newChannel = New_Signal_Channel[0]*10+New_Signal_Channel[1];
					changeChannel_inverter(inverterInfo,newChannel);
					//改变自己的信道
							
					//保存新信道到Flash
					memcpy(Signal_Channel,New_Signal_Channel,3);
					Write_CHANNEL(Signal_Channel);
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
				
			case COMMAND_IOINITSTATUS:	//设置IO初始化状态	OK		
				SEGGER_RTT_printf(0, "WIFI_Recv_Event%d %s\n",COMMAND_IOINITSTATUS,WIFI_RecvData);
				//先对比ECUID是否匹配
				if(!memcmp(&WIFI_RecvData[11],ECUID12,12))
				{//匹配成功进行相应的操作
					//获取IO初始状态
					IO_Init_Status = WIFI_RecvData[23];
					APP_Response_IOInitStatus(0x00);
					APP_Response_SetChannel(0x00,New_Signal_Channel,Signal_Level);
					changeIOinit_inverter(inverterInfo,IO_Init_Status);
					//保存新IO初始化状态到Flash
					Write_IO_INIT_STATU(&IO_Init_Status);	
					
				}else
				{
					APP_Response_IOInitStatus(0x01);
				}
		}
	}
}


//串口事件处理
void process_UART1Event(void)
{
	char USART1_ECUID12[13] = {'\0'};
	char USART1_ECUID6[7] = {'\0'};
	char USART1_UID_NUM[2] = {0x00,0x00};
	char ret =0;
	int AddNum = 0;
	UART1_ResolveFlag =  Resolve_Usart1RecvData((char *)USART1_RecvData,&UART1_Data_Len,&UART1_Command_Id);
	if(UART1_ResolveFlag == 0)
	{
		switch(UART1_Command_Id)
		{
			case COMMAND_SET_ID:				//设置ID
				SEGGER_RTT_printf(0, "WIFI_Recv_Event%d %s\n",COMMAND_SET_ID,USART1_RecvData);
				memcpy(USART1_ECUID12,&USART1_RecvData[11],12);
				USART1_ECUID6[0] = ((USART1_ECUID12[0]-'0')<<4) + (USART1_ECUID12[1]-'0') ;
				USART1_ECUID6[1] = ((USART1_ECUID12[2]-'0')<<4) + (USART1_ECUID12[3]-'0') ;
				USART1_ECUID6[2] = ((USART1_ECUID12[3]-'0')<<4) + (USART1_ECUID12[5]-'0') ;
				USART1_ECUID6[3] = ((USART1_ECUID12[4]-'0')<<4) + (USART1_ECUID12[7]-'0') ;
				USART1_ECUID6[4] = ((USART1_ECUID12[5]-'0')<<4) + (USART1_ECUID12[9]-'0') ;
				USART1_ECUID6[5] = ((USART1_ECUID12[10]-'0')<<4) + (USART1_ECUID12[11]-'0') ;
				ret = Write_ECUID(USART1_ECUID6);													  		//ECU ID
				USART1_Response_SET_ID(ret);
				init_ecu();	
				break;
					
			case COMMAND_READ_ID:			//读取ID
				SEGGER_RTT_printf(0, "WIFI_Recv_Event%d %s\n",COMMAND_READ_ID,USART1_RecvData);
				Read_ECUID(USART1_ECUID6);
				transformECUID(USART1_ECUID6,USART1_ECUID12);		//转换ECU ID
				USART1_ECUID12[12] = '\0';
				if(USART1_ECUID6[0] == 0xff)
				{
					USART1_Response_READ_ID(1,USART1_ECUID12);
				}else
				{
					USART1_Response_READ_ID(0,USART1_ECUID12);
				}
				
				break;
					
			case COMMAND_TEST:			//测试
				SEGGER_RTT_printf(0, "WIFI_Recv_Event%d %s\n",COMMAND_TEST,USART1_RecvData);
				
				
				break;
						
			case COMMAND_SET_NETWORK:	//组网
				SEGGER_RTT_printf(0, "WIFI_Recv_Event%d %s\n",COMMAND_SET_NETWORK,USART1_RecvData);
				{	//匹配成功进行相应的操作
					
					SEGGER_RTT_printf(0, "COMMAND_SETNETWORK   Mapping");
							
					AddNum = (messageUsart1Len - 17)/6;
					//将数据写入EEPROM
					add_inverter(inverterInfo,AddNum,(char *)&WIFI_RecvData[14]);
					APP_Response_SetNetwork(0x00);
					init_inverter(inverterInfo);
					//进行一些绑定操作
					bind_inverter(inverterInfo);
				}	
				break;
						
			case COMMAND_FACTORY:	//恢复到刚出厂的状态
				SEGGER_RTT_printf(0, "WIFI_Recv_Event%d %s\n",COMMAND_FACTORY,USART1_RecvData);
				ret = Write_UID_NUM(USART1_UID_NUM);
				USART1_Response_FACTORY(ret);
				init_inverter(inverterInfo);
			
				break;
		}
	}
}

//按键事件处理
void process_KEYEvent(void)
{
	SEGGER_RTT_printf(0, "KEY_FormatWIFI_Event Start\n");
	WIFI_ClearPasswd();
	SEGGER_RTT_printf(0, "KEY_FormatWIFI_Event End\n");
}

int main(void)
{	
//	int RF_leng;
//	extern int RFM300H_SW;
//	unsigned char eepromSenddata[32] = "HJJHJJHJJ HJJHJJHJJ HJJHJJHJJ ";
//	
//	unsigned char eepromRecvdata[32] = {'\0'};
//	eepromSenddata[30] = 'A';
	
	delay_init();	    	 					//延时函数初始化	
	NVIC_Configuration(); 				//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	I2C_Init();										//FLASH 芯片初始化
	LED_init();										//LED灯初始化
	EXTIX_Init();									//恢复出厂设置IO中断初始化
	KEY_Init();										//恢复出厂设置按键初始化

	CMT2300_init();
	uart1_init(57600);
	uart2_init(57600);							//串口初始化
	TIM2_Int_Init(4999,7199);    //心跳包超时事件定时器初始化
	SEGGER_RTT_printf(0, "init OK \n");
	init_ecu();										//初始化ECU
	init_inverter(inverterInfo);	//初始化逆变器
#if 1
	while(1)
	{	
		//检测USART1事件
		USART1_GetEvent(&messageUsart1Len);
		if(USART1_Recv_Event == 1)
		{
			SEGGER_RTT_printf(0, "USART1_Recv_Event %s\n",USART1_RecvData);
			process_UART1Event();
			USART1_Recv_Event = 0;
		}
		//检测按键事件
		if(KEY_FormatWIFI_Event == 1)
		{
			process_KEYEvent();
			KEY_FormatWIFI_Event = 0;
		}
		
		//判断是否有433模块心跳超时事件
		if(COMM_Timeout_Event == 1)
		{
			process_HeartBeatEvent();
			COMM_Timeout_Event = 0;
		}
		
		//检测WIFI事件
		WIFI_GetEvent(&messageLen);
		//判断是否有WIFI接收事件
		if(WIFI_Recv_Event == 1)
		{
			process_WIFIEvent();
			WIFI_Recv_Event = 0;
		}
		
	}
#endif


#if 0
	while(1)
	{
		unsigned char rss;
		unsigned char eepromSenddata[32] = "HJJHJJHJJ HJJHJJHJJ HJJHJJHJJ ";
	
		unsigned char eepromRecvdata[32] = {'\0'};
		Write_24L512_nByte(0x0001A0,32,eepromSenddata);
		SEGGER_RTT_printf(0, "eepromSenddata:   %s\n",eepromSenddata);

		Read_24L512_nByte(0x0001A0,32,eepromRecvdata);
		SEGGER_RTT_printf(0, "eepromRecvdata:   %s\n",eepromRecvdata);
		
		rss =  ReadRssiValue(1);
		SEGGER_RTT_printf(0, "ReadRssiValue:   %d\n",rss);
		delay_ms(1000);
		
	}	
#endif
	
#if 0
	while(1)
	{		
		eepromSenddata[30]++;
		if (eepromSenddata[30] > 'Z')
		{
			eepromSenddata[30] = 'A';
		}
		SendMessage(eepromSenddata,31);
		SEGGER_RTT_printf(0, "SendMessage:%s\n",eepromSenddata);
		
		RF_leng = GetMessage(eepromRecvdata);
		if(RF_leng)
		{
			SEGGER_RTT_printf(0, "RecvMessage 11111 :%s\n",eepromRecvdata);
			RFM300H_SW = 0;
			
		}
		delay_ms(1000);
	}
//#else
	 while(1)
 {  
  delay_ms(20);
  RF_leng = GetMessage(eepromRecvdata);
  if(RF_leng)
  {
		SEGGER_RTT_printf(0, "RECVMessage:%s\n",eepromRecvdata);
		RFM300H_SW = 0;
		delay_ms(1800);
		SendMessage(eepromRecvdata,31);
		SEGGER_RTT_printf(0, "SendMessage:%s\n",eepromRecvdata);
  
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

