

//单片机头文件


//网络设备
#include "esp8266.h"

#include "M8266WIFIDrv.h"
#include "M8266HostIf.h"
#include "M8266WIFI_ops.h"

//协议文件
#include "onenet.h"
#include "edpkit.h"
#include "ff.h"
#include "encode.h"



//硬件驱动
#include "./usart/bsp_usart.h"
#include "delay.h"
#include "iic.h"
#include "timer.h"
#include "./key/bsp_key.h"  



//C库
#include <string.h>
#include <stdio.h>

//////////cmd和main任务未好




extern unsigned char esp8266_buf[1024];
_Bool  heart_flag;
extern enum data_type Data_type;
_Bool photo=0;
extern   u16 time_count;
extern u16 link_status;
extern u8 link_no;
unsigned char camera_buf[1000];
FIL bmpfsrc; 
FRESULT bmpres;

 int16_t temp_max=100;
 int16_t temp_min=10;
 u8 humi_max=100;
 u8 humi_min=0;
 _Bool fire;
 extern uint8_t Ov7725_vsync;




//==========================================================
//	函数名称：	OneNet_DevLink
//
//	函数功能：	与onenet创建连接
//
//	入口参数：	无
//
//	返回参数：	1-成功	0-失败
//
//	说明：		与onenet平台建立连接
//==========================================================
_Bool OneNet_DevLink(void)
{
	
	EDP_PACKET_STRUCTURE edpPacket = {NULL, 0, 0, 0};				//协议包

	unsigned char *dataPtr;
	
	unsigned char status = 1;
	
	UsartPrintf(USART_DEBUG, "OneNet_DevLink\r\n"
                        "DEVID: %s,     APIKEY: %s\r\n"
                        , DEVID, APIKEY);

	if(EDP_PacketConnect1(DEVID, APIKEY, 256, &edpPacket) == 0)		//根据devid 和 apikey封装协议包
	{
		//ESP8266_SendData(edpPacket._data, edpPacket._len);			//上传平台
		M8266WIFI_SPI_Send_Data(edpPacket._data, edpPacket._len,link_no,&link_status);
	
		dataPtr = M8266_GetIPD(250);								//等待平台响应
		if(dataPtr != NULL)
		{
			if(EDP_UnPacketRecv(dataPtr) == CONNRESP)
			{
				switch(EDP_UnPacketConnectRsp(dataPtr))
				{
					case 0:UsartPrintf(USART_DEBUG, "Tips:	连接成功\r\n");status = 0;break;
					
					case 1:UsartPrintf(USART_DEBUG, "WARN:	连接失败：协议错误\r\n");break;
					case 2:UsartPrintf(USART_DEBUG, "WARN:	连接失败：设备ID鉴权失败\r\n");break;
					case 3:UsartPrintf(USART_DEBUG, "WARN:	连接失败：服务器失败\r\n");break;
					case 4:UsartPrintf(USART_DEBUG, "WARN:	连接失败：用户ID鉴权失败\r\n");break;
					case 5:UsartPrintf(USART_DEBUG, "WARN:	连接失败：未授权\r\n");break;
					case 6:UsartPrintf(USART_DEBUG, "WARN:	连接失败：授权码无效\r\n");break;
					case 7:UsartPrintf(USART_DEBUG, "WARN:	连接失败：激活码未分配\r\n");break;
					case 8:UsartPrintf(USART_DEBUG, "WARN:	连接失败：该设备已被激活\r\n");break;
					case 9:UsartPrintf(USART_DEBUG, "WARN:	连接失败：重复发送连接请求包\r\n");break;
					
					default:UsartPrintf(USART_DEBUG, "ERR:	连接失败：未知错误\r\n");break;
				}
			}
		}
		
		EDP_DeleteBuffer(&edpPacket);								//删包
	}
	else
		UsartPrintf(USART_DEBUG, "WARN:	EDP_PacketConnect Failed\r\n");
	
	return status;
	
}

unsigned char OneNet_FillBuf(char *buf,data_Stream *data_stream)
{
	char text[16];
	memset(text, 0, sizeof(text));
	strcpy(buf, "{");
	memset(text, 0, sizeof(text));
	sprintf(text,  "\"Fire\":%d,",data_stream->fire);
	strcat(buf, text);
	memset(text, 0, sizeof(text));
	sprintf(text,  "\"Temperature\":%d,",data_stream->temp);
	strcat(buf, text);
	memset(text, 0, sizeof(text));
	sprintf(text,  "\"Humidity\":%d,",data_stream->humidit);
	strcat(buf, text);
	memset(text, 0, sizeof(text));
	sprintf(text,  "\"Temp_Max\":%d,",data_stream->tem_max);
	strcat(buf, text);
	memset(text, 0, sizeof(text));
	sprintf(text,  "\"Temp_Min\":%d,",data_stream->tem_min);
	strcat(buf, text);
	memset(text, 0, sizeof(text));
	sprintf(text,  "\"Hum_Max\":%d,",data_stream->hum_max);///////注意逗号
	strcat(buf, text);
	memset(text, 0, sizeof(text));
	sprintf(text,  "\"Hum_Min\":%d",data_stream->hum_min);
	strcat(buf, text);
	strcat(buf, "}");	
	return strlen(buf);
}

