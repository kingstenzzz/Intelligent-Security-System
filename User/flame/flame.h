#ifndef __FLAME_H
#define	__FLAME_H

#include "stm32f10x.h"


#define FIRE_Dout_0		GPIO_ResetBits(GPIODE, GPIO_Pin_5)
#define FIRE_Dout_1		GPIO_SetBits(GPIOE, GPIO_Pin_5) 
#define FIRE_Data_IN()	GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5)


void Fire_Init(void);

#endif 
