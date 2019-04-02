
#include "iwdg.h"

void IWDG_Config(uint16_t prv,uint16_t rlv)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);//开锁
	IWDG_SetPrescaler(prv);//分频0-255
	IWDG_SetReload(rlv);//重装值
	//4分频，重装值624，看门狗溢出时间大概为1000ms
	
	IWDG_ReloadCounter();//喂狗
	IWDG_Enable();//使能看门狗
}


void Feed()
	{
		IWDG_ReloadCounter();
	}
	