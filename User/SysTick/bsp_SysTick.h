#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f10x.h"


#define TASK_ENABLE 0
#define NumOfTask 3

void SysTick_Init(void);
void mdelay(unsigned long nTime);

int get_tick_count(unsigned long *count);
void TimeStamp_Increment(void);
void Delay_us(__IO u32 nTime);         // 单位1us

#define Delay_ms(x) Delay_us(1000*x); //单位ms


#endif /* __SYSTICK_H */
