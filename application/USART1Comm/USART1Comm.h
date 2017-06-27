/*****************************************************************************/
/* File      : usart1_comm.h                                                 */
/*****************************************************************************/
/*  History:                                                                 */
/*****************************************************************************/
/*  Date       * Author          * Changes                                   */
/*****************************************************************************/
/*  2017-06-04 * Shengfeng Dong  * Creation of the file                      */
/*             *                 *                                           */
/*****************************************************************************/
#ifndef __USART1_COMM_H__
#define __USART1_COMM_H__

/*****************************************************************************/
/*  Include Files                                                            */
/*****************************************************************************/
#include "usart.h"


/*****************************************************************************/
/*  Definitions                                                              */
/*****************************************************************************/

/*****************************************************************************/
/*  Variable Declarations                                                    */
/*****************************************************************************/
typedef enum
{ 
    COMMAND_SET_ID      = 1,		//��������ͷ
    COMMAND_READ_ID     = 2,	//�������ݳ���   �������ݲ��ֵĳ���Ϊ���յ����ȼ�ȥ12���ֽ�
    COMMAND_TEST      	= 3,	//�������ݲ�������
    COMMAND_SET_NETWORK = 4,		//����END��β��־
		COMMAND_FACTORY 		= 5
		
} eUSART1CommandID;// receive state machin


/*****************************************************************************/
/*  Function Declarations                                                    */
/*****************************************************************************/
int Resolve_Usart1RecvData(char *RecvData,int* Data_Len,int* Command_Id);

void USART1_Response_SET_ID(char setflag);
void USART1_Response_READ_ID(char readflag,char *ECUID12);
void USART1_Response_TEST(char testItem,char testflag);
void USART1_Response_SET_NETWORK(char setflag);
void USART1_Response_FACTORY(char setflag);
#endif /*__USART1_COMM_H__*/
