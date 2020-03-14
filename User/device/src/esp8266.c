

//单片机头文件
#include "stm32f10x.h"

//网络设备驱动
#include "esp8266.h"

//硬件驱动
#include "delay.h"
#include "./usart/bsp_usart.h"
#include "hwtimer.h"
#include "onenet.h"

//C库
#include <string.h>
#include <stdio.h>

#ifdef ESP8266

#define ESP8266_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"192.168.1.102\",876\r\n"



unsigned char esp8266_buf[1024];
unsigned short esp8266_cnt = 0, esp8266_cntPre = 0;


void  SendCmd(char* cmd, char* result,int timeout)
{
	while(1)
	{
		
	 Usart_SendString(USART3, (uint8_t*)cmd, strlen(cmd));
		mDelay(timeout);
	//	UsartPrintf(USART1,"%s %d cmd:%s,rsp:%s\n", __func__, __LINE__, cmd, esp8266_buf);
		 if((NULL != strstr((const char *)esp8266_buf, (const char *)result)))	//判断是否有预期的结果
        {
            break;
        }
        else
        {
            mDelay(100);
						UsartPrintf(USART1,( char*)esp8266_buf);
        }
	}
}


//==========================================================
//	函数名称：	ESP8266_Clear
//
//	函数功能：	清空缓存
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void ESP8266_Clear(void)
{

	memset(esp8266_buf, 0, sizeof(esp8266_buf));
	esp8266_cnt = 0;

}

//==========================================================
//	函数名称：	ESP8266_WaitRecive
//
//	函数功能：	等待接收完成
//
//	入口参数：	无
//
//	返回参数：	REV_OK-接收完成		REV_WAIT-接收超时未完成
//
//	说明：		循环调用检测是否接收完成
//==========================================================
_Bool ESP8266_WaitRecive(void)
{

	if(esp8266_cnt == 0) 							//如果接收计数为0 则说明没有处于接收数据中，所以直接跳出，结束函数
		return REV_WAIT;
		
	if(esp8266_cnt == esp8266_cntPre)				//如果上一次的值和这次相同，则说明接收完毕
	{
		esp8266_cnt = 0;							//清0接收计数
			
		return REV_OK;								//返回接收完成标志
	}		
	esp8266_cntPre = esp8266_cnt;					//置为相同
	
	return REV_WAIT;								//返回接收未完成标志

}


//==========================================================
//	函数名称：	ESP8266_SendCmd
//
//	函数功能：	发送命令
//
//	入口参数：	cmd：命令
//				res：需要检查的返回指令
//
//	返回参数：	0-成功	1-失败
//
//	说明：		
//==========================================================
_Bool ESP8266_SendCmd(char *cmd, char *res)
{
	
	
	
	unsigned char timeOut = 200;

	Usart_SendString(USART3, (unsigned char *)cmd, strlen((const char *)cmd));
	
	while(timeOut--)
	{
	
		if(ESP8266_WaitRecive() == REV_OK)							//如果收到数据
		{
				//UsartPrintf(USART1,"%s %d cmd:%s,rsp:%s\n", __func__, __LINE__, cmd, esp8266_buf);
			
			if(strstr((const char *)esp8266_buf, res) != NULL)		//如果检索到关键词
			{
		//	ESP8266_Clear();									//清空缓存
				
				return 0;
			}
			printf((const char *)esp8266_buf);
	}
		
		mDelay(10);
	}
	
	return 1;

}

//==========================================================
//	函数名称：	ESP8266_SendData
//
//	函数功能：	发送数据
//
//	入口参数：	data：数据
//				len：长度
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void ESP8266_SendData(unsigned char *data, unsigned short len)
{

	char cmdBuf[32];
	
	ESP8266_Clear();								//清空接收缓存
	sprintf(cmdBuf, "AT+CIPSEND=%d\r\n", len);		//发送命令
	if(!ESP8266_SendCmd(cmdBuf, ">"))				//收到‘>’时可以发送数据
	{
		Usart_SendString(USART3, data, len);		//发送设备连接请求数据
	}
//	printf((const char *)esp8266_buf);
	

}

