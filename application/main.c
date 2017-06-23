/*****************************************************************************/
/* File      : main.c                                                        */
/*****************************************************************************/
/*  History:                                                                 */
/*****************************************************************************/
/*  Date       * Author          * Changes                                   */
/*****************************************************************************/
/*  2017-06-02 * Shengfeng Dong  * Creation of the file                      */
/*             *                 *                                           */
/*****************************************************************************/

/*****************************************************************************/
/*  Include Files                                                            */
/*****************************************************************************/
#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "Flash_24L512.h"
#include "SEGGER_RTT.h"
#include "led.h"
#include "timer.h"
#include "string.h"
#include "RFM300H.h"
#include "variation.h"
#include "event.h"
#include "inverter.h"

/*****************************************************************************/
/*  Variable Declarations                                                    */
/*****************************************************************************/
char ECUID12[13] = {'\0'};
char ECUID6[7] = {'\0'};
char Signal_Level = 0;
char Signal_Channel[3] = {'\0'};
char Channel_char = 0;
char IO_Init_Status = 0;			//IO��ʼ״̬
char ver = 0;						//�Ż����汾��
int vaildNum = 0;				//��ǰ��Ч̨��
int curSequence = 0;		//������ѵ������
inverter_info inverterInfo[MAXINVERTERCOUNT] = {'\0'};
int Data_Len = 0,Command_Id = 0,ResolveFlag = 0,messageLen = 0,messageUsart1Len = 0;
int UART1_Data_Len = 0,UART1_Command_Id = 0,UART1_ResolveFlag = 0;
unsigned char ID[9] = {'\0'};

/*****************************************************************************/
/*  Function Implementations                                                 */
/*****************************************************************************/
int main(void)
{		
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
		//���WIFI�¼�
		WIFI_GetEvent(&messageLen,ID);
		//�ж��Ƿ���WIFI�����¼�
		if(WIFI_Recv_Event == 1)
		{
			process_WIFIEvent(ID);
			WIFI_Recv_Event = 0;
		}
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
		
		//WIFI��λ�¼�
		if(WIFI_RST_Event == 1)
		{
			process_WIFI_RST();
			WIFI_RST_Event = 0;
		}
		
		//�ж��Ƿ���433ģ��������ʱ�¼�
		if(COMM_Timeout_Event == 1)
		{
			process_HeartBeatEvent();
			COMM_Timeout_Event = 0;
		}
		

		
	}	
#endif
 }

