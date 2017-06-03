#ifndef __FILE_H__
#define __FILE_H__
#include "Flash_24L512.h"

#define  ADDRESS_Test 0x000000


int Write_Test(char *Data_Ptr,unsigned char Counter);
int Read_Test(char *Data_Ptr,unsigned char Counter);


#endif /*__FILE_H__*/
