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
char IO_Init_Status = 0;			//IO��ʼ״̬
char ver = 0;						//�Ż����汾��
int vaildNum = 0;				//��ǰ��Ч̨��
int curSequence = 0;		//������ѵ������
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
//	Write_ECUID(ECUID6);						//��ȡECU ID	
	Read_ECUID(ECUID6);						//��ȡECU ID
	transformECUID(ECUID6,ECUID12);		//ת��ECU ID
	
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
	//�����е�������ƿ�
	for(i=0; i<MAXINVERTERCOUNT; i++, curinverter++)
	{
		memset(curinverter->uid, 0xff, sizeof(curinverter->uid));			//��������ID
		curinverter->heart_rate = 0;
		curinverter->off_times = 0;
		curinverter->mos_status = 0;
	}
	
	//��EEPROM�ж�ȡ�������Ϣ

	Read_UID_NUM((char *)&UID_NUM);
	vaildNum = UID_NUM[0] *256 + UID_NUM[1];
	if(vaildNum > MAXINVERTERCOUNT)
	{
		vaildNum = 0;
	}
	//�����ǰ�Ż�������Ϊ0����ô��ʾ�Ƴ���
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

	
	//��EEPROMд������
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


//�������
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

//����ŵ�
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


//�����¼�����
void process_HeartBeatEvent(void)
{
	//SEGGER_RTT_printf(0, "COMM_Timeout_Event \n");
	//����������
	if(	vaildNum >0	)
	{
		RFM300_Heart_Beat(ECUID6,(char *)inverterInfo[curSequence].uid,(char *)&inverterInfo[curSequence].mos_status,&IO_Init_Status,&inverterInfo[curSequence].heart_rate,&inverterInfo[curSequence].off_times,&ver);
		curSequence++;
		if(curSequence >= vaildNum)		//����ѵ����Ŵ������һ̨ʱ����������0̨
		{
			curSequence = 0;
		}
				
	}
}

