#include "timer.h"
#include "stm32f10x.h"
#include "delay.h"

void GPIO(void)
{
	//不采用复用功能，会把引脚变换
	
	  GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	  GPIO_InitStructure.GPIO_Pin=(GPIO_Pin_8); 
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	
		GPIO_Init(GPIOB,&GPIO_InitStructure);		

}

//arr的作用就是计数周期，相当于CNT的值
//psc就是计数一次的时间
//如输出一个周期为20ms的方波则arr为199，psc为7199（计数一次0.1ms）
void TIM4_PWM_Init()
	{
	  
		GPIO_InitTypeDef GPIO_InitStructure;   //声明一个结构体变量，用来初始化GPIO

	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;//声明一个结构体变量，用来初始化定时器

	TIM_OCInitTypeDef TIM_OCInitStructure;//根据TIM_OCInitStruct中指定的参数初始化外设TIMx
	/* 开启时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

	/*  配置GPIO的模式和IO口 */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5;// PC6
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;//复用推挽输出
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	//TIM4定时器初始化
	TIM_TimeBaseInitStructure.TIM_Period = 199; //PWM 频率=72000/(199+1)=36Khz//设置自动重装载寄存器周期的值
	TIM_TimeBaseInitStructure.TIM_Prescaler = 7199;//设置用来作为TIMx时钟频率预分频值
	TIM_TimeBaseInitStructure.TIM_ClockDivision = 0;//设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	//TIM向上计数模式
	TIM_TimeBaseInit(TIM3, & TIM_TimeBaseInitStructure);

  GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3,ENABLE);//改变指定管脚的映射	//pC6

	//PWM初始化	  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;//PWM输出使能
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_Low;

	TIM_OC2Init(TIM3,&TIM_OCInitStructure);
	//注意此处初始化时TIM_OC1Init而不是TIM_OCInit，否则会出错。因为固件库的版本不一样。
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);//使能或者失能TIMx在CCR1上的预装载寄存器
	TIM_Cmd(TIM3,ENABLE);//使能或者失能TIMx外设
	 TIM_SetCompare2(TIM3, 195);
	
	mDelay(500);
	TIM_SetCompare2(TIM3, 175);
			 TIM_SetCompare2(TIM3, 195);
	
	mDelay(500);
	TIM_SetCompare2(TIM3, 175);

	
	}
	
	u16 Get_angle(int jiaodu)//返回值类型一定要是u16     因为arr的值大于一个字节
	{
		 u16 zkb=0;
		//0.5/45=time/0.18   ==>   time=0.002ms  ==>每转动0.18角度走时0.002ms=2us
//    zkb=jiaodu*14+250;//错误的
		if(jiaodu>0)
		zkb=185+jiaodu*0.11;//250个2us+（角度/精度）=占空比
		else
			{
		jiaodu=-jiaodu;
		zkb=185-jiaodu*0.11;
			}
		return zkb;  
	} 	
	
	
	void Set_angle(_Bool dir,u8 angle)
 {
	static u8 pre_angel=0;
	 if(dir)
	 {
		 pre_angel=pre_angel+angle;
		 TIM_SetCompare3(TIM3, Get_angle(pre_angel));
		 
	 }
	  else
	 {
		 		 pre_angel=pre_angel-angle;

		 TIM_SetCompare3(TIM3, Get_angle(pre_angel));
		 
	 }
	 
 }
	
	

	
	
	
	

	