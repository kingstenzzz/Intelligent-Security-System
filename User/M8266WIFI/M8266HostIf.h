/********************************************************************
 * M8266HostIf.h
 * .Description
 *     header file of M8266WIFI Host Interface 
 * .Copyright(c) Anylinkin Technology 2015.5-
 *     IoT@anylinkin.com
 *     http://www.anylinkin.com
 *     http://anylinkin.taobao.com
 *  Author
 *     wzuo
 *  Date
 *  Version
 ********************************************************************/
#ifndef _M8266_HOST_IF_H_
#define _M8266_HOST_IF_H_
#include "stm32f10x.h"

	 #define TEST_M8266WIFI_TYPE    1	     //           1 = Repeative Sending, 2 = Repeative Reception, 3 = Echo  4 = multi-clients transimission test
	                                       // (Chinese: 1=模组向外不停地发送数据 2=模组不停地接收数据 3= 模组将接收到的数据发送给发送方 4=多客户端测试) 

	 /////////////
   ////	 Macros for Socket Connection Type (Chinese：套接字类型的宏定义) 
	 #define TEST_CONNECTION_TYPE   1
	 #define TEST_REMOTE_ADDR    	 	"183.230.40.39"  // "onenet"
   #define TEST_REMOTE_PORT  	    876						// 80
	 #define REV_OK		1	//接收完成标志
   #define REV_WAIT	0	//接收未完成标志
	 
	 enum net_status{Conneted=0,Closed,Lost,Error};



 
void M8266HostIf_Init(void);

void M8266HostIf_SPI_SetSpeed(u32 SPI_BaudRatePrescaler);

void M8266HostIf_usart_txd_data(char* data, u16 len);
void M8266HostIf_usart_txd_string(char* str);
void M8266HostIf_USART_Reset_RX_BUFFER(void);
u16 M8266HostIf_USART_RX_BUFFER_Data_count(void);
u16 M8266HostIf_USART_get_data(u8* data, u16 len);
void M8266_init(void);



unsigned char *M8266_GetIPD(unsigned short timeOut);
u8 M8266_WaitRecive(void);
void M8266_Clear(void);
unsigned char M8266_NetCheck(void);
void M8266_ReLink(u8 status );



#endif

