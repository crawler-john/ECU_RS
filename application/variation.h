#ifndef __VARIATION_H__
#define __VARIATION_H__

#define MAXINVERTERCOUNT 100	//最大的逆变器数
#define INVERTERLENGTH 11	//最大的逆变器数

typedef struct restartNum
{
    char pre_restart_num:4;
    char cur_restart_num:4;
}restart_num_t;


typedef struct inverter_info_t{
	unsigned char uid[6];		//逆变器ID（逆变器ID的BCD编码）
	unsigned short heart_rate;	//心跳次数
	unsigned short off_times;	//心跳超时次数
	unsigned char mos_status;	//0x01 开、0x00 关
	unsigned char bind_status;	//绑定状态  
	unsigned char channel;	//信道状态
	unsigned char heart_Failed_times;	//连续通信失败次数  ，当大于3的时候默认该RSD2为关机状态
	restart_num_t restartNum;
}inverter_info;


#endif /*__VARIATION_H__*/
