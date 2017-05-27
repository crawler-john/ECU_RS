/*
 * File      : led.h
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 */

#ifndef __LED_H__
#define __LED_H__


void rt_hw_led_init(void);	//LED��ʼ��
void rt_hw_led_on(void);		//LED��
void rt_hw_led_off(void);		//LEDϨ��

#endif
