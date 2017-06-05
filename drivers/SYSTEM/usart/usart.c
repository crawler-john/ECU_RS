#include "sys.h"
#include "usart.h"	
#include "SEGGER_RTT.h"
#include "timer.h"
#include "string.h"
#include "delay.h"


#define WIFI_RCC                    RCC_APB2Periph_GPIOB
#define WIFI_GPIO                   GPIOB
#define WIFI_PIN                    (GPIO_Pin_15)

unsigned short packetlen(unsigned char *packet)
{
	unsigned short len = 0;
	len = ((packet[0]-'0')*1000 +(packet[1]-'0')*100 + (packet[2]-'0')*10 + (packet[3]-'0'));
	return len;
}

//WIFI���ͺ��� 
int WIFI_SendData(char *data, int num)
{      
	int index = 0;
	char ch = 0;
	for(index = 0;index < num;index++)
	{
		ch = data[index];
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET); 
    USART_SendData(USART2,(uint8_t)ch);
	}
	return index;
}

 
#if EN_USART2_RX   //���ʹ���˽���
//��ʼ��IO ����1 
//bound:������
void uart_init(u32 bound){
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//ʹ�ܴ���ʱ��USART2
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��GPIOAʱ��
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART2|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART2��GPIOAʱ��
 	USART_DeInit(USART2);  //��λ����1
	 //USART2_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA9
   
    //USART2_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA10
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

    USART_Init(USART2, &USART_InitStructure); //��ʼ������
#if EN_USART2_RX		  //���ʹ���˽���  
   //USART2 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
   
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�����ж�
#endif
    USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ��� 


    RCC_APB2PeriphClockCmd(WIFI_RCC,ENABLE);

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin   = WIFI_PIN;
    GPIO_Init(WIFI_GPIO, &GPIO_InitStructure);
		GPIO_SetBits(WIFI_GPIO, WIFI_PIN);

}

//����2�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	

unsigned char WIFI_RecvData[USART_REC_LEN] = {'\0'};
unsigned char WIFI_Recv_Event = 0;

unsigned char USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
unsigned short Cur = 0;		//��ǰ��ֵλ��
unsigned short PackLen = 0;
eRecvSM eStateMachine = EN_RECV_ST_GET_HEAD;	//���ݲɼ�״̬��
unsigned short pos = 0;				//���ݽ���λ��
unsigned short mvsize = 0;

void USART2_IRQHandler(void)                	//����1�жϷ������
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		USART_RX_BUF[Cur] = USART_ReceiveData(USART2);//(USART2->DR);	//��ȡ���յ�������
		//SEGGER_RTT_printf(0, "%x %c\n",USART_RX_BUF[Cur],USART_RX_BUF[Cur]);
		Cur +=1;

	}
} 