//==========================================================
//	函数名称：	OneNet_SendData
//
//	函数功能：	上传数据到平台
//
//	入口参数：	type：发送数据的格式
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void OneNet_SendData(volatile data_Stream *data_stream)
{
	
	EDP_PACKET_STRUCTURE edpPacket = {NULL, 0, 0, 0};												//协议包
	
	char buf[128];
	
	short int body_len = 0, i = 0;
		
	memset(buf, 0, sizeof(buf));
	
	body_len = OneNet_FillBuf(buf,data_stream);		//获取当前需要发送的数据流的总长度
	
	if(body_len)
	{
		if(EDP_PacketSaveData(DEVID, body_len, NULL, kTypeSimpleJsonWithoutTime, &edpPacket) == 0)	//封包
		{
			for(; i < body_len; i++)
				edpPacket._data[edpPacket._len++] = buf[i];
			
		M8266WIFI_SPI_Send_Data(edpPacket._data, edpPacket._len,link_no,&link_status);
										//上传数据到平台
			EDP_DeleteBuffer(&edpPacket);
		}
		else
			UsartPrintf(USART_DEBUG, "WARN:	EDP_NewBuffer Failed\r\n");
	}
	//ESP8266_Clear();
	
}

//==========================================================
//	函数名称：	OneNet_RevPro
//
//	函数功能：	平台返回数据检测
//
//	入口参数：	dataPtr：平台返回的数据
//
//	返回参数：	无
//
//	说明：		
//==========================================================



void OneNet_RevPro(unsigned char *cmd)
{
	
	EDP_PACKET_STRUCTURE edpPacket = {NULL, 0, 0, 0};	//协议包
	
	char *cmdid_devid = NULL;
	unsigned short cmdid_len = 0;
	char *req = NULL;
	unsigned int req_len = 0;
	unsigned char type = 0;
	
	short result = 0;

	char *dataPtr = NULL;
	char numBuf[10];
	int num = 0;
	heart_flag=Heart_err;//先初始化未丢失
	hexdump(edpPacket._data,edpPacket._len);//打印收到的信息
	type = EDP_UnPacketRecv(cmd);
	switch(type)										//判断是pushdata还是命令下发
	{
			case PUSHDATA:									//解pushdata包
			
			result = EDP_UnPacketPushData(cmd, &cmdid_devid, &req, &req_len);
		
			if(result == 0)
				UsartPrintf(USART_DEBUG, "src_devid: %s, req: %s, req_len: %d\r\n", cmdid_devid, req, req_len);
			
		break;
		case CMDREQ:									//解命令包
			
			result = EDP_UnPacketCmd(cmd, &cmdid_devid, &cmdid_len, &req, &req_len);
			
			if(result == 0)								//解包成功，则进行命令回复的组包
			{
				EDP_PacketCmdResp(cmdid_devid, cmdid_len, req, req_len, &edpPacket);
				UsartPrintf(USART_DEBUG, "cmdid: %s, req: %s, req_len: %d\r\n", cmdid_devid, req, req_len);
			}
			
		break;
			
		case SAVEACK:
			
			if(cmd[3] == MSG_ID_HIGH && cmd[4] == MSG_ID_LOW)
			{
				//UsartPrintf(USART_DEBUG, "Tips:	Send %s\r\n", cmd[5] ? "Err" : "Ok");
			}
			else
				UsartPrintf(USART_DEBUG, "Tips:	Message ID Err\r\n");
			
		break;
		case PINGRESP:
			printf("心跳回应");
			heart_flag=Heart_OK;
		break;
				
			
		default:
			result = -1;
		break;
	}
	
		//M8266_Clear();
			
	
	if(result == -1)
		return;
	
	dataPtr = strchr(req, ':');							//搜索':'
	
	if(dataPtr != NULL)									//如果找到了
	{
		dataPtr++;
		
		while(*dataPtr >= '0' && *dataPtr <= '9')		//判断是否是下发的命令控制数据
		{
			numBuf[num++] = *dataPtr++;
		}
		
		num = atoi((const char *)numBuf);				//转为数值形式
		if(strstr((char *)req, "photo"))				//搜索"Fire"
		{
			
			if(num == 1)       photo=1;
			else             	 photo=0;	
	
		}
				else if(strstr((char *)req, "time"))				//搜索"Fire"
				{
					time_count=num;   //赋值

				}
					else if(strstr((char *)req, "Hum_Max"))				//搜索"Fire"
				{
					humi_max=num;   //赋值

				}
					else if(strstr((char *)req, "Hum_Min"))				//搜索"Fire"
				{
					humi_min=num;   //赋值

				}
						else if(strstr((char *)req, "Temp_Max"))				//搜索"Fire"
				{
					temp_max=num;   //赋值

				}
						else if(strstr((char *)req, "Temp_Min"))				//搜索"Fire"
				{
					temp_min=num;   //赋值

				}
				else if(strstr((char *)req, "door"))
				{
					if(num == 1)       
					{Door_OPEN;
					 mDelay(3000);
					Door_ClOSE;
					}
			    else             	 Door_ClOSE;	
					
				}
				else if(strstr((char *)req, "Door"))
				{
					if(num == 1)       
					{
					photo=1;
					printf("有人开门\r\n");

					}
				}
	}
	
	if(type == CMDREQ && result == 0)						//如果是命令包 且 解包成功
	{
		EDP_FreeBuffer(cmdid_devid);						//释放内存
		EDP_FreeBuffer(req);												//回复命令
		M8266WIFI_SPI_Send_Data(edpPacket._data, edpPacket._len,link_no,&link_status);	//上传平台
		EDP_DeleteBuffer(&edpPacket);						//删包
	}

}



