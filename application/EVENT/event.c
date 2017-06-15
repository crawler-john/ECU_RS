#include "event.h"
#include "RFM300H.h"
#include "file.h"
#include "string.h"
#include "usart.h"
#include "appcomm.h"
#include "SEGGER_RTT.h"
#include "inverter.h"
#include "USART1Comm.h"
#include "delay.h"
#include "led.h"

typedef enum
{ 
    HARDTEST_TEST_ALL    	= 0,		//��������ͷ
    HARDTEST_TEST_EEPROM  = 1,	//�������ݳ���   �������ݲ��ֵĳ���Ϊ���յ����ȼ�ȥ12���ֽ�
    HARDTEST_TEST_WIFI  	= 2,	//�������ݲ�������
    HARDTEST_TEST_433  		= 3
} eHardWareID;// receive state machin


int ResolveWifiPasswd(char *oldPasswd,int *oldLen,char *newPasswd,int *newLen,char *passwdstring)
{
	*oldLen = (passwdstring[0]-'0')*10+(passwdstring[1]-'0');
	memcpy(oldPasswd,&passwdstring[2],*oldLen);
	*newLen = (passwdstring[2+(*oldLen)]-'0')*10+(passwdstring[3+(*oldLen)]-'0');
	memcpy(newPasswd,&passwdstring[4+*oldLen],*newLen);
	
	return 0;
}

//Ӳ������   ����ֵ�ǲ��ԵĴ�����
int HardwareTest(char testItem)
{
	char testWrite[10] = "YUNENG APS";
	char testRead[10] = {'\0'};
	switch(testItem)
	{
		case HARDTEST_TEST_ALL:
			//����EEPROM��д
			Write_Test(testWrite,10);				//����
			Read_Test(testRead,10);
			if(memcmp(testWrite,testRead,10))
			{
				return 1;
			}
			//����WIFIģ��
			if(0 != WIFI_Test())
			{
				return 2;
			}
			
			//����433ģ��
			
			break;
		
		case HARDTEST_TEST_EEPROM:
			//����EEPROM��д
			break;
		
		case HARDTEST_TEST_WIFI:
			//����WIFIģ��
			if(0 != WIFI_Test())
			{
				return 2;
			}
			break;
		
		case HARDTEST_TEST_433:
			//����EEPROM��д
		
			break;
				
	}
	return 0;
}

//�����¼�����
void process_HeartBeatEvent(void)
{
	int ret = 0;
	//SEGGER_RTT_printf(0, "COMM_Timeout_Event \n");
	//����������
	if(	vaildNum >0	)
	{
		//SEGGER_RTT_printf(0, "RFM300_Heart_Beat %02x%02x%02x%02x%02x%02x\n",inverterInfo[curSequence].uid[0],inverterInfo[curSequence].uid[1],inverterInfo[curSequence].uid[2],inverterInfo[curSequence].uid[3],inverterInfo[curSequence].uid[4],inverterInfo[curSequence].uid[5]);

		ret = RFM300_Heart_Beat(ECUID6,(char *)inverterInfo[curSequence].uid,(char *)&inverterInfo[curSequence].mos_status,&IO_Init_Status,&inverterInfo[curSequence].heart_rate,&inverterInfo[curSequence].off_times,&ver);
		if(ret == 0)
		{
			//�鿴�󶨱�־λ�������δ�ɹ������԰󶨡�
			if(inverterInfo[curSequence].bind_status != 1)
			{
				ret = RFM300_Bind_Uid(ECUID6,(char *)inverterInfo[curSequence].uid,0,0);
				if(ret == 1)
				{
					if(Write_UID_Bind(0x01,(curSequence+1)) == 0)
					{
						inverterInfo[curSequence].bind_status = 1;
					}
					
				}
			}
			
		}
		//�鿴�Ƿ���Ҫ�ı��ŵ�
		
		
		curSequence++;
		if(curSequence >= vaildNum)		//����ѵ����Ŵ������һ̨ʱ����������0̨
		{
			curSequence = 0;
		}
				
	}
}

