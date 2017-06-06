#include "file.h"
#include "string.h"

int Write_Test(char *Data_Ptr,unsigned char Counter)
{
	char test[10] = {'\0'};
	//先写入然后再读取  如果相同表示成功
	Write_24L512_nByte(ADDRESS_Test,Counter,(unsigned char *)Data_Ptr);

	Read_24L512_nByte(ADDRESS_Test,Counter, (unsigned char *)test);
	if(!memcmp(Data_Ptr,test,Counter))
		return 0;
	else
		return 1;
}

int Read_Test(char *Data_Ptr,unsigned char Counter)
{
	if(Data_Ptr == NULL)
		return -1;
	Read_24L512_nByte(ADDRESS_Test,Counter, (unsigned char *)Data_Ptr);
	return 0;
}

int Write_ECUID(char *ECUID)             //ECU id
{
	char id[6] = {'\0'};
	//先写入然后再读取  如果相同表示成功
	Write_24L512_nByte(ADDRESS_ECUID,6,(unsigned char *)ECUID);

	Read_24L512_nByte(ADDRESS_ECUID,6, (unsigned char *)id);
	if(!memcmp(ECUID,id,6))
		return 0;
	else
		return 1;
}

int Read_ECUID(char *ECUID)
{
	if(ECUID == NULL)
		return -1;
	Read_24L512_nByte(ADDRESS_ECUID,6, (unsigned char *)ECUID);
	return 0;
}

void transformECUID(char * ECUID6,char *ECUID12)
{
	ECUID12[0] = (((ECUID6[0]&0xF0)>>4)+'0');
	ECUID12[1] = (((ECUID6[0]&0x0F))+'0');
	ECUID12[2] = (((ECUID6[1]&0xF0)>>4)+'0');
	ECUID12[3] = (((ECUID6[1]&0x0F))+'0');
	ECUID12[4] = (((ECUID6[2]&0xF0)>>4)+'0');
	ECUID12[5] = (((ECUID6[2]&0x0F))+'0');
	ECUID12[6] = (((ECUID6[3]&0xF0)>>4)+'0');
	ECUID12[7] = (((ECUID6[3]&0x0F))+'0');
	ECUID12[8] = (((ECUID6[4]&0xF0)>>4)+'0');
	ECUID12[9] = (((ECUID6[4]&0x0F))+'0');
	ECUID12[10] = (((ECUID6[5]&0xF0)>>4)+'0');
	ECUID12[11] = (((ECUID6[5]&0x0F))+'0');
}

int Write_CHANNEL(char *Channel)       //信道
{
	char channel[2] = {'\0'};
	//先写入然后再读取  如果相同表示成功
	Write_24L512_nByte(ADDRESS_CHANNEL,2,(unsigned char *)Channel);

	Read_24L512_nByte(ADDRESS_CHANNEL,2, (unsigned char *)channel);
	if(!memcmp(Channel,channel,2))
		return 0;
	else
		return 1;
}

int Read_CHANNEL(char *Channel)
{
	if(Channel == NULL)
		return -1;
	Read_24L512_nByte(ADDRESS_CHANNEL,2, (unsigned char *)Channel);
	return 0;
}

int Write_RATE(char *Rate)       //速度
{
	char rate[2] = {'\0'};
	//先写入然后再读取  如果相同表示成功
	Write_24L512_nByte(ADDRESS_RATE,2,(unsigned char *)Rate);

	Read_24L512_nByte(ADDRESS_RATE,2, (unsigned char *)rate);
	if(!memcmp(Rate,rate,2))
		return 0;
	else
		return 1;
}

int Read_RATE(char *Rate)
{
	if(Rate == NULL)
		return -1;
	Read_24L512_nByte(ADDRESS_RATE,2, (unsigned char *)Rate);
	return 0;
}

int Write_IO_STATUS(char *IO_Status)       //IO状态
{
	char io_status[1] = {'\0'};
	//先写入然后再读取  如果相同表示成功
	Write_24L512_nByte(ADDRESS_IO_STATUS,1,(unsigned char *)IO_Status);

	Read_24L512_nByte(ADDRESS_IO_STATUS,1, (unsigned char *)io_status);
	if(!memcmp(IO_Status,io_status,1))
		return 0;
	else
		return 1;
}

int Read_IO_STATUS(char *IO_Status)
{
	if(IO_Status == NULL)
		return -1;
	Read_24L512_nByte(ADDRESS_IO_STATUS,1, (unsigned char *)IO_Status);
	return 0;
}