//==========================================================
//	函数名称：	ESP8266_GetIPD
//
//	函数功能：	获取平台返回的数据
//
//	入口参数：	等待的时间(乘以10ms)
//
//	返回参数：	平台返回的原始数据
//
//	说明：		不同网络设备返回的格式不同，需要去调试
//				如ESP8266的返回格式为	"+IPD,x:yyy"	x代表数据长度，yyy是数据内容
//==========================================================
unsigned char *ESP8266_GetIPD(unsigned short timeOut)
{

	char *ptrIPD = NULL;
	while(timeOut--)
	
	{
		ESP8266_WaitRecive();
		while(ESP8266_WaitRecive()==REV_OK)								//如果接收完成
		{
			ptrIPD = strstr((char *)esp8266_buf, "IPD");				//搜索“IPD”头
			if(ptrIPD == NULL)											//如果没找到，可能是IPD头的延迟，还是需要等待一会，但不会超过设定的时间
			{
				//UsartPrintf(USART_DEBUG, "\"IPD\" not found\r\n");
			}
			else
			{
				ptrIPD = strchr(ptrIPD, ':');							//找到':'
				if(ptrIPD != NULL)
				{
					ptrIPD++;
					return (unsigned char *)(ptrIPD);
					
				}
				else
					return NULL;
				
			}
		}
		
		mDelay(5);													//延时等待
	} 
	
	return NULL;														//超时还未找到，返回空指针

}

//==========================================================
//	函数名称：	ESP8266_Init
//
//	函数功能：	初始化ESP8266
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void ESP8266_Init(void)
{
	
	GPIO_InitTypeDef GPIO_Initure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

	//ESP8266复位引脚
	/*
	GPIO_Initure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Initure.GPIO_Pin = GPIO_Pin_0;					//GPIOA0-复位
	GPIO_Initure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_Initure);
	
	GPIO_WriteBit(GPIOD, GPIO_Pin_0, Bit_RESET);
	mDelay(250);
	GPIO_WriteBit(GPIOD, GPIO_Pin_0, Bit_SET);
	mDelay(600);
	*/
	ESP8266_Clear();

	UsartPrintf(USART_DEBUG, "1. AT\r\n");
	while(ESP8266_SendCmd(AT, "OK"))
		mDelay(500);
	
	
	
	UsartPrintf(USART_DEBUG, "2. CWMODE\r\n");
	while(ESP8266_SendCmd(CWMODE, "OK"))
		mDelay(500);
	
	UsartPrintf(USART_DEBUG, "1. AT\r\n");
	while(ESP8266_SendCmd(RST, "OK"))
		mDelay(500);
	
	UsartPrintf(USART_DEBUG, "3. CWJAP\r\n");
	while(ESP8266_SendCmd(CWJAP, "OK"))
		mDelay(500);
		UsartPrintf(USART_DEBUG, "3. CIPMODE\r\n");
	while(ESP8266_SendCmd(CIPMODE0, "OK"))
		mDelay(500);
	
	UsartPrintf(USART_DEBUG, "4. CIPSTART\r\n");
	while(ESP8266_SendCmd(CIPSTART, "CONNECT"))
		mDelay(500);
	
	UsartPrintf(USART_DEBUG, "5. ESP8266 Init OK\r\n");

}

//==========================================================
//	函数名称：	USART3_IRQHandler
//
//	函数功能：	串口2收发中断
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void USART3_IRQHandler(void)
{

	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) //接收中断
	{
		if(esp8266_cnt >= sizeof(esp8266_buf))	
			esp8266_cnt = 0; //防止串口被刷爆
		esp8266_buf[esp8266_cnt++] = USART3->DR;
		USART_ClearFlag(USART3, USART_FLAG_RXNE);
		
	}

}


void ESP8266_EnterTrans(void)
{
	printf("进入透传\r\n");

	ESP8266_SendCmd(CIPMODE1, "OK");	//配置透传模式
	ESP8266_SendCmd(CIPSEND, "OK" );	//配置透传模式
	ESP8266_SendCmd("AT+CIPSEND\r\n", ">");	//发送数据
						//等待100ms

}

void ESP8266_QuitTrans(void)
{
    SendCmd(QUITTRANS,"+++",100);			//等待100ms
printf("退出透传\r\n");

}


u8 Check_Wifi(void )
{
		ESP8266_SendCmd(CIPSTATUS,"STATUS");
	if(strstr((const char *)esp8266_buf,"STATUS:3"))
	{
		printf("wifi is  normal\n");
		return 0;				
	}
	else if(strstr((const char *)esp8266_buf,"STATUS:4")) //失去连接  2
	{
		printf("tcp link lost\n");
		return 2;
		
	}	
	else if(strstr((const char *)esp8266_buf,"STATUS:5"))
	{
		printf("wifi  is unusable\n");
		return 1;		
	}
	else
	{
		return 3;
	}
	
}


void reLink(void )
{
	UsartPrintf(USART_DEBUG, "4. CIPSTART\r\n");
	while(ESP8266_SendCmd(CIPSTART, "CONNECT"))
		mDelay(500);
   OneNet_DevLink();
}

#endif


