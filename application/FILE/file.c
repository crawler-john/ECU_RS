#include "file.h"
#include "string.h"

int Write_Test(char *Data_Ptr,unsigned char Counter)
{
	char test[10] = {'\0'};
	//��д��Ȼ���ٶ�ȡ  �����ͬ��ʾ�ɹ�
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
