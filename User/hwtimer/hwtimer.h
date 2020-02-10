#ifndef __HWTIMER_H
#define __HWTIMER_H
#include "stm32f10x.h"
#include "stm32f10x_it.h"

void NVIC_con(void);
void gpio_Con(void);
void TIM2_con(void);
void TIM_IRQHandler(void);
void Tim2_init(void);
void uDelay(uint32_t time);




#endif
