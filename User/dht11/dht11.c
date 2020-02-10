#include "dht11.h"
#include "./systick/bsp_SysTick.h"


void DHT11_delay_us(u32 time)
{ 
	u16 i=0;  
   while(time--)
   {
      i=10;  //自己定义
      while(i--) ;    
   }

}

void DHT11_delay_ms(u32 z)
{
  u32 i=1000*z;
	DHT11_delay_us(i);

}




void DHT11_Init(void)
{
	DHT11_GPIO_Config();	//DHT11的GPIO端口初始化
	
	DHT11_Dout_1;			//拉高PE.6
}


/*
************************************************************
*	函数名称：	DHT11_GPIO_Config
*	函数功能：	配置DHT11用到的I/O口
*	入口参数：	无
*	返回参数：	无
*	说明：		PE.6推挽输出
************************************************************
*/
void DHT11_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}


/*
************************************************************
*	函数名称：	DHT11_Mode_IPU
*	函数功能：	使DHT11-DATA引脚变为上拉输入模式
*	入口参数：	无
*	返回参数：	无
*	说明：		PE.6上拉输入
************************************************************
*/
void DHT11_Mode_IPU(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}


/*
************************************************************
*	函数名称：	DHT11_Mode_Out_PP
*	函数功能：	使DHT11-DATA引脚变为推挽输出模式
*	入口参数：	无
*	返回参数：	无
*	说明：		PE.6推挽输出
************************************************************
*/
void DHT11_Mode_Out_PP(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}


/*
************************************************************
*	函数名称：	DHT11_ReadByte
*	函数功能：	从DHT11读取一个字节，最高有效字节（MSB）先行
*	入口参数：	无
*	返回参数：	temp：单个数值
*	说明：		  
************************************************************
*/
uint8_t DHT11_ReadByte(void)
{
	uint8_t i = 0;
	uint8_t temp = 0;
	
	for(i = 0; i < 8; i++)
	{
		//每bit以50us低电平标置开始，轮询直到从机发出的50us低电平结束
		while(DHT11_Dout_IN == Bit_RESET){};
			
		/*DHT11 以50us的低电平 + 26~28us的高电平表示“0”，以50us的低电平 + 70us高电平表示“1”，
		通过检测 x us后的电平即可区别这两个状 ，x 即下面的延时*/
		DHT11_delay_us(35);								//延时x us 这个延时需要大于数据0持续的时间即可
		
			
		if(DHT11_Dout_IN == Bit_SET)				//x us后仍为高电平表示数据“1” 
		{
			while(DHT11_Dout_IN == Bit_SET){};		//等待数据1的高电平结束
					
			temp |= (u8)(0x01 << (7 - i));			//把第7-i位置1，MSB先行
		}
		else										// x us后为低电平表示数据“0”
		{
			temp &= (u8)~(0x01 << (7 - i));			//把第7-i位置0，MSB先行
		}
	}
	return temp;
}


/*
************************************************************
*	函数名称：	DHT11_Read_TempAndHumidity
*	函数功能：	从DHT11读取一个字节，最高有效字节（MSB）先行
*	入口参数：	DHT11_Data：温湿度结构体
*	返回参数：	SUCCESS(或ERROR)
*	说明：		一次完整的数据传输为40bit，高位先出
*			8bit 湿度整数 + 8bit 湿度小数 + 8bit 温度整数 + 8bit 温度小数 + 8bit 校验和
*			DHT11为从机，微处理器为主机，只有主机呼叫从机，从机才能应答 
************************************************************
*/
uint8_t DHT11_Read_TempAndHumidity(DHT11_Data_TypeDef *DHT11_Data)
{
	DHT11_Mode_Out_PP();				//引脚设为输出
	DHT11_Dout_0;						//主机拉低
	DHT11_delay_ms(18);						//延时18ms

	DHT11_Dout_1;						//总线拉高
	DHT11_delay_us(30);						//主机延时30us

	DHT11_Mode_IPU();					//主机设为输入，判断从机响应信号

	//判断从机是否有低电平响应信号 如不响应则跳出，响应则向下运行   
	if(DHT11_Dout_IN == Bit_RESET)     
	{
		//轮询直到从机发出80us低电平 作为应答信号 
		while(DHT11_Dout_IN == Bit_RESET){};

		//轮询直到从机发出的80us高电平 通知微处理器准备接收数据
		while(DHT11_Dout_IN == Bit_SET){};

		//开始接收数据   
		DHT11_Data->humi_int = DHT11_ReadByte();
		DHT11_Data->humi_deci = DHT11_ReadByte();
		DHT11_Data->temp_int = DHT11_ReadByte();
		DHT11_Data->temp_deci = DHT11_ReadByte();
		DHT11_Data->check_sum = DHT11_ReadByte();

		DHT11_Mode_Out_PP();			//读取结束，引脚改为输出模式
		DHT11_Dout_1;					//主机拉高

		//检查读取的数据是否正确
		if(DHT11_Data->check_sum == DHT11_Data->humi_int + DHT11_Data->humi_deci + DHT11_Data->temp_int + DHT11_Data->temp_deci)
		{
			return SUCCESS;
		}
		else 
		{    
			return ERROR;
		}
	}
	else
	{
		return ERROR;
	}
}

