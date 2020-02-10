#ifndef _ONENET_H_
#define _ONENET_H_



#define DEVID	"30964714"

#define APIKEY	"Uv=e=yMBymo8In9FVA4Ub16Oleo="
#define Heart_OK 1
#define Heart_err 0


typedef struct
{
	unsigned short time;
	unsigned   char temp;
	unsigned   char humidit;
	_Bool fire;
	unsigned char tem_max;
	unsigned char  tem_min;
	unsigned char hum_max;
	unsigned char hum_min;
	
	
	
	

} data_Stream;


typedef enum
{

	NET_EVENT_Timer_Check_Err = 0,			//网络定时检查超时错误
	NET_EVENT_Timer_Send_Err,				//网络发送失败错误
	
	NET_EVENT_Send_HeartBeat,				//即将发送心跳包
	NET_EVENT_Send_Data,					//即将发送数据点
	
	NET_EVENT_Recv,							//Modbus用-收到数据查询指令
	NET_EVENT_SendPhoto,         //发送照片
	
	NET_EVENT_Check_Status,					//进入网络模组状态检查
	
	NET_EVENT_Device_Ok,					//网络模组检测Ok
	NET_EVENT_Device_Err,					//网络模组检测错误
	
	NET_EVENT_Initialize,					//正在初始化网络模组
	NET_EVENT_Init_Ok,						//网络模组初始化成功
	
	NET_EVENT_Auto_Create_Ok,				//自动创建设备成功
	NET_EVENT_Auto_Create_Err,				//自动创建设备失败
	
	NET_EVENT_Connect,						//正在连接、登录OneNET
	NET_EVENT_Connect_Ok,					//连接、登录成功
	NET_EVENT_Connect_Err,					//连接、登录错误
	
	NET_EVENT_Fault_Process,				//进入错误处理

} NET_EVENT;



enum data_type{data_stream=0,picture,heart,Cmd} ;


_Bool OneNet_DevLink(void);

void OneNet_SendData(volatile data_Stream *data_stream);
void OneNet_RevPro(unsigned char *cmd);

//unsigned char EDP_UnPacketRecv(unsigned char *cmd);
void OneNet_SendData_Picture(char *devid,  char * pic_name);
void OneNet_Send_heart(void);
void OneNET_CmdHandle(void);

#endif
