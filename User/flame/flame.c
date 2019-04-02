#include "flame.h"


/*
************************************************************
*	函数名称：	Fire_Init
*	函数功能：	配置火焰传感器用到的I/O口
*	入口参数：	无
*	返回参数：	无
*	说明：	PD5
************************************************************
*/
void Fire_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);
		
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;       
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//上拉输入 
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}
