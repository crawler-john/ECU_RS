#ifndef __VARIATION_H__
#define __VARIATION_H__

#define MAXINVERTERCOUNT 100	//�����������
#define INVERTERLENGTH 11	//�����������

typedef struct restartNum
{
    char pre_restart_num:4;
    char cur_restart_num:4;
}restart_num_t;


typedef struct inverter_info_t{
	unsigned char uid[6];		//�����ID�������ID��BCD���룩
	unsigned short heart_rate;	//��������
	unsigned short off_times;	//������ʱ����
	unsigned char mos_status;	//0x01 ����0x00 ��
	unsigned char bind_status;	//��״̬  
	unsigned char channel;	//�ŵ�״̬
	unsigned char heart_Failed_times;	//����ͨ��ʧ�ܴ���  ��������3��ʱ��Ĭ�ϸ�RSD2Ϊ�ػ�״̬
	restart_num_t restartNum;
}inverter_info;


#endif /*__VARIATION_H__*/