//WIFI�¼�����
void process_WIFIEvent(void)
{
	ResolveFlag =  Resolve_RecvData((char *)WIFI_RecvData,&Data_Len,&Command_Id);
	if(ResolveFlag == 0)
	{
		switch(Command_Id)
		{
			case COMMAND_BASEINFO:				//��ȡ������Ϣ			APS11001401END
				SEGGER_RTT_printf(0, "WIFI_Recv_Event%d %s\n",COMMAND_BASEINFO,WIFI_RecvData);
				//��ȡ������Ϣ
				//��ȡ�ź�ǿ��
				Signal_Level = ReadRssiValue(1);
				APP_Response_BaseInfo(ECUID12,VERSION_ECU_RS,Signal_Level,Signal_Channel,5,SOFEWARE_VERSION);
				break;
					
			case COMMAND_SYSTEMINFO:			//��ȡϵͳ��Ϣ			APS11002602406000000009END
				SEGGER_RTT_printf(0, "WIFI_Recv_Event%d %s\n",COMMAND_SYSTEMINFO,WIFI_RecvData);
				//�ȶԱ�ECUID�Ƿ�ƥ��
				if(!memcmp(&WIFI_RecvData[11],ECUID12,12))
				{	//ƥ��ɹ�������Ӧ�Ĳ���
					SEGGER_RTT_printf(0, "COMMAND_SYSTEMINFO  Mapping\n");
					APP_Response_SystemInfo(0x00,inverterInfo,vaildNum);
				}	else
				{	//��ƥ��
					SEGGER_RTT_printf(0, "COMMAND_SYSTEMINFO   Not Mapping");
					APP_Response_SystemInfo(0x01,inverterInfo,0);
				}
				break;
					
			case COMMAND_SETNETWORK:			//��������					APS11003503406000000009END111111END
				SEGGER_RTT_printf(0, "WIFI_Recv_Event%d %s\n",COMMAND_SETNETWORK,WIFI_RecvData);
				//�ȶԱ�ECUID�Ƿ�ƥ��
				if(!memcmp(&WIFI_RecvData[11],ECUID12,12))
				{	//ƥ��ɹ�������Ӧ�Ĳ���
					int AddNum = 0;;
					SEGGER_RTT_printf(0, "COMMAND_SETNETWORK   Mapping");
							
					AddNum = (messageLen - 29)/6;
					//������д��EEPROM
					add_inverter(inverterInfo,AddNum,(char *)&WIFI_RecvData[26]);
					APP_Response_SetNetwork(0x00);
					init_inverter(inverterInfo);
					//����һЩ�󶨲���
					bind_inverter(inverterInfo);
				}	else
				{	//��ƥ��
					APP_Response_SetNetwork(0x01);
					SEGGER_RTT_printf(0, "COMMAND_SETNETWORK   Not Mapping");
				}
				break;
						
			case COMMAND_SETCHANNEL:			//�����ŵ�					APS11003104406000000009END22END
				SEGGER_RTT_printf(0, "WIFI_Recv_Event%d %s\n",COMMAND_SETCHANNEL,WIFI_RecvData);
				if(!memcmp(&WIFI_RecvData[11],ECUID12,12))
				{	//ƥ��ɹ�������Ӧ�Ĳ���
					char newChannel = 0;
					//��ȡ�µ��ŵ�
					memcpy(New_Signal_Channel,&WIFI_RecvData[26],2);
					//��ȡ�ź�ǿ��
					Signal_Level = ReadRssiValue(1);
					APP_Response_SetChannel(0x00,New_Signal_Channel,Signal_Level);
					//����ת��Ϊ1���ֽ�
					newChannel = New_Signal_Channel[0]*10+New_Signal_Channel[1];
					changeChannel_inverter(inverterInfo,newChannel);
					//�ı��Լ����ŵ�
							
					//�������ŵ���Flash
					memcpy(Signal_Channel,New_Signal_Channel,3);
					Write_CHANNEL(Signal_Channel);
				}	else
				{	//��ƥ��
					APP_Response_SetChannel(0x01,NULL,NULL);
				}
				break;
						
			case COMMAND_SETWIFIPASSWORD:	//����WIFI����	OK		APS11004905406000000009END08123456780887654321END
				SEGGER_RTT_printf(0, "WIFI_Recv_Event%d %s\n",COMMAND_SETWIFIPASSWORD,WIFI_RecvData);
				//�ȶԱ�ECUID�Ƿ�ƥ��
				if(!memcmp(&WIFI_RecvData[11],ECUID12,12))
				{	//ƥ��ɹ�������Ӧ�Ĳ���
					char OldPassword[100] = {'\0'};
					char NewPassword[100] = {'\0'};
					char EEPROMPasswd[100] = {'\0'};
					int oldLen,newLen;
							
					ResolveWifiPasswd(OldPassword,&oldLen,NewPassword,&newLen,(char *)&WIFI_RecvData[26]);
							
					SEGGER_RTT_printf(0, "COMMAND_SETWIFIPASSWORD %d %s %d %s\n",oldLen,OldPassword,newLen,NewPassword);
							
					//��ȡ�����룬�����������ͬ������������
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
				{	//��ƥ��
					APP_Response_SetWifiPassword(0x01);
				}					
				break;
				
			case COMMAND_IOINITSTATUS:	//����IO��ʼ��״̬	OK		
				SEGGER_RTT_printf(0, "WIFI_Recv_Event%d %s\n",COMMAND_IOINITSTATUS,WIFI_RecvData);
				//�ȶԱ�ECUID�Ƿ�ƥ��
				if(!memcmp(&WIFI_RecvData[11],ECUID12,12))
				{//ƥ��ɹ�������Ӧ�Ĳ���
					//��ȡIO��ʼ״̬
					IO_Init_Status = WIFI_RecvData[23];
					APP_Response_IOInitStatus(0x00);
					APP_Response_SetChannel(0x00,New_Signal_Channel,Signal_Level);
					changeIOinit_inverter(inverterInfo,IO_Init_Status);
					//������IO��ʼ��״̬��Flash
					Write_IO_INIT_STATU(&IO_Init_Status);	
					
				}else
				{
					APP_Response_IOInitStatus(0x01);
				}
		}
	}
}


//�����¼�����
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
			case COMMAND_SET_ID:				//����ID
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
					
			case COMMAND_READ_ID:			//��ȡID
				SEGGER_RTT_printf(0, "WIFI_Recv_Event%d %s\n",COMMAND_READ_ID,USART1_RecvData);
				Read_ECUID(USART1_ECUID6);
				transformECUID(USART1_ECUID6,USART1_ECUID12);		//ת��ECU ID
				USART1_ECUID12[12] = '\0';
				if(USART1_ECUID6[0] == 0xff)
				{
					USART1_Response_READ_ID(1,USART1_ECUID12);
				}else
				{
					USART1_Response_READ_ID(0,USART1_ECUID12);
				}
				
				break;
					
			case COMMAND_TEST:			//����
				SEGGER_RTT_printf(0, "WIFI_Recv_Event%d %s\n",COMMAND_TEST,USART1_RecvData);
				
				
				break;
						
			case COMMAND_SET_NETWORK:	//����
				SEGGER_RTT_printf(0, "WIFI_Recv_Event%d %s\n",COMMAND_SET_NETWORK,USART1_RecvData);
				{	//ƥ��ɹ�������Ӧ�Ĳ���
					
					SEGGER_RTT_printf(0, "COMMAND_SETNETWORK   Mapping");
							
					AddNum = (messageUsart1Len - 17)/6;
					//������д��EEPROM
					add_inverter(inverterInfo,AddNum,(char *)&WIFI_RecvData[14]);
					APP_Response_SetNetwork(0x00);
					init_inverter(inverterInfo);
					//����һЩ�󶨲���
					bind_inverter(inverterInfo);
				}	
				break;
						
			case COMMAND_FACTORY:	//�ָ����ճ�����״̬
				SEGGER_RTT_printf(0, "WIFI_Recv_Event%d %s\n",COMMAND_FACTORY,USART1_RecvData);
				ret = Write_UID_NUM(USART1_UID_NUM);
				USART1_Response_FACTORY(ret);
				init_inverter(inverterInfo);
			
				break;
		}
	}
}