void WIFI_GetEvent(int *messageLen)
{
	  pos = 0;
		//receive start character
		if(eStateMachine == EN_RECV_ST_GET_HEAD)    //���ձ���ͷ��
		{
			////SEGGER_RTT_printf(0, "EN_RECV_ST_GET_HEAD\n");
			// check for the start character(SYNC_CHARACTER)
      // also check it's not arriving the end of valid data
      while(pos < Cur)
      {
				TIM3_Int_Deinit();
				mvsize = Cur - pos;		//��ǰ�ڼ����ֽ�
				if(1 == mvsize)   //'A'
				{
						if(USART_RX_BUF[0] != 'A')
						{
							Cur = 0;
							pos = 0;
						}
				}
				
				if(2 == mvsize)   //'P'
				{
						if(USART_RX_BUF[1] != 'P')
						{
							Cur = 0;
							pos = 0;
						}
				}	
				
				if(3 == mvsize)   //'S'
				{
						if(USART_RX_BUF[2] != 'S')
						{
							Cur = 0;
							pos = 0;
						}
				}
				
				if(5 == mvsize)   //���հ汾�����
				{
					//SEGGER_RTT_printf(0, "APS11\n");
					eStateMachine = EN_RECV_ST_GET_LEN;

					TIM3_Int_Init(149,7199);//10Khz�ļ���Ƶ�ʣ�������5000Ϊ500ms �򿪶�ʱ��
					break;
				}
				
				TIM3_Int_Init(149,7199);//10Khz�ļ���Ƶ�ʣ�������5000Ϊ500ms �򿪶�ʱ��
				
				pos++;
			}
		}
		
		//receive data length
		if(eStateMachine == EN_RECV_ST_GET_LEN)
		{
			////SEGGER_RTT_printf(0, "EN_RECV_ST_GET_LEN\n");
			while(pos < Cur)
      {
				TIM3_Int_Deinit();
				mvsize = Cur - pos;		//��ǰ�ڼ����ֽ�
				if(9 == mvsize)   //�������ݳ��Ƚ���
				{
					PackLen = packetlen(&USART_RX_BUF[5]);
					//SEGGER_RTT_printf(0, "LENGTH : %d\n",PackLen);
					//���㳤��
					eStateMachine = EN_RECV_ST_GET_DATA;

					TIM3_Int_Init(149,7199);//10Khz�ļ���Ƶ�ʣ�������5000Ϊ500ms �򿪶�ʱ��
					break;
				}
				TIM3_Int_Init(149,7199);//10Khz�ļ���Ƶ�ʣ�������5000Ϊ500ms �򿪶�ʱ��
				pos++;
			}
		}
		
		//Continue to receive data
		if(eStateMachine == EN_RECV_ST_GET_DATA)
		{
			////SEGGER_RTT_printf(0, "EN_RECV_ST_GET_DATA\n");
			while(pos < Cur)
      {
				TIM3_Int_Deinit();
				mvsize = Cur - pos;		//��ǰ�ڼ����ֽ�
				if((PackLen - 3) == mvsize)   //�������ݳ��Ƚ���
				{
					eStateMachine = EN_RECV_ST_GET_END;

					TIM3_Int_Init(149,7199);//10Khz�ļ���Ƶ�ʣ�������5000Ϊ500ms �򿪶�ʱ��
					break;
				}
				TIM3_Int_Init(149,7199);//10Khz�ļ���Ƶ�ʣ�������5000Ϊ500ms �򿪶�ʱ��
				pos++;
			}
		}
		
		
		
		//receive END
		if(eStateMachine == EN_RECV_ST_GET_END)
		{
			////SEGGER_RTT_printf(0, "EN_RECV_ST_GET_END\n");
			while(pos < Cur)
      {
				TIM3_Int_Deinit();
				mvsize = Cur - pos;		//��ǰ�ڼ����ֽ�
				if((PackLen - 2) == mvsize)   //'A'
				{
						if(USART_RX_BUF[PackLen - 3] != 'E')
						{
							Cur = 0;
							pos = 0;
						}
				}
				
				if((PackLen - 1) == mvsize)   //'P'
				{
						if(USART_RX_BUF[PackLen - 2] != 'N')
						{
							Cur = 0;
							pos = 0;
						}
				}	
				
				if((PackLen) == mvsize)   //'S'
				{
						if(USART_RX_BUF[PackLen - 1] != 'D')
						{
							Cur = 0;
							pos = 0;
						}
						//SEGGER_RTT_printf(0, "EN_RECV_ST_GET_END OVER\n");
						
						//���Ľ������
						//������ϵ���Ӧ����
						//���ɼ��ɹ������ݸ��Ƶ��ɹ�����
						memset(WIFI_RecvData,0x00,USART_REC_LEN);
						memcpy(WIFI_RecvData,USART_RX_BUF,PackLen);
						*messageLen = PackLen;
						WIFI_RecvData[PackLen] = '\0';
						WIFI_Recv_Event = 1;
						//SEGGER_RTT_printf(0, "WIFI_RecvData :%s\n",WIFI_RecvData);
						eStateMachine = EN_RECV_ST_GET_HEAD;
						Cur = 0;
						pos = 0;
						
						TIM3_Int_Init(149,7199);//10Khz�ļ���Ƶ�ʣ�������5000Ϊ500ms �򿪶�ʱ��
						break;
				}
				
				TIM3_Int_Init(149,7199);//10Khz�ļ���Ƶ�ʣ�������5000Ϊ500ms �򿪶�ʱ��
				
				pos++;
			}
		}
}

