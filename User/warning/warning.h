#ifndef __WARNING_H_
#define __WARNING_H_

#include "stm32f10x.h"
#include "./led/bsp_led.h"   
#include "beep.h"
#include "dht11.h"
#include "adc.h"
#include "flame.h"
#include "./usart/bsp_usart.h"
#include "onenet.h"
#include <stdlib.h>
#include <string.h>



void WARNING(uint8_t flag);		//MQ135、DHT11、LIGHT接受数据后对LED灯和蜂鸣器进行处理控制
void DHT11_Check(DHT11_Data_TypeDef *DHT11_Data, int16_t temp_max, int16_t temp_min, uint16_t humi_max, uint16_t humi_min);
void Fire_Check(u8 *fire);
void Check_sensor(volatile data_Stream *data_stram);
#endif
