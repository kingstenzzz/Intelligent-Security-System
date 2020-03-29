#ifndef __TIMER_H
#define __TIMER_H
#include "stm32f10x.h"


void TIM4_PWM_Init();
void GPIO(void);
void Set_angle(_Bool dir,u8 angle);

#endif