void clear_WIFI(void)
{
	TIM3_Int_Deinit();
	eStateMachine = EN_RECV_ST_GET_HEAD;
	Cur = 0;
}

//����ATģʽ
int AT(void)
{
	clear_WIFI();
	//����ģ��д��"+++"Ȼ����д��"a" д��+++����"a" д��"a"����+ok
	WIFI_SendData("+++", 3);
	//��ȡ��a
	delay_ms(50);
	if(Cur <1)
	{
		return -1;
	}else
	{
		if(memcmp(USART_RX_BUF,"a",1))
		{
			return -1;
		}
	}
	
	//���ŷ���a
	clear_WIFI();
	WIFI_SendData("a", 1);
	delay_ms(50);
	if(Cur < 3)
	{
		return -1;
	}else
	{
		if(memcmp(USART_RX_BUF,"+ok",3))
		{
			return -1;
		}

	}
	SEGGER_RTT_printf(0, "AT :a+ok\n");
	clear_WIFI();
	return 0;
}


//�л���ԭ���Ĺ���ģʽ    OK
int AT_ENTM(void)
{

	clear_WIFI();
	//����"AT+ENTM\n",����+ok
	WIFI_SendData("AT+ENTM\n", 8);
	delay_ms(300);
	if(Cur < 10)
	{
		return -1;
	}else
	{
		if(memcmp(&USART_RX_BUF[9],"+ok",3))
		{
			return -1;
		}

	}
	SEGGER_RTT_printf(0, "AT+ENTM :+ok\n");
	clear_WIFI();
	return 0;
	
}


//����WIFI����
int AT_WAKEY(char *NewPasswd)
{
	char AT[100] = { '\0' };
	clear_WIFI();
	//����"AT+WAKEY\n",����+ok
	sprintf(AT,"AT+WAKEY=WPA2PSK,AES,%s\n",NewPasswd);
	SEGGER_RTT_printf(0, "%s",AT);
	WIFI_SendData(AT, (strlen(AT)+1));
	
	delay_ms(1000);
	
	if(Cur < 10)
	{
		return -1;
	}else
	{
		if(memcmp(&USART_RX_BUF[strlen(AT)+1],"+ok",3))
		{
			return -1;
		}
	}
	SEGGER_RTT_printf(0, "AT+WAKEY :+ok\n");
	clear_WIFI();
	return 0;
}



int WIFI_ChangePasswd(char *NewPasswd)
{
	int ret = 0,index;
	for(index = 0;index<3;index++)
	{
		delay_ms(200);
		ret =AT();
		if(ret == 0) break;
	}
	if(ret == -1) return -1;
	
	delay_ms(200);
	
		for(index = 0;index<3;index++)
	{
		delay_ms(200);
		ret =AT_WAKEY(NewPasswd);
		if(ret == 0) break;
	}
	if(ret == -1)
	{
		for(index = 0;index<3;index++)
		{
			delay_ms(200);
			ret =AT_ENTM();;
			if(ret == 0) break;
		}
	
		return -1;
	}		
	
	for(index = 0;index<3;index++)
	{
		delay_ms(200);
		ret =AT_ENTM();;
		if(ret == 0) break;
	}
	if(ret == -1) return -1;
	
	WIFI_Reset();	
	return 0;
}

int WIFI_Reset(void)
{
	GPIO_SetBits(WIFI_GPIO, WIFI_PIN);
	
	delay_ms(1000);
	GPIO_ResetBits(WIFI_GPIO, WIFI_PIN);
	return 0;
}


#endif	




