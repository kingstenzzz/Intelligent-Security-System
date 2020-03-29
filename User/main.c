
  
#include "stm32f10x.h"
#include "./ov7725/bsp_ov7725.h"
#include "./lcd/bsp_ili9341_lcd.h"
#include "./led/bsp_led.h"   
#include "./usart/bsp_usart.h"
#include "./key/bsp_key.h"  
#include "./systick/bsp_SysTick.h"
#include "ff.h"
#include "encode.h"
#include "malloc.h"

//网络协议层
#include "onenet.h"
#include "EdpKit.h"
//网络设备
#include "esp8266.h"
#include "M8266WIFIDrv.h"
#include "M8266HostIf.h"
#include "M8266WIFI_ops.h"
//硬件驱动
#include "delay.h"
#include "hwtimer.h"
#include "warning.h"
#include "flame.h"
#include "dht11.h"
#include "beep.h"
#include "iic.h"
#include "iwdg.h"
#include "light_moto.h"
#include "timer.h"

//rtos
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "semphr.h"
//C库
#include <string.h>
#include <stdio.h>
//#define heap_size_check 
enum net_status Net_status;
extern uint8_t Ov7725_vsync;
extern _Bool photo;
volatile data_Stream data_value;
u16 time_count=60;
u32 time_secend=0;

///////////////////
TaskHandle_t NetTask_Handler;
TaskHandle_t ReceiveCmdTask_Handler;
TaskHandle_t CheckSensor_Handler;

//////////////////任务要分配栈空间
#define Net_Task_Stack 1024//500+
#define ReceiveCmd_Stack 256//54
#define CheckSensor_Stack     512
/////////////////
#define Net_Task_Prioruty 6
#define ReceiveCMd_Priority 5
#define Display_Prioruty   2
#define CheckSensor_Priority     5
////////////////////////
static void ReceiveCmdTask(void *pvParameters);
static void CheckSensorTask(void *pvParameters);
static void Net_Task(void *pvParameters);


void Hardware_Init()
{
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//中断控制器分组设置
  //TIM4_PWM_Init();
  ILI9341_Init();
	
	USART_Config();  //调试串口
	
	//LED_GPIO_Config();
	OV7725_GPIO_Config();
	M8266_init();//SPI-wifi模块
	printf("wifi init \r\n");
	Fatfs_Init();  
	DHT11_Init();
	Fire_Init();	/*FAtFs挂载*/
	
	printf("Hardware init ok\r\n");
}


/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void) 	
{		
	Hardware_Init();
		
	
	/* 液晶初始化 */
	while(OneNet_DevLink())			//接入OneNET
	mDelay(500);
	Net_status=Conneted;
	//LED_BLUE;  
  Camera_Set_Test();
	CameraFous();	
	Camera_Set();
	ILI9341_DispStringLine_EN(LINE(0),"      Intelligent-Security-System");
	printf("start TASK\r\n");
	xTaskCreate((TaskFunction_t)ReceiveCmdTask,"ReceiveCmdTask",ReceiveCmd_Stack,"ReceiveCmdTask",ReceiveCMd_Priority,&ReceiveCmdTask_Handler);
	xTaskCreate((TaskFunction_t)Net_Task,"Net_Task",Net_Task_Stack,"Net_Task",Net_Task_Prioruty,&NetTask_Handler);
	xTaskCreate((TaskFunction_t)CheckSensorTask,"Check_Task",CheckSensor_Stack,"Check_Task",CheckSensor_Priority,&CheckSensor_Handler);

	//xTaskCreate((TaskFunction_t)DisplayTask,"Display",Display_Stack,"Display",Display_Prioruty,&Display_Handler);
	vTaskStartScheduler();

}

///////////////////////////////////////////////////////////////
//
//
//任务创建
///////////////////////////

//检查网络
void CheckNetWork_Task()
{			  
	Net_status=(enum net_status)M8266_NetCheck(); //检查wifi模块的网络状态


			if( Net_status!=Conneted)
			{
				M8266_ReLink(Net_status);		
			}
	
}


//处理下发的命令
static void ReceiveCmdTask(void *pvParameters) 
{
	while(1)
	{			
		if(OneNET_CmdHandle())
		{
		//	NET_Event_CallBack(NET_EVENT_Send_Data);//更新平台数据
		}
	
		vTaskDelay(400/portTICK_RATE_MS);
	}
}

static void CheckSensorTask(void *pvParameters) 
{
	while(1)
	{
		char display[10];
		taskENTER_CRITICAL(); //禁止中断
		time_secend++;		
		Check_sensor(&data_value);
		taskEXIT_CRITICAL();	
		
		if(Net_status==Conneted)
		ILI9341_DispStringLine_EN(LINE(3),"Online!");
		else if(Net_status==Closed)
		{
		ILI9341_DispStringLine_EN(LINE(3),"Check wifi!");
		}
		else if(Net_status==Lost)
		{
		ILI9341_DispStringLine_EN(LINE(3),"Check service!");

		}
		sprintf(display,"hour:%d:min:%d:sec:%d",time_secend/3600,(time_secend/60)%60,time_secend%60);
		ILI9341_DispStringLine_EN(LINE(2),display);	
		sprintf(display,"HUM:%d",data_value.humidit);
		ILI9341_DispStringLine_EN(LINE(4),display);	
		sprintf(display,"TMP:%d",data_value.temp);
		ILI9341_DispStringLine_EN(LINE(5),display);
		#ifdef heap_size_check
		printf(" sensor the min free stack size is %d \r\n",(int32_t)uxTaskGetStackHighWaterMark(NULL));
		printf(" hole free stack size is %d \r\n",(int32_t)xPortGetMinimumEverFreeHeapSize());
		#endif
		vTaskDelay(1000/portTICK_RATE_MS);
	}
}



static void DisplayTask(void *pvParameters) 
{
	while(1)
	{
		
		vTaskDelay(1000/portTICK_RATE_MS);
	}
		
}


static void Net_Task(void *pvParameters)
{
	unsigned short data_count=0,photo_count=0,check_count=0;
	
	while(1)
	{
		
#ifdef heap_size_check
		printf(" NET the min free stack size is %d \r\n",(int32_t)uxTaskGetStackHighWaterMark(NULL));
#endif
		if(Net_status==Conneted)
		{

		if(++data_count>=12)   //60s发送一次数据
		{
			data_count=0;
			printf("send data");
			OneNet_SendData(&data_value);
		}
	
		else if(photo==1||photo_count++>=time_count)
		{
			photo_count=0;
			taskENTER_CRITICAL(); //禁止中断
			OneNet_SendPhoto();
			photo=0;
			taskEXIT_CRITICAL();			
		}
		else if(++check_count>=20)
		{			
			check_count=0;
			CheckNetWork_Task();

		}
		
  }
	else
	{
		M8266_ReLink(Net_status);
		
		
	}
	vTaskDelay(1000/portTICK_RATE_MS);
	
}
}



	




/*********************************************END OF FILE**********************/

