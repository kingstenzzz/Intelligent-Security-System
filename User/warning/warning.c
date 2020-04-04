#include "warning.h"
#include "hwtimer.h"
#include "beep.h"
#include "iic.h"



enum 
{
	Safe=0,
	Fire,
	Hot,
	Cold,
	Wet,
	Dry
}alarm_mode;

extern int16_t temp_max;
extern int16_t temp_min;
extern u8 humi_max;
extern u8 humi_min;
extern _Bool fire;


//读取数据对LED灯和蜂鸣器进行控制
void WARNING(uint8_t flag)
{
	switch (flag)
	{
		case Safe:
			LED_RGBOFF		//安全模式	
		break;				
		
		case Fire:                     
			Beep_Set(BEEP_ON);
		break;						
		case Hot:	
			LED_RED		//高温模式，红灯亮	
		break;
		case Cold:	
			LED_BLUE	//低温模式，蓝灯亮	
		break;		
		case Wet:
			LED_CYAN;
		break;
		case Dry:
			LED_PURPLE;
		break;
		default: break;
	}
}


/*
************************************************************
*	函数名称：	DHT11_Check
*	函数功能：	温湿度越限检测
*	入口参数：	无
*	返回参数：	无
*	说明：		
************************************************************
*/
void DHT11_Check(DHT11_Data_TypeDef *DHT11_Data, int16_t temp_max, int16_t temp_min, uint16_t humi_max, uint16_t humi_min)
{
	//温度越限检测
	if(DHT11_Data->temp_int >= temp_max)		//当前温度大于等于最高温度时,高温警告
	{
		alarm_mode=Hot;
		WARNING(Hot);
		
	}			
	else if(DHT11_Data->temp_int <= temp_min)	//当前温度小于等于最低温度时,低温警告
	{          
		alarm_mode=Cold;
		WARNING(Cold);

	}
	else										//安全
	{             
		alarm_mode=Safe;
		WARNING(Safe);

    }
	
	//湿度越限检测
	if(DHT11_Data->humi_int >= humi_max)		//当前温度大于等于最高温度时,高温警告
	{
             
   alarm_mode=Wet;
		WARNING(Wet);
	}			
	else if(DHT11_Data->humi_int <= humi_min)	//当前温度小于等于最低温度时,低温警告
	{
		alarm_mode=Dry;
		WARNING(Dry);
	}

}






/*
************************************************************
*	函数名称：	Fire_Check
*	函数功能：	火焰检测报警功能
*	入口参数：	fire：火焰标志
*	返回参数：	无
*	说明：		 
************************************************************
*/
void Fire_Check(u8 *fire)
{
	if(FIRE_Data_IN() == 1)								//当输入信号为0时，有火焰
	{		
		UsartPrintf(USART_DEBUG, "火焰警报\r\n");
		*fire = 1;			
		alarm_mode=Fire;
		WARNING(Fire);
		mDelay(500);
	}
	else if(FIRE_Data_IN() == 0)						//当输入信号为1时，无火焰
	{
		*fire = 0;
		Beep_Set(BEEP_OFF);
	}
}



void Check_sensor(volatile data_Stream *data_stram)
{
	Fire_Check(&fire);
	DHT11_Data_TypeDef DH11_data;
	if(DHT11_Read_TempAndHumidity(&DH11_data))
	{
		DHT11_Check(&DH11_data,temp_max,temp_min,humi_max,humi_min);
			UsartPrintf(USART_DEBUG, "--------------监测数据--------------\r\n");			
			UsartPrintf(USART_DEBUG, "温度：%d ℃\r\n", DH11_data.temp_int);
			UsartPrintf(USART_DEBUG, "湿度：%d ％RH\r\n", DH11_data.humi_int);
			UsartPrintf(USART_DEBUG, "最高温度：%d\r\n", temp_max);
			UsartPrintf(USART_DEBUG, "最低温度：%d\r\n", temp_min);
			UsartPrintf(USART_DEBUG, "最大湿度：%d\r\n", humi_max);
			UsartPrintf(USART_DEBUG, "最小湿度：%d\r\n", humi_min);	
		  data_stram->humidit=DH11_data.humi_int;
	    data_stram->temp=DH11_data.temp_int;
	}
			printf("火情%d",fire);
			data_stram->fire=fire;
	    
			if(data_stram->tem_max!=temp_max)
			{
					data_stram->tem_max=temp_max;
					ee_WriteBytes((u8)temp_max , 0x02);

			}
			else if(data_stram->tem_min!=temp_min)
				{
						data_stram->tem_min=temp_min;
				ee_WriteBytes((u8)temp_min, 0x03);
					
				}
			else if(data_stram->hum_max!=humi_max)
				{
							data_stram->hum_max=humi_max;
							ee_WriteBytes((u8)humi_max , 0x00);
				}
				else if (data_stram->hum_min!=humi_min)
			{
	
			data_stram->hum_min=humi_min;
			ee_WriteBytes((u8)humi_min, 0x01);
		
		
			}
	
	
	
}


