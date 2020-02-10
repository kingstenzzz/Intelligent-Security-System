#include "light_moto.h"



unsigned char  S_Rotation[4]={0x01,0x02,0x04,0x08};//正转表格
unsigned char  N_Rotation[4]={0x01,0x08,0x04,0x02};//正转表格


void  Shelter_GPIOEonfig()
{
	 GPIO_InitTypeDef GPIO_InitStructure; 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    GPIO_InitStructure.GPIO_Pin =Pin_1|Pin_2|Pin_3|Pin_4; //电机驱动ioP0.1.2.3
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //输入
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure); 
	
}  
//////////通过输出电平控制步进电机

void Shadding()
{
	unsigned  char i=128;//90度
	 while(i--)
	 {
     GPIO_SetBits(GPIOE,Pin_1);//依次高电平输出	
		 GPIO_ResetBits(GPIOE,Pin_2);	
		 GPIO_ResetBits(GPIOE,Pin_3);	
		 GPIO_ResetBits(GPIOE,Pin_4);	
		 mDelay(50);		
		 
		 GPIO_ResetBits(GPIOE,Pin_1);	
		 GPIO_SetBits(GPIOE,Pin_2);	
		 GPIO_ResetBits(GPIOE,Pin_3);	
		 GPIO_ResetBits(GPIOE,Pin_4);	
		 
		
		 mDelay(50);		
		 GPIO_ResetBits(GPIOE,Pin_1);	
		 GPIO_ResetBits(GPIOE,Pin_2);	
		 GPIO_SetBits(GPIOE,Pin_3);	
		 GPIO_ResetBits(GPIOE,Pin_4);	
		 mDelay(50);		
		 GPIO_ResetBits(GPIOE,Pin_1);	
		 GPIO_ResetBits(GPIOE,Pin_2);	
		 GPIO_ResetBits(GPIOE,Pin_3);	
		 GPIO_SetBits(GPIOE,Pin_4);		
		 mDelay(50); 
 }

	
}



void  UnShadding()
{
	unsigned  char i=128;//90度
	 while(i--)
	 {
	
	   GPIO_SetBits(GPIOE,Pin_1);//依次高电平输出	
		 GPIO_ResetBits(GPIOE,Pin_2);	
		 GPIO_ResetBits(GPIOE,Pin_3);	
		 GPIO_ResetBits(GPIOE,Pin_4);	
		 mDelay(50);		
		  GPIO_ResetBits(GPIOE,Pin_1);	
		 GPIO_ResetBits(GPIOE,Pin_2);	
		 GPIO_ResetBits(GPIOE,Pin_3);	
		 GPIO_SetBits(GPIOE,Pin_4);	
		 
			
		 mDelay(50);		
		 GPIO_ResetBits(GPIOE,Pin_1);	
		 GPIO_ResetBits(GPIOE,Pin_2);	
		 GPIO_SetBits(GPIOE,Pin_3);	
		 GPIO_ResetBits(GPIOE,Pin_4);	
		 mDelay(50);		
		 GPIO_ResetBits(GPIOE,Pin_1);	
		 GPIO_SetBits(GPIOE,Pin_2);	
		 GPIO_ResetBits(GPIOE,Pin_3);	
		 GPIO_ResetBits(GPIOE,Pin_4);
		 mDelay(50);
	 }
	
}


void  Rotating_Step_S()
{
	//1100
	   GPIO_SetBits(GPIOE,Pin_1);//依次高电平输出	
		 GPIO_SetBits(GPIOE,Pin_2);	
		 GPIO_ResetBits(GPIOE,Pin_3);	
		 GPIO_ResetBits(GPIOE,Pin_4);	
		 mDelay(50);		
	//0110
		 GPIO_ResetBits(GPIOE,Pin_1);	
		 GPIO_SetBits(GPIOE,Pin_2);	
		 GPIO_SetBits(GPIOE,Pin_3);	
		 GPIO_ResetBits(GPIOE,Pin_4);	
		 
			//0011
		 mDelay(50);		
		 GPIO_ResetBits(GPIOE,Pin_1);	
		 GPIO_ResetBits(GPIOE,Pin_2);	
		 GPIO_SetBits(GPIOE,Pin_3);	
		 GPIO_SetBits(GPIOE,Pin_4);	
		 mDelay(50);	
	//1001
		 GPIO_SetBits(GPIOE,Pin_1);	
		 GPIO_ResetBits(GPIOE,Pin_2);	
		 GPIO_ResetBits(GPIOE,Pin_3);	
		 GPIO_SetBits(GPIOE,Pin_4);
		 mDelay(50);
	
	
}

void  Rotating_Step_N()
{
	//1100
	   GPIO_SetBits(GPIOE,GPIO_Pin_12);//依次高电平输出	
		 GPIO_SetBits(GPIOE,GPIO_Pin_13);	
		 GPIO_ResetBits(GPIOE,GPIO_Pin_14);	
		 GPIO_ResetBits(GPIOE,GPIO_Pin_15);	
		 mDelay(50);		
	//1001
	 GPIO_SetBits(GPIOE,GPIO_Pin_12);	
		 GPIO_ResetBits(GPIOE,GPIO_Pin_13);	
		 GPIO_ResetBits(GPIOE,GPIO_Pin_14);	
		 GPIO_SetBits(GPIOE,GPIO_Pin_15);
		 
		 
			//0011
		 mDelay(50);		
		 GPIO_ResetBits(GPIOE,GPIO_Pin_12);	
		 GPIO_ResetBits(GPIOE,GPIO_Pin_13);	
		 GPIO_SetBits(GPIOE,GPIO_Pin_14);	
		 GPIO_SetBits(GPIOE,GPIO_Pin_15);	
		 mDelay(50);	
	//0110
	
     GPIO_ResetBits(GPIOE,GPIO_Pin_12);	
		 GPIO_SetBits(GPIOE,GPIO_Pin_13);	
		 GPIO_SetBits(GPIOE,GPIO_Pin_14);	
		 GPIO_ResetBits(GPIOE,GPIO_Pin_15);	
		 
		 
		 mDelay(50);
	
	
}
#define angle_step 0.7
void Rotating(u16 angle,u8 dir)
{
	float num;
	num=angle/angle_step;
	while((u16)num--)
	{
		Rotating_Step_S();
	}
	
	
}

