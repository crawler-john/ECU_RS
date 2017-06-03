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

char ECUID12[13] = {'\0'};
char ECUID6[7] = {'\0'};
char Signal_Level[4] = {'\0'};
char Signal_Channel[3] = {'\0'};
char New_Signal_Channel[3] = {'\0'};



int main(void)
{	
//	unsigned char eepromSenddata[32] = "YUNENG APSYUNENG APSYUNENG APS1";
//	unsigned char eepromRecvdata[32] = {'\0'};	
//	unsigned char ret = 0;
	int Data_Len = 0,Command_Id = 0,ResolveFlag = 0;
	
	delay_init();	    	 					//��ʱ������ʼ��	  
	NVIC_Configuration(); 				//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	I2C_Init();										//FLASH оƬ��ʼ��
	LED_init();										//LED�Ƴ�ʼ��
	KEY_Init();										//�ָ��������ð�����ʼ��
	EXTIX_Init();									//�ָ���������IO�жϳ�ʼ��
	CMT2300_init();
	uart_init(57600);							//���ڳ�ʼ��
	TIM2_Int_Init(14999,7199);    //��������ʱ�¼���ʱ����ʼ��
	SEGGER_RTT_printf(0, "init OK \n");
//	ECUID6[0] = 0x40;
//	ECUID6[1] = 0x60;
//	ECUID6[2] = 0x00;
//	ECUID6[3] = 0x00;
//	ECUID6[4] = 0x00;
//	ECUID6[5] = 0x09;	
//	Write_ECUID(ECUID6);						//��ȡECU ID	
//	Read_ECUID(ECUID6);						//��ȡECU ID
	transformECUID(ECUID6,ECUID12);		//ת��ECU ID
	
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
	
	while(1)
	{
		//�ж��Ƿ���433ģ��������ʱ�¼�
		if(COMM_Timeout_Event == 1)
		{
			SEGGER_RTT_printf(0, "COMM_Timeout_Event \n");
			COMM_Timeout_Event = 0;
		}
		
		//�ж��Ƿ���WIFI�����¼�
		if(WIFI_Recv_Event == 1)
		{
			ResolveFlag =  Resolve_RecvData((char *)WIFI_RecvData,&Data_Len,&Command_Id);
			if(ResolveFlag == 0)
			{
				switch(Command_Id)
				{
					case COMMAND_BASEINFO:				//��ȡ������Ϣ
						SEGGER_RTT_printf(0, "WIFI_Recv_Event%d %s\n",COMMAND_BASEINFO,WIFI_RecvData);
						//��ȡ������Ϣ
						APP_Response_BaseInfo(ECUID12,VERSION_ECU_RS,Signal_Level,Signal_Channel,5,SOFEWARE_VERSION);
						break;
					
					case COMMAND_SYSTEMINFO:			//��ȡϵͳ��Ϣ
						SEGGER_RTT_printf(0, "WIFI_Recv_Event%d %s\n",COMMAND_SYSTEMINFO,WIFI_RecvData);
						
						break;
					
					case COMMAND_SETNETWORK:			//��������
						SEGGER_RTT_printf(0, "WIFI_Recv_Event%d %s\n",COMMAND_SETNETWORK,WIFI_RecvData);
						//�ȶԱ�ECUID�Ƿ�ƥ��
						if(!memcmp(&WIFI_RecvData[9],ECUID12,12))
						{	//ƥ��ɹ�������Ӧ�Ĳ���
							
							APP_Response_SetNetwork(0x00);
						}	else
						{	//��ƥ��
							APP_Response_SetNetwork(0x01);
						}
						break;
						
					case COMMAND_SETCHANNEL:			//�����ŵ�
						SEGGER_RTT_printf(0, "WIFI_Recv_Event%d %s\n",COMMAND_SETCHANNEL,WIFI_RecvData);
						if(!memcmp(&WIFI_RecvData[9],ECUID12,12))
						{	//ƥ��ɹ�������Ӧ�Ĳ���
							memcpy(New_Signal_Channel,&WIFI_RecvData[26],2);
							APP_Response_SetChannel(0x00,New_Signal_Channel,Signal_Level);
						}	else
						{	//��ƥ��
							APP_Response_SetChannel(0x01,NULL,NULL);
						}
						break;
						
					case COMMAND_SETWIFIPASSWORD:	//����WIFI����
						SEGGER_RTT_printf(0, "WIFI_Recv_Event%d %s\n",COMMAND_SETWIFIPASSWORD,WIFI_RecvData);
						//�ȶԱ�ECUID�Ƿ�ƥ��
						if(!memcmp(&WIFI_RecvData[9],ECUID12,12))
						{	//ƥ��ɹ�������Ӧ�Ĳ���
							
							APP_Response_SetWifiPassword(0x00);
						}	else
						{	//��ƥ��
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
	
	/*
	while(1)
	{		
	SEGGER_RTT_printf(0, "111111\n");
		SendMessage(eepromSenddata,31);
		RF_leng =1;
		delay_ms(2000);
		//RF_leng = GetMessage(eepromRecvdata);
		//if(RF_leng)
		//{
		//	RF_leng = 0;
		//	RFM300H_SW = 0;
		//}
	}
	*/
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