void OneNet_Send_heart(void)
{
		EDP_PACKET_STRUCTURE edpPacket = {NULL, 0, 0, 0};	
		if(!EDP_PacketPing(&edpPacket))
		{
		printf("send ping pkt to server, bytes: %d\r\n", edpPacket._len);
		M8266WIFI_SPI_Send_Data(edpPacket._data, edpPacket._len,link_no,&link_status);
			hexdump(edpPacket._data,edpPacket._len);
		EDP_DeleteBuffer(&edpPacket);									//删包
		}
		else 
		printf("心跳失败");
}





#define PKT_SIZE 1000
void OneNet_SendData_Picture(char *devid,  char * pic_name)
{
	
	EDP_PACKET_STRUCTURE edpPacket = {NULL, 0, 0, 0};					//协议包
	
	char type_bin_head[] = "{\"ds_id\":\"ppp\"}";						//图片数据头
	unsigned int read_num;	u16 pic_len;
	u16 ucAlign;

	bmpres = f_open( &bmpfsrc , (char *)pic_name, FA_OPEN_EXISTING | FA_READ);	
/*-------------------------------------------------------------------------------------------------------*/
	if(bmpres == FR_OK)
	{
		printf("打开文件成功\r\n");

		/* 读取文件头信息  两个字节*/  
		pic_len=bmpfsrc.fsize;
		ucAlign=pic_len%PKT_SIZE;
	
	if(EDP_PacketSaveData(devid, pic_len, type_bin_head, kTypeBin, &edpPacket) == 0)
	{	
			M8266_Clear();
			M8266WIFI_SPI_Send_Data(edpPacket._data, edpPacket._len,link_no,&link_status);
		//hexdump(edpPacket._data,edpPacket._len);
		EDP_DeleteBuffer(&edpPacket);									//删包
		UsartPrintf(USART_DEBUG, "image len = %d\r\n", pic_len);
		while(pic_len > 0)
		{
			mDelay(500);												//传图时，时间间隔会大一点，这里额外增加一个延时	
			if(pic_len >= PKT_SIZE)
			{
			bmpres=f_read(&bmpfsrc,&camera_buf,PKT_SIZE*sizeof(unsigned char),&read_num);
			//if(read_num||res==0) break;
			M8266WIFI_SPI_Send_Data(camera_buf,PKT_SIZE,link_no,&link_status);
				//串口发送分片
				
		//		pImage += PKT_SIZE;
				pic_len -= PKT_SIZE;
			}
			
			else
			{
			
				bmpres=f_read(&bmpfsrc,&camera_buf,ucAlign*sizeof(unsigned char),&read_num);
				M8266WIFI_SPI_Send_Data(camera_buf,ucAlign,link_no,&link_status);
				//ESP8266_SendData(camera_buf, ucAlign);					//串口发送最后一个分片
				pic_len = 0;
			}
		}
		
		UsartPrintf(USART_DEBUG, "image send ok\r\n");
	}
	else
		UsartPrintf(USART_DEBUG, "EDP_PacketSaveData Failed\r\n");

	

}
	
	}


//==========================================================
//	函数名称：	OneNET_CmdHandle
//
//	函数功能：	读取平台rb中的数据
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
unsigned char  OneNET_CmdHandle(void)
{
	
	unsigned char *dataPtr = NULL;		//数据指针
	
	dataPtr = M8266_GetIPD(5);		//检查是否是平台数据
		if(dataPtr != NULL)
		{	
				OneNet_RevPro(dataPtr);					//集中处理
			  return 1;
			
		}
		return 0;

}




 u8 OneNet_SendPhoto(void)
{
		static int name_count=0;
	  char name[40];
		name_count++;
		sprintf(name,"0:photos_%d.jpg",name_count); //字符串格式化
		printf("\r\n正在拍照...");	
	  Ov7725_vsync=0;
		if(JPEG_encode(name)== 0)
			{
			  OneNet_SendData_Picture(NULL,name);
				return 1;	
			}
			else
			{
			  printf("\r\n拍照失败");
				return 0;
				
				
			}	
}

