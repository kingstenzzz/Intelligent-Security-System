#include "delay.h"

static __IO u32 TimingDelay;
__IO uint32_t g_ul_ms_ticks=0;



/*
************************************************************
*	函数名称：	SysTick_Init
*	函数功能：	启动系统滴答定时器 SysTick
*	入口参数：	无
*	返回参数：	无
*	说明：		  
************************************************************
*/
void SysTick_Init(void)
{
	/* SystemFrequency / 1000    1ms中断一次
	* SystemFrequency / 100000	 10us中断一次
	* SystemFrequency / 1000000  1us中断一次
	*/
	//if (SysTick_Config(SystemFrequency / 100000))	// ST3.0.0库版本
	if(SysTick_Config(SystemCoreClock / 1000))	// ST3.5.0库版本
	{ 
		/* Capture error */ 
		while(1);
	}
	// 关闭滴答定时器  
	SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;
}


/*
************************************************************
*	函数名称：	Delay_us
*	函数功能：	us延时程序,10us为一个单位
*	入口参数：	nTime: Delay_us( 1 ) 则实现的延时为 1 * 10us = 10us
*	返回参数：	无
*	说明：		  
************************************************************
*/
void Delay_us(__IO uint32_t nTime)
{ 
	TimingDelay = nTime;	

	// 使能滴答定时器  
	SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk;

	while(TimingDelay != 0);
}


/*
************************************************************
*	函数名称：	TimingDelay_Decrement
*	函数功能：	获取节拍程序
*	入口参数：	无
*	返回参数：	无
*	说明：		在 SysTick 中断函数 SysTick_Handler()调用		  
************************************************************
*/
void TimingDelay_Decrement(void)
{
	if(TimingDelay != 0x00)
	{ 
		TimingDelay--;
	}
}

