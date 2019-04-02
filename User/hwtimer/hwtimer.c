
#include "hwtimer.h"

#ifndef _TIMUSE_



#else
void Tim2_init(void)
{
RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);  
 NVIC_con();
 TIM2_con();
}


void NVIC_con(void)
{
	NVIC_InitTypeDef NVIC_config;
	NVIC_config.NVIC_IRQChannel=TIM2_IRQn;
	NVIC_config.NVIC_IRQChannelPreemptionPriority=3;
	NVIC_config.NVIC_IRQChannelSubPriority=0;
	NVIC_config.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_config);

	
}
void gpio_Con(void)
{
	GPIO_InitTypeDef GPIO_config;
	GPIO_config.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_config.GPIO_Pin=GPIO_Pin_7;
	GPIO_config.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_config);
	
}

void TIM2_con(void)
{
	TIM_TimeBaseInitTypeDef TIM2_config;
	TIM_DeInit(TIM2);
	TIM2_config.TIM_Prescaler=35999;
	TIM2_config.TIM_Period=19;
	TIM2_config.TIM_CounterMode=TIM_CounterMode_Up;
	TIM2_config.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM2,&TIM2_config);
	TIM_ClearFlag(TIM2,TIM_FLAG_Update);
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
}



void uDelay(uint32_t time)
{
	u32 i=8*time;
	while(i--);
	
	
}


#endif



