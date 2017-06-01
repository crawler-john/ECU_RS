#include "sys.h"
#include "usart.h"	
#include "SEGGER_RTT.h"
#include "timer.h"
#include "string.h"
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
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
   
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�����ж�
#endif
    USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ��� 

}

//����2�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
u8 *pCur = USART_RX_BUF;
eRecvSM eStateMachine = EN_RECV_ST_GET_HEAD;
unsigned short pos = 0;
u16 USART_RX_STA=0;       //����״̬���
unsigned short mvsize = 0;

void USART2_IRQHandler(void)                	//����1�жϷ������
	{
	unsigned Res;

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		pCur = (u8 *)USART_ReceiveData(USART2);//(USART2->DR);	//��ȡ���յ�������
		SEGGER_RTT_printf(0, "%x\n",pCur);
		pCur += 1;
    pos = 0;
		//receive start character
		if(eStateMachine == EN_RECV_ST_GET_HEAD)    //���ձ���ͷ��
		{
			// check for the start character(SYNC_CHARACTER)
      // also check it's not arriving the end of valid data
      while(&USART_RX_BUF[pos] < pCur)
      {
				mvsize = pCur - &USART_RX_BUF[pos];		//��ǰ�ڼ����ֽ�
				if(1 == mvsize)   //'A'
				{
						if(USART_RX_BUF[0] != 'A')
						{
							pCur = USART_RX_BUF;
							pos = 0;
						}
				}
				
				if(2 == mvsize)   //'P'
				{
						if(USART_RX_BUF[1] != 'P')
						{
							pCur = USART_RX_BUF;
							pos = 0;
						}
				}	
				
				if(3 == mvsize)   //'S'
				{
						if(USART_RX_BUF[2] != 'S')
						{
							pCur = USART_RX_BUF;
							pos = 0;
						}
				}
				
				if(5 == mvsize)   //���հ汾�����
				{
					eStateMachine = EN_RECV_ST_GET_LEN;

					TIM3_Int_Init(4999,7199);//10Khz�ļ���Ƶ�ʣ�������5000Ϊ500ms �򿪶�ʱ��
					break;
				}
				
				pos++;
			}
		}
		
		//receive data length
		if(eStateMachine == EN_RECV_ST_GET_LEN)
		{
			
		}
		
		//receive END
		if(eStateMachine == EN_RECV_ST_GET_END)
		{
			
		}
		//Continue to receive data
		if(eStateMachine == EN_RECV_ST_GET_DATA)
		{
			
		}
		
	}
} 
#endif	

