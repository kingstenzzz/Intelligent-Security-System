#ifndef _LIGT_MOTO_H_
#define _LIGT_MOTO_H_
#include "stm32f10x_gpio.h"
#include "stm32f10x.h"
#include "delay.h"


#define dir_S 0
#define dir_N 1

#define Pin_1 GPIO_Pin_7
#define Pin_2 GPIO_Pin_8
#define Pin_3 GPIO_Pin_9
#define Pin_4 GPIO_Pin_10


void Shadding(void);
void  Shelter_GPIOEonfig(void);
void  UnShadding(void);
void Rotating(u16 angle,u8 dir);





#endif