//�����¼�����
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
	
	delay_init();	    	 					//��ʱ������ʼ��	
	NVIC_Configuration(); 				//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	I2C_Init();										//FLASH оƬ��ʼ��
	LED_init();										//LED�Ƴ�ʼ��
	EXTIX_Init();									//�ָ���������IO�жϳ�ʼ��
	KEY_Init();										//�ָ��������ð�����ʼ��

	CMT2300_init();
	uart1_init(57600);
	uart2_init(57600);							//���ڳ�ʼ��
	TIM2_Int_Init(4999,7199);    //��������ʱ�¼���ʱ����ʼ��
	SEGGER_RTT_printf(0, "init OK \n");
	init_ecu();										//��ʼ��ECU
	init_inverter(inverterInfo);	//��ʼ�������
#if 1
	while(1)
	{	
		//���USART1�¼�
		USART1_GetEvent(&messageUsart1Len);
		if(USART1_Recv_Event == 1)
		{
			SEGGER_RTT_printf(0, "USART1_Recv_Event %s\n",USART1_RecvData);
			process_UART1Event();
			USART1_Recv_Event = 0;
		}
		//��ⰴ���¼�
		if(KEY_FormatWIFI_Event == 1)
		{
			process_KEYEvent();
			KEY_FormatWIFI_Event = 0;
		}
		
		//�ж��Ƿ���433ģ��������ʱ�¼�
		if(COMM_Timeout_Event == 1)
		{
			process_HeartBeatEvent();
			COMM_Timeout_Event = 0;
		}
		
		//���WIFI�¼�
		WIFI_GetEvent(&messageLen);
		//�ж��Ƿ���WIFI�����¼�
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