int Write_IO_INIT_STATU(char *IO_InitStatus)       //IO上电状态
{
	char io_init_status[1] = {'\0'};
	//先写入然后再读取  如果相同表示成功
	Write_24L512_nByte(ADDRESS_IO_INIT_STATUS,1,(unsigned char *)IO_InitStatus);

	Read_24L512_nByte(ADDRESS_IO_INIT_STATUS,1, (unsigned char *)io_init_status);
	if(!memcmp(IO_InitStatus,io_init_status,1))
		return 0;
	else
		return 1;
}

int Read_IO_INIT_STATU(char *IO_InitStatus)
{
	if(IO_InitStatus == NULL)
		return -1;
	Read_24L512_nByte(ADDRESS_IO_INIT_STATUS,1, (unsigned char *)IO_InitStatus);
	return 0;
}

int Write_HEART_NUM(char *HeartNum)       //接受心跳包数量
{
	char heart_num[2] = {'\0'};
	//先写入然后再读取  如果相同表示成功
	Write_24L512_nByte(ADDRESS_HEART_NUM,2,(unsigned char *)HeartNum);

	Read_24L512_nByte(ADDRESS_HEART_NUM,2, (unsigned char *)heart_num);
	if(!memcmp(HeartNum,heart_num,2))
		return 0;
	else
		return 1;
}

int Read_HEART_NUM(char *HeartNum)
{
	if(HeartNum == NULL)
		return -1;
	Read_24L512_nByte(ADDRESS_HEART_NUM,2, (unsigned char *)HeartNum);
	return 0;
}

int Write_HEART_TIMEOUT_NUM(char *Heart_TimeoutNum)       //心跳包超时数量
{
	char heart_timeout_num[2] = {'\0'};
	//先写入然后再读取  如果相同表示成功
	Write_24L512_nByte(ADDRESS_HEART_TIMEOUT_NUM,2,(unsigned char *)Heart_TimeoutNum);

	Read_24L512_nByte(ADDRESS_HEART_TIMEOUT_NUM,2, (unsigned char *)heart_timeout_num);
	if(!memcmp(Heart_TimeoutNum,heart_timeout_num,2))
		return 0;
	else
		return 1;
}

int Read_HEART_TIMEOUT_NUM(char *Heart_TimeoutNum)
{
	if(Heart_TimeoutNum == NULL)
		return -1;
	Read_24L512_nByte(ADDRESS_HEART_TIMEOUT_NUM,2, (unsigned char *)Heart_TimeoutNum);
	return 0;
}

int Write_WIFI_PW(char *WIFIPasswd,unsigned char Counter)       //WIFI密码
{
	char wifi_pw[12] = {'\0'};
	//先写入然后再读取  如果相同表示成功
	Write_24L512_nByte(ADDRESS_WIFI_PW,Counter,(unsigned char *)WIFIPasswd);

	Read_24L512_nByte(ADDRESS_WIFI_PW,Counter, (unsigned char *)wifi_pw);
	if(!memcmp(WIFIPasswd,wifi_pw,Counter))
		return 0;
	else
		return 1;
}

int Read_WIFI_PW(char *WIFIPasswd,unsigned char Counter)
{
	if(WIFIPasswd == NULL)
		return -1;
	Read_24L512_nByte(ADDRESS_WIFI_PW,Counter, (unsigned char *)WIFIPasswd);
	return 0;
}

int Write_UID_NUM(char *UID_NUM)						//UID_NUM
{
	char uid_num[2] = {'\0'};

	//先写入然后再读取  如果相同表示成功
	Write_24L512_nByte(ADDRESS_UID_NUM,2,(unsigned char *)UID_NUM);
	Read_24L512_nByte(ADDRESS_UID_NUM,2, (unsigned char *)uid_num);
	if(!memcmp(UID_NUM,uid_num,6))
		return 0;
	else
		return 1;
}
int Read_UID_NUM(char *UID_NUM)
{
	if(UID_NUM == NULL)
		return -1;
	Read_24L512_nByte(ADDRESS_UID_NUM,2, (unsigned char *)UID_NUM);
	return 0;
}

int Write_UID(char *UID,int n)      //UID
{

	char uid[6] = {'\0'};

	//先写入然后再读取  如果相同表示成功
	Write_24L512_nByte(ADDRESS_UID+0x06*(n-1),6,(unsigned char *)UID);
	Read_24L512_nByte(ADDRESS_UID+0x06*(n-1),6, (unsigned char *)uid);
	if(!memcmp(UID,uid,6))
		return 0;
	else
		return 1;
	
}

int Read_UID(char *UID,int n)
{
	if(UID == NULL)
		return -1;
	Read_24L512_nByte(ADDRESS_UID+0x06*(n-1),6, (unsigned char *)UID);
	return 0;
}
