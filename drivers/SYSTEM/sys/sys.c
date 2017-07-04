#include "sys.h"

void NVIC_Configuration(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级

}

#include <stm32f10x.h>

#define RFM_rcc                    RCC_APB2Periph_GPIOB
#define RFM_gpio                   GPIOB
#define RFM_pin                    (GPIO_Pin_0)

void RFM_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RFM_rcc,ENABLE);

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin   = RFM_pin;
    GPIO_Init(RFM_gpio, &GPIO_InitStructure);
		GPIO_SetBits(RFM_gpio, RFM_pin);
}

void RFM_on(void)
{
		GPIO_SetBits(RFM_gpio, RFM_pin);   
}

void RFM_off(void)
{
    GPIO_ResetBits(RFM_gpio, RFM_pin);
}