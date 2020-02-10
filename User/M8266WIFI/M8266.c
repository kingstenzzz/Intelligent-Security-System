
//单片机头文件
#include "stm32f10x.h"

//网络设备驱动


//硬件驱动
#include "delay.h"
#include "./usart/bsp_usart.h"
#include "hwtimer.h"
#include "onenet.h"
#include "M8266WIFIDrv.h"
#include "M8266HostIf.h"
#include "M8266WIFI_ops.h"
#include "brd_cfg.h"
#include "./led/bsp_led.h"




//C库
#include <string.h>
#include <stdio.h>

u16 link_status;
u8 link_no;
#define  RECV_DATA_MAX_SIZE  1024

   u8  M8266buf[RECV_DATA_MAX_SIZE];
   u16 received = 0;
	 u32 total_received = 0;
   u32 MBytes = 0;



void M8266HostIf_Init(void)
{
	 M8266HostIf_GPIO_CS_RESET_Init();
	
	 M8266HostIf_SPI_Init();
//	 M8266HostIf_SPI_SetSpeed(SPI_BaudRatePrescaler_8);
	
}


void M8266_init()
{
	


	M8266HostIf_Init();
	while(!M8266WIFI_Module_Init_Via_SPI())
	{
		
		
		LED_RED;
		mDelay(500);
		
		LED_GREEN;
	
	}
	printf("wifi init\r\n");
	 while(M8266WIFI_SPI_Config_Tcp_Window_num(link_no, 4, &link_status)==0)
  {
		LED_RED;
		mDelay(500);
		LED_GREEN;	
	}
	printf("wifi config\r\n");

	while(M8266WIFI_SPI_Setup_Connection(TEST_CONNECTION_TYPE, 80, TEST_REMOTE_ADDR, TEST_REMOTE_PORT, link_no, 20, &link_status)==0)
	{		
		LED_RED;
		mDelay(500);
			printf("wifi connecting\r\n");
		LED_GREEN;	
	}

		
	
	
	
}

void M8266_SendData(unsigned char *data, unsigned short len)
{
	
	
	
}


u8 M8266_WaitRecive()
{
	if(M8266WIFI_SPI_Has_DataReceived())
			{
				//u16 M8266WIFI_SPI_RecvData(u8 data[], u16 max_len, uint16_t max_wait_in_ms, u8* link_no, u16* status);
	      received = M8266WIFI_SPI_RecvData(M8266buf, RECV_DATA_MAX_SIZE, 5*1000, &link_no, &link_status);

			  if(  (link_status&0xFF)!= 0 )  
			  {
				    if( (link_status&0xFF)==0x22 )      // 0x22 = Module buffer has no data received
				    {  
			         return 1;
				    }
				    else if( (link_status&0xFF)==0x23 )   
				    { 
							return 0;
							// (Chinese: 上次调用接收函数M8266WIFI_SPI_RecvData()时，并未收完整上次那个包，于是这次调用继续接受之前的包。可以在这里做一些工作，比如将一次接收缓冲区和做大长度上限加大。
				    }
				    else if(  (link_status&0xFF)==0x24)   
				    { 
							return 3;
	            // the packet is large in size than max_len specified and received only the max_len // TCP?????????
							// normally caused by the burst transmision by the routers after some block. 
							// Suggest to stop the TCP transmission for some time
					    // do some work here if necessary
							// (Chinese: 模组所接收到而正在被读取的这个包的长度，超过了这里的max_len参数所指定的长度。通常是因为远端阵发发送或路由器等阻塞时出现了大面积粘包导致到达模块的包过长，
							//           或者远端实际发送的就是一个长包，其长度超过了这里所指定的最大长度上限。如果是前者的原因，建议暂停远端TCP通信一段时间。如果是后者，建议加大max_len的数值或者
							//           不做任何处理，不做处理时，单片机侧这边接收到的长包会被拆成多个小包需要自行再次破解。
							//           必要时可以做相应的处理。
				    }
					
	
   }
					else
						{
							return 0;
					    printf("接受失败");
						}
				
				}
			return 0;
			}



unsigned char *M8266_GetIPD(unsigned short timeOut)
{

	char *ptrIPD = NULL;


		
		if(M8266_WaitRecive()==REV_OK)								//如果接收完成
		{
						return (unsigned char *)(M8266buf);
		
		}


}

