#ifndef __VARIATION_H__
#define __VARIATION_H__

#define MAXINVERTERCOUNT 100	//�����������
#define INVERTERLENGTH 11	//�����������

typedef struct inverter_info_t{
	unsigned char uid[6];		//�����ID�������ID��BCD���룩
	unsigned short heart_rate;	//��������
	unsigned short off_times;	//������ʱ����
	unsigned char mos_status;	//0x01 ����0x00 ��
	unsigned char bind_status;	//��״̬  
	unsigned char channel;	//�ŵ�״̬
	unsigned char heart_Failed_times;
}inverter_info;


#endif /*__VARIATION_H__*/
