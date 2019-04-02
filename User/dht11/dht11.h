#ifndef __DHT11_H
#define	__DHT11_H


#include "stm32f10x.h"


/************************** DHT11 数据类型定义********************************/
typedef struct
{
	uint8_t humi_int;		//湿度的整数部分
	uint8_t humi_deci;   //湿度的小数部分
	uint8_t temp_int;    //温度的整数部分
	uint8_t temp_deci;   //温度的小数部分
	uint8_t check_sum;   //校验和
}DHT11_Data_TypeDef;


/************************** DHT11 函数宏定义********************************/
#define DHT11_Dout_0	GPIO_ResetBits(GPIOE, GPIO_Pin_6) 
#define DHT11_Dout_1	GPIO_SetBits(GPIOE, GPIO_Pin_6) 

#define DHT11_Dout_IN	GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_6) 


/************************** DHT11 函数声明 ********************************/
void DHT11_Init(void);
uint8_t DHT11_Read_TempAndHumidity(DHT11_Data_TypeDef *DHT11_Data);
void DHT11_GPIO_Config(void);
void DHT11_Mode_IPU(void);
void DHT11_Mode_Out_PP(void);
uint8_t DHT11_ReadByte(void);

#endif