//WIFI�¼�����
void process_WIFIEvent(char * ID)
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
				APP_Response_BaseInfo(ID,ECUID12,VERSION_ECU_RS,Signal_Level,"00",5,SOFEWARE_VERSION);
				break;
					
			case COMMAND_SYSTEMINFO:			//��ȡϵͳ��Ϣ			APS11002602406000000009END
				SEGGER_RTT_printf(0, "WIFI_Recv_Event%d %s\n",COMMAND_SYSTEMINFO,WIFI_RecvData);
				//�ȶԱ�ECUID�Ƿ�ƥ��
				if(!memcmp(&WIFI_RecvData[11],ECUID12,12))
				{	//ƥ��ɹ�������Ӧ�Ĳ���
					SEGGER_RTT_printf(0, "COMMAND_SYSTEMINFO  Mapping\n");
					APP_Response_SystemInfo(ID,0x00,inverterInfo,vaildNum);
				}	else
				{	//��ƥ��
					SEGGER_RTT_printf(0, "COMMAND_SYSTEMINFO   Not Mapping");
					APP_Response_SystemInfo(ID,0x01,inverterInfo,0);
				}
				break;
					
			case COMMAND_SETNETWORK:			//��������					APS11003503406000000009END111111END
				SEGGER_RTT_printf(0, "WIFI_Recv_Event%d %s\n",COMMAND_SETNETWORK,WIFI_RecvData);
				//�ȶԱ�ECUID�Ƿ�ƥ��
				if(!memcmp(&WIFI_RecvData[11],ECUID12,12))
				{	//ƥ��ɹ�������Ӧ�Ĳ���
					int AddNum = 0;
					SEGGER_RTT_printf(0, "COMMAND_SETNETWORK   Mapping");
							
					AddNum = (messageLen - 29)/6;
					//������д��EEPROM
					add_inverter(inverterInfo,AddNum,(char *)&WIFI_RecvData[26]);
					APP_Response_SetNetwork(ID,0x00);
					init_inverter(inverterInfo);
					//����һЩ�󶨲���
					LED_off();
					//bind_inverter(inverterInfo);
				}	else
				{	//��ƥ��
					APP_Response_SetNetwork(ID,0x01);
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
					APP_Response_SetChannel(ID,0x00,New_Signal_Channel,Signal_Level);
					//����ת��Ϊ1���ֽ�
					newChannel = New_Signal_Channel[0]*10+New_Signal_Channel[1];
					//changeChannel_inverter(inverterInfo,newChannel);
					//�ı��Լ����ŵ�
							
					//�������ŵ���Flash
					memcpy(Signal_Channel,New_Signal_Channel,3);
					Write_CHANNEL(Signal_Channel);
				}	else
				{	//��ƥ��
					APP_Response_SetChannel(ID,0x01,NULL,NULL);
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
							
					if(!memcmp(EEPROMPasswd,OldPassword,oldLen))
					{
						APP_Response_SetWifiPassword(ID,0x00);
						WIFI_ChangePasswd(NewPassword);
						Write_WIFI_PW(NewPassword,newLen);
					}else
					{
						APP_Response_SetWifiPassword(ID,0x02);
					}
							
				}	else
				{	//��ƥ��
					APP_Response_SetWifiPassword(ID,0x01);
				}					
				break;
				
			case COMMAND_IOINITSTATUS:	//����IO��ʼ��״̬	OK		
				SEGGER_RTT_printf(0, "WIFI_Recv_Event%d %s\n",COMMAND_IOINITSTATUS,WIFI_RecvData);
				//�ȶԱ�ECUID�Ƿ�ƥ��
				if(!memcmp(&WIFI_RecvData[11],ECUID12,12))
				{//ƥ��ɹ�������Ӧ�Ĳ���
					//��ȡIO��ʼ״̬
					IO_Init_Status = WIFI_RecvData[23];
					APP_Response_IOInitStatus(ID,0x00);

					//changeIOinit_inverter(inverterInfo,IO_Init_Status);
					//������IO��ʼ��״̬��Flash
					Write_IO_INIT_STATU(&IO_Init_Status);	
					
				}else
				{
					APP_Response_IOInitStatus(ID,0x01);
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
	char testItem = 0;
	int AddNum = 0;
	UART1_ResolveFlag =  Resolve_Usart1RecvData((char *)USART1_RecvData,&UART1_Data_Len,&UART1_Command_Id);
	if(UART1_ResolveFlag == 0)
	{
		switch(UART1_Command_Id)
		{
			case COMMAND_SET_ID:				//����ID
				SEGGER_RTT_printf(0, "WIFI_Recv_Event%d %s\n",COMMAND_SET_ID,USART1_RecvData);
				if(messageUsart1Len ==26)
				{
					memcpy(USART1_ECUID12,&USART1_RecvData[11],12);
					USART1_ECUID6[0] = ((USART1_ECUID12[0]-'0')<<4) + (USART1_ECUID12[1]-'0') ;
					USART1_ECUID6[1] = ((USART1_ECUID12[2]-'0')<<4) + (USART1_ECUID12[3]-'0') ;
					USART1_ECUID6[2] = ((USART1_ECUID12[3]-'0')<<4) + (USART1_ECUID12[5]-'0') ;
					USART1_ECUID6[3] = ((USART1_ECUID12[4]-'0')<<4) + (USART1_ECUID12[7]-'0') ;
					USART1_ECUID6[4] = ((USART1_ECUID12[5]-'0')<<4) + (USART1_ECUID12[9]-'0') ;
					USART1_ECUID6[5] = ((USART1_ECUID12[10]-'0')<<4) + (USART1_ECUID12[11]-'0') ;
					ret = Write_ECUID(USART1_ECUID6);													  		//ECU ID
					//����WIFI����
					if(ret != 0) 	USART1_Response_SET_ID(ret);
					
					//����WIFI����
					USART1_ECUID12[12] = '\0';
					delay_ms(5000);
					ret = WIFI_Factory(USART1_ECUID12);
					//д��WIFI����
					Write_WIFI_PW("88888888",8);	//WIFI����		
					
					USART1_Response_SET_ID(ret);
					init_ecu();	
				}else
				{
					USART1_Response_SET_ID(1);
				}

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
				testItem =((USART1_RecvData[11]-'0')<<4) + (USART1_RecvData[12]-'0');
				ret = HardwareTest(testItem);
				SEGGER_RTT_printf(0, "%d %d\n",testItem,ret);
				USART1_Response_TEST(testItem,ret);
				break;
						
			case COMMAND_SET_NETWORK:	//����
				SEGGER_RTT_printf(0, "WIFI_Recv_Event%d %s\n",COMMAND_SET_NETWORK,USART1_RecvData);
				{	//ƥ��ɹ�������Ӧ�Ĳ���
					
					SEGGER_RTT_printf(0, "COMMAND_SETNETWORK   Mapping\n");
							
					AddNum = (messageUsart1Len - 17)/6;
					//������д��EEPROM
					add_inverter(inverterInfo,AddNum,(char *)&USART1_RecvData[14]);
					USART1_Response_SET_NETWORK(0x00);
					init_inverter(inverterInfo);
					//����һЩ�󶨲���
					LED_off();
					//bind_inverter(inverterInfo);
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

//������ʼ�������¼�����
void process_KEYEvent(void)
{
	int ret =0,i = 0;
	SEGGER_RTT_printf(0, "KEY_FormatWIFI_Event Start\n");
		delay_ms(5000);


	for(i = 0;i<3;i++)
	{
		
		ret = WIFI_Factory(ECUID12);
		if(ret == 0) break;
	}
	
	if(ret == 0) 	//д��WIFI����
		Write_WIFI_PW("88888888",8);	//WIFI����	
	
	SEGGER_RTT_printf(0, "KEY_FormatWIFI_Event End\n");
}

//���߸�λ����
void process_WIFI_RST(void)
{
	int ret =0,i = 0;
	SEGGER_RTT_printf(0, "process_WIFI_RST Start\n");
	for(i = 0;i<3;i++)
	{
		ret = WIFI_SoftReset();
		if(ret == 0) break;
	}
	
	SEGGER_RTT_printf(0, "process_WIFI_RST End\n");
}
