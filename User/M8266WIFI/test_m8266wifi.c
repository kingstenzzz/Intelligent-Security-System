/********************************************************************
 * test_m8266wifi.c
 * .Description
 *     Source file of M8266WIFI testing application 
 * .Copyright(c) Anylinkin Technology 2015.5-
 *     IoT@anylinkin.com
 *     http://www.anylinkin.com
 *     http://anylinkin.taobao.com
 *  Author
 *     wzuo
 *  Date
 *  Version
 ********************************************************************/
 
#include "stdio.h"
#include "brd_cfg.h"
#include "M8266WIFIDrv.h"
#include "M8266HostIf.h"
#include "M8266WIFI_ops.h"
#include "led.h"

void M8266WIFI_Test(void)
{
	 u16 i;
	 u16 status = 0;
	 u8  link_no=0;

#ifdef USE_LED_AND_KEY_FOR_TEST	
	for(i=0; i<3; i++)   // Flash the Two LEDs 4 times in the Main Board to indicate we start test. not mandatory.
	{                    // Chinese: 单片机主板上的LED灯闪烁4次，用于提示代码执行到这里的方便调试。和模组通信无关，非必须，可略。若没有定义宏USE_LED_AND_KEY_FOR_TEST，此处不会被编译进来。
      	 LED_set(0, 0); LED_set(1, 1);  M8266WIFI_Module_delay_ms(100);
		     LED_set(0, 1); LED_set(1, 0);  M8266WIFI_Module_delay_ms(100);
		     LED_set(0, 0); LED_set(1, 1);  M8266WIFI_Module_delay_ms(100);
		     LED_set(0, 1); LED_set(1, 0);  M8266WIFI_Module_delay_ms(100);
	 }
	 LED_set(0, 1); LED_set(1, 1);
#endif	 

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	 Macro Defitions SETUP  SOCKET CONNECTIONS  (Chinese: 创建套接字的一些宏)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	 
   /////////////
   ////	 Macro for Test Type(Chinese：测试类型宏定义)
	 #define TEST_M8266WIFI_TYPE    1	     //           1 = Repeative Sending, 2 = Repeative Reception, 3 = Echo  4 = multi-clients transimission test
	                                       // (Chinese: 1=模组向外不停地发送数据 2=模组不停地接收数据 3= 模组将接收到的数据发送给发送方 4=多客户端测试) 

	 /////////////
   ////	 Macros for Socket Connection Type (Chinese：套接字类型的宏定义) 
	 #define TEST_CONNECTION_TYPE   1    //           0=WIFI module as UDP, 1=WIFI module as TCP Client, 2=WIFI module as TCP Server
	                                      // (Chinese: 0=WIFI模组做UDP, 1=WIFI模组做TCP客户端, 2=WIFI模组做TCP服务器
   /////////////
   ////	 Macros for Soket ip:port pairs  (Chinese：套接字的本地端口和目标地址目标端口的宏定义) 
   //local port	(Chinese：套接字的本地端口)
#if (TEST_CONNECTION_TYPE==1)         //// if module as TCP Client (Chinese:如果模组作为TCP客户端)
	 #define TEST_LOCAL_PORT  			0			//           local port=0 will generate a random local port each time fo connection. To avoid the rejection by TCP server due to repeative connection with the same ip:port
   	                                    // (Chinese: 当local port传递的参数为0时，本地端口会随机产生。这一点对于模组做客户端反复连接服务器时很有用。因为随机产生的端口每次会不一样，从而避免连续两次采用同样的地址和端口链接时被服务器拒绝。
#elif (TEST_CONNECTION_TYPE==0) || (TEST_CONNECTION_TYPE==2) //// if module as UDP or TCP Server (Chinese:如果模组作为UDP或TCP服务器)
   #define TEST_LOCAL_PORT  			4321  //           a local port should be specified //(Chinese:如果模组作为UDP或TCP服务器，则需要指定该套接字的本地端口)
#else
#error WRONG TEST_CONNECTION_TYPE defined !
#endif                                  // (Chinese: 如果模组作为TCP服务器或UDP，那么必须指定本地端口

   //local port	(Chinese：套接字的目标地址和目标端口)
#if (TEST_CONNECTION_TYPE==0)        //// if module as UDP (Chinese:如果模组作为UDP，则可以指定目标地址和端口，也可以随便填充，在发送数据时再设置或更改)
   #define TEST_REMOTE_ADDR    		"192.168.4.2"
   #define TEST_REMOTE_PORT  	    1234
#elif (TEST_CONNECTION_TYPE==1)      //// if module as TCP Client (Chinese:如果模组作为TCP客户端，当然必须指定目标地址和目标端口，即模组所要去连接的TCP服务器的地址和端口)
   #define TEST_REMOTE_ADDR    	 	"183.230.40.39"  // "www.baidu.com"
   #define TEST_REMOTE_PORT  	    876						// 80
#elif (TEST_CONNECTION_TYPE==2)     //// if module as TCP Server (Chinese:如果模组作为TCP服务器)
		#define TEST_REMOTE_ADDR      "1.1.1.1" // no need remote ip and port upon setup connection. below values just for filling and any value would be ok. 
    #define TEST_REMOTE_PORT  	  1234  	 //(Chinese: 创建TCP服务器时，不需要指定目标地址和端口，这里的数据只是一个格式填充，随便填写。
#else
#error WRONG TEST_CONNECTION_TYPE defined !
#endif			 


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	 Setup Connection and Config connection upon neccessary (Chinese: 创建套接字，以及必要时对套接字的一些配置)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////
//step 0: config tcp windows number (Chinese: 步骤0：如果是TCP类型的套接字，可以配置调整窗口参数）
#if ( 1 && ((TEST_CONNECTION_TYPE==1)||(TEST_CONNECTION_TYPE==2)) ) //If you hope to change TCP Windows, please change '0' to '1' in the #if clause before setup the connection
																																		//(Chinese: 如果想改变套接字的窗口数，可以将#if语句中的0改成1，这个配置需要在创建套接字之前执行)
// u8 M8266WIFI_SPI_Config_Tcp_Window_num(u8 link_no, u8 tcp_wnd_num, u16* status)
  if(M8266WIFI_SPI_Config_Tcp_Window_num(link_no, 4, &status)==0)
  {
		 while(1)
		 {
     #ifdef USE_LED_AND_KEY_FOR_TEST	 // led flash in 1Hz uppon errors
	      LED_set(0, 0); LED_set(1, 0); M8266WIFI_Module_delay_ms(100);
	      LED_set(0, 1); LED_set(1, 1); M8266WIFI_Module_delay_ms(100);
     #endif
	   }
	}
#endif

///////	
//step 1: setup connection (Chinese: 步骤1：创建套接字连接）

  //  u8 M8266WIFI_SPI_Setup_Connection(u8 tcp_udp, u16 local_port, char remote_addr, u16 remote_port, u8 link_no, u8 timeout_in_s, u16* status);
	if(M8266WIFI_SPI_Setup_Connection(TEST_CONNECTION_TYPE, TEST_LOCAL_PORT, TEST_REMOTE_ADDR, TEST_REMOTE_PORT, link_no, 20, &status)==0)
	{		
		 while(1)
		 {
     #ifdef USE_LED_AND_KEY_FOR_TEST	 // led flash in 1Hz uppon errors (Chinese: 如果创建套接字失败，则进入死循环和闪烁主板上的灯）
	      LED_set(0, 0); LED_set(1, 0); M8266WIFI_Module_delay_ms(500);
	      LED_set(0, 1); LED_set(1, 1); M8266WIFI_Module_delay_ms(500);
     #endif
		 }
	}
	else  // else: setup connection successfully, we could config it (Chinese: 创建套接字成功，就可以配置套接字）
	{
#if (0 && (TEST_CONNECTION_TYPE == 0) )  		//// If UDP, then the module could join a multicust group. If you hope to use multicust, Change first '0' to '1'  in the #if clause
		                                        //   (Chinese: 如果这个套接字是UDP，那么可以配置成组播模式。如果需要配置成组播，可以将#if语句中的第一个'0'改成'1')
			//u8 M8266WIFI_SPI_Set_Multicuast_Group(u8 join_not_leave, char multicust_group_ip[15+1], u16* status)
		 if(M8266WIFI_SPI_Op_Multicuast_Group(0, "224.6.6.6", &status)==0)
     {
		   while(1)
		   {
       #ifdef USE_LED_AND_KEY_FOR_TEST   // led flash in 1Hz uppon errors (Chinese: 如果失败，则进入死循环和闪烁主板上的灯）
				  LED_set(0, 0); LED_set(1, 0); M8266WIFI_Module_delay_ms(1000);
	        LED_set(0, 1); LED_set(1, 1); M8266WIFI_Module_delay_ms(1000);
       #endif				 
		   }
     }
     else

#elif (TEST_CONNECTION_TYPE == 2)          //// If TCP server, then tcp server auto disconnection timeout, and max clients allowed could be set	
		                                        //  (Chinese: 如果TCP服务器，那么可以设置这个TCP服务器(因长时间无通信而)断开客户端的超时时间)
#if 1
		 //u8 M8266WIFI_SPI_Set_TcpServer_Auto_Discon_Timeout(u8 link_no, u16 timeout_in_s, u16* status)	
		 if( M8266WIFI_SPI_Set_TcpServer_Auto_Discon_Timeout(link_no, 120, &status) == 0)
     {
		   while(1)
		   {
       #ifdef USE_LED_AND_KEY_FOR_TEST	// led flash in 1Hz when error
	        LED_set(0, 0); LED_set(1, 0); M8266WIFI_Module_delay_ms(1000);
	        LED_set(0, 1); LED_set(1, 1); M8266WIFI_Module_delay_ms(1000);
       #endif		 
		   }
     }
#endif		 
#if 0
  		   //u8 M8266WIFI_SPI_Config_Max_Clients_Allowed_To_A_Tcp_Server(u8 server_link_no, u8 max_allowed, u16* status);
     else if( M8266WIFI_SPI_Config_Max_Clients_Allowed_To_A_Tcp_Server(link_no, 5, &status)==0)
		 {
		   while(1)
		   {
       #ifdef USE_LED_AND_KEY_FOR_TEST	// led flash in 1Hz when error
	        LED_set(0, 0); LED_set(1, 0); M8266WIFI_Module_delay_ms(1000);
	        LED_set(0, 1); LED_set(1, 1); M8266WIFI_Module_delay_ms(1000);
       #endif		 
		   }
		 }
#endif		 
		 else
#endif
		 //Setup Connection successfully (Chinese: 设置套接字链接成功)
     {
     #ifdef USE_LED_AND_KEY_FOR_TEST			 
		   // led 1 flash 4 times upon success 
		   LED_set(1, 0); M8266WIFI_Module_delay_ms(50); LED_set(1, 1); M8266WIFI_Module_delay_ms(50);
		   LED_set(1, 0); M8266WIFI_Module_delay_ms(50); LED_set(1, 1); M8266WIFI_Module_delay_ms(50);
		   LED_set(1, 0); M8266WIFI_Module_delay_ms(50); LED_set(1, 1); M8266WIFI_Module_delay_ms(50);
		   LED_set(1, 0); M8266WIFI_Module_delay_ms(50); LED_set(1, 1); M8266WIFI_Module_delay_ms(50);
     #endif			 
		 }
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	 Communication Test (Chinese: WIFI套接字的数据收发通信测试)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if (TEST_M8266WIFI_TYPE==1) // Transmission Test: to test send data to remote constantly // (Chinese: 发送测试，不断地向目标发送数据) 
{                            
	
#define TEST_SEND_DATA_SIZE   2920 //5840 //2048 // 1024   // If using large size of array to send, ensure system stack is large engough for the array variable
	                                                         // (Chinese: 如果使用较大的数组，记得确保有足够大的系统的堆栈来容纳这个大数组变量)

   u8  snd_data[TEST_SEND_DATA_SIZE];
	 volatile u16 sent = 0;
   volatile u32 total_sent = 0, MBytes = 0; 
	 u16 batch;
	 
	 volatile u8 debug_point;

 	 for(i=0; i<TEST_SEND_DATA_SIZE; i++) snd_data[i]=i;  //Generate an array of data to transmit (Chinese: 生成一个连续数据的数组作为待发送的数据) 
	 
   link_no=0;
	
 	 for(batch=0; ; batch++)
	 {
		  // below used to convenient reception end to differentiate packets when test and evaluate the reliability of transmission. Not necesary.
		  // (Chinese: 下面三句话是在改造发送的包，这样前后连续发送的包会不一样，这样在做传输的可靠性分析时，方便接收端分析数据。非必须。)
		  snd_data[0]= batch>>8;    							// (Chinese: first two bytes using bacth to differentiate the continous packets (Chinese: 包开始两个字节设定为不同的序号，以便区分不同的包) 
			snd_data[1]= batch&0xFF;                
		  snd_data[TEST_SEND_DATA_SIZE-2]=0xFF;		// last byte using customised to lable end of a packet for the sake of searching for data verification. Here FF FF as an example
      snd_data[TEST_SEND_DATA_SIZE-1]=0xFF;   // (Chinese: 包里的最后两个字节设定为一个特殊的字节，以方便接收端搜索包的结尾。这里举例用的是 FF FF。因为产生的发送数据时顺序递增的本身不可能出现FF FF，所以这样改造后收到的FF FF必然代表结尾。也可以用其他的模式匹配。

		  if(total_sent> 1024*1024)  // watch MBytes*1024*1024+total_sent, which is the count of data module sends, compared with the received count at the reception end, to determin the packet loss etc
			{                          // (Chinese: 持续发送一段时间后，观察表达式 MBytes*1024*1024+total_sent 的值，和接收端接收到的数据个数进行比较，可以粗略衡量模组的丢包率。)
				 MBytes++;
				 total_sent -= 1024*1024;
			}

#if 1  // using M8266WIFI_SPI_Send_BlockData() to sent block data (Chinese: 调用M8266WIFI_SPI_Send_BlockData() 来发送大块数据
  {
		//u32 M8266WIFI_SPI_Send_BlockData(u8 Data[], u32 Data_len, u16 max_loops, u8 link_no, char* remote_ip, u16 remote_port, u16* status);
      sent = M8266WIFI_SPI_Send_BlockData(snd_data, TEST_SEND_DATA_SIZE, 5000, link_no, NULL, NULL, &status);
		  total_sent += sent;
		  		
			if( (sent==TEST_SEND_DATA_SIZE) && ((status&0xFF)==0x00) ) //Send successfully 
			{
			}
			else if( (status&0xFF) == 0x1E)			       // 0x1E = too many errors ecountered during sending and can not fixed, or transsmission blocked heavily(Chinese: 发送阶段遇到太多的错误或阻塞了，可以考虑加大max_loops)
      {
				debug_point = 1;
				//add some process here (Chinese: 可以在此处加一些处理，比如增加max_loops的值)
			}
		  else if(  ((status&0xFF) == 0x14)			 // 0x14 = connection of link_no not present (Chinese: 该套接字不存在)
             || ((status&0xFF) == 0x15) )    // 0x15 = connection of link_no closed(Chinese: 该套接字已经关闭或断开)			
      {
				 debug_point = 2;
				 //need to re-establish the socket connection (Chinese: 需要重建建立套接字)
			}
			else if( (status&0xFF) == 0x18 )        // 0x18 = TCP server in listening states and no tcp clients have connected. (Chinese: 这个TCP服务器还没有客户端连接着它)
			{
				 debug_point = 3;
				 M8266HostIf_delay_us(99);
			}
			else
	    {
				 debug_point = 4;
	       M8266HostIf_delay_us(101);
	    }
	}			
#else // using M8266WIFI_SPI_Send_Data() to sent data packet by packet (Chinese: 调用M8266WIFI_SPI_Send_Data() 来一个一个包的发送数据，实际几乎就是M8266WIFI_SPI_Send_BlockData()的函数实现代码
	{
         u16 tcp_packet_size = 1024;
         u16 loops     = 0;
		     u16 max_loops = 5000;
			   u32 len       = TEST_SEND_DATA_SIZE; 
		     
         for(sent=0, loops=0; (sent<len)&&(loops<=max_loops); loops++)
         {		
				    sent += M8266WIFI_SPI_Send_Data(snd_data+sent, ((len-sent)>tcp_packet_size)?tcp_packet_size:(len-sent), link_no, &status);
            if(sent>=len)  break;
			      if((status&0xFF) == 0x00)
			      {
							 loops = 0;
			      }
		        else
			      {
				      if(   ((status&0xFF) == 0x14)      // 0x14 = connection of link_no not present (Chinese: 该套接字不存在)
                 || ((status&0xFF) == 0x15) )    // 0x15 = connection of link_no closed(Chinese: 该套接字已经关闭或断开)
	            {
								 M8266HostIf_delay_us(99);
          			 //need to re-establish the socket connection (Chinese: 需要重建建立套接字)
	            }
							else if( (status&0xFF) == 0x18 )        // 0x18 = TCP server in listening states and no tcp clients have connected. (Chinese: 这个TCP服务器还没有客户端连接着它)
			        {
				         M8266HostIf_delay_us(100);
			        }
	            else
	            {
	               M8266HostIf_delay_us(250);
	            }
			     }
         } // end of for(...
			
			   total_sent += sent;
		   }
#endif
			 
	 } // end of for(batch=0; ; batch++)
 }
	
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#elif (TEST_M8266WIFI_TYPE==2)  // to test constant reception from remote // (Chinese: 接收送测试，不断地接收来自远端节点发送的数据)
{
#define  RECV_DATA_MAX_SIZE  2048

   u8  RecvData[RECV_DATA_MAX_SIZE];
   u16 received = 0;
	 u32 total_received = 0;
   u32 MBytes = 0;
	
	 for(i=0; i<RECV_DATA_MAX_SIZE; i++) RecvData[i]=0xFF-i; 	
   M8266WIFI_SPI_Send_Data(RecvData, 1024, link_no, &status);  // just send to check the ip address on TCP tester. not necessary 
																															 // (Chinese: 向远端节点发送一个包，以便在远端查看模组的IP地址和端口，非必须) 
	 while(1)
	 {
			if(M8266WIFI_SPI_Has_DataReceived())
			{
				//u16 M8266WIFI_SPI_RecvData(u8 data[], u16 max_len, uint16_t max_wait_in_ms, u8* link_no, u16* status);
	      received = M8266WIFI_SPI_RecvData(RecvData, RECV_DATA_MAX_SIZE, 5*1000, &link_no, &status);

			  if(  (status&0xFF)!= 0 )  
			  {
				    if( (status&0xFF)==0x22 )      // 0x22 = Module buffer has no data received
				    {  
			         M8266HostIf_delay_us(250); 
					   //M8266WIFI_Module_delay_ms(1);
				    }
				    else if( (status&0xFF)==0x23 )   
				    { // the packet had not been received completed in the last call of M8266WIFI_SPI_RecvData()
							// and has continued to be received in this call of M8266WIFI_SPI_RecvData()
							// do some work here if necessary
							// (Chinese: 上次调用接收函数M8266WIFI_SPI_RecvData()时，并未收完整上次那个包，于是这次调用继续接受之前的包。可以在这里做一些工作，比如将一次接收缓冲区和做大长度上限加大。
				    }
				    else if(  (status&0xFF)==0x24)   
				    { 
	            // the packet is large in size than max_len specified and received only the max_len // TCP?????????
							// normally caused by the burst transmision by the routers after some block. 
							// Suggest to stop the TCP transmission for some time
					    // do some work here if necessary
							// (Chinese: 模组所接收到而正在被读取的这个包的长度，超过了这里的max_len参数所指定的长度。通常是因为远端阵发发送或路由器等阻塞时出现了大面积粘包导致到达模块的包过长，
							//           或者远端实际发送的就是一个长包，其长度超过了这里所指定的最大长度上限。如果是前者的原因，建议暂停远端TCP通信一段时间。如果是后者，建议加大max_len的数值或者
							//           不做任何处理，不做处理时，单片机侧这边接收到的长包会被拆成多个小包需要自行再次破解。
							//           必要时可以做相应的处理。
				    }
						else
						{
					    // do some work here if necessary
							// (Chinese: 其他异常，必要时可以做一些处理。
						}
			  }				
#define  TEST_RECV_UNIT (1024*1024)
				 total_received += received;
				 if( total_received >= (TEST_RECV_UNIT) )
				 {
					   LED_set(0, MBytes&0x01);	
					   total_received = total_received%(TEST_RECV_UNIT);
				     MBytes++;
         }
			} // end of if(M8266WIFI_SPI_Has_DataReceived())				 
	 } // end of while(1)
 }	 

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#elif (TEST_M8266WIFI_TYPE==3)  // Echo test: to receive data from remote and then echo back to remote (Chinese: 收发测试，模组将接收到的数据立刻返回给发送方)
{
#define  RECV_DATA_MAX_SIZE  2048    

   u8  RecvData[RECV_DATA_MAX_SIZE];   // make sure the stack size is more than RECV_DATA_MAX_SIZE to avoid stack leak (Chinese: 这里有大数组。请确保单片机的堆栈足够大，至少不少于数据和变量的总长度和，否则单片机程序可能会出现溢出错误)
   u16 received = 0;
	 u16 sent;
	
	 for(i=0; i<RECV_DATA_MAX_SIZE; i++) RecvData[i]=i; 

   link_no = 0;
   sent = M8266WIFI_SPI_Send_Data(RecvData, 1024, link_no, &status);

   while(1)
	 {
			if(M8266WIFI_SPI_Has_DataReceived()) //if received data (Chinese: 如果接收到数据)
			{
				//Receive the data (Chinese: 单片机接收数据)
				//u16 M8266WIFI_SPI_RecvData(u8 data[], u16 max_len, uint16_t max_wait_in_ms, u8* link_no, u16* status);
	      received = M8266WIFI_SPI_RecvData(RecvData, RECV_DATA_MAX_SIZE, 5*1000, &link_no, &status);
				
				if(received!=0) //if received data length is not 0  (Chinese: 如果单片机的确接收到了数据，即长度不等于0)
				{
					u16 tcp_packet_size = 1024;
          u16 loops     = 0;
		      u16 max_loops = 5000;
			    u32 len       = received; 
		     
          for(sent=0, loops=0; (sent<len)&&(loops<=max_loops); loops++)
          {		
				    sent += M8266WIFI_SPI_Send_Data(RecvData+sent, ((len-sent)>tcp_packet_size)?tcp_packet_size:(len-sent), link_no, &status);
            if(sent>=len)  break;
			      if((status&0xFF) == 0x00)
			      {
							 loops = 0;
			      }
		        else
			      {
				      if(   ((status&0xFF) == 0x14)      // 0x14 = connection of link_no not present (Chinese: 该套接字不存在)
                 || ((status&0xFF) == 0x15) )    // 0x15 = connection of link_no closed(Chinese: 该套接字已经关闭或断开)
	            {
								 M8266HostIf_delay_us(99);
          			 //need to re-establish the socket connection (Chinese: 需要重建建立套接字)
	            }
							else if( (status&0xFF) == 0x18 )        // 0x18 = TCP server in listening states and no tcp clients have connected. (Chinese: 这个TCP服务器还没有客户端连接着它)
			        {
				         M8266HostIf_delay_us(100);
			        }
	            else
	            {
	               M8266HostIf_delay_us(250);
	            }
			      }
          } // end of for(...
		    } // end of if(received!=0)
			}
		} // end of while(1)
}	 
 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#elif (TEST_M8266WIFI_TYPE==4)  //           multi-clients transimission test: TCP Server or UDP receive data from multilple sender and echo back the data to the coresponding source
																// (Chinese: 多客户端收发测试，模组作为TCP服务器或UDP，接收来自多个远端节点的数据，并将接收到的数据立刻发送回给对应的发送方)
#if ( (TEST_CONNECTION_TYPE!=0) && (TEST_CONNECTION_TYPE!=2) )
#error multiple clients test could be supported only when socket connection is UDP or TCP Server
#endif
{	
#define  RECV_DATA_MAX_SIZE  2048    

   u8  RecvData[RECV_DATA_MAX_SIZE];   // make sure the stack size is more than RECV_DATA_MAX_SIZE to avoid stack leak (Chinese: 这里有大数组。请确保单片机的堆栈足够大，至少不少于数据和变量的总长度和，否则单片机程序可能会出现溢出错误)
   u16 received = 0;
	 u16 sent;
	 u8  remote_ip[4];
	 u16 remote_port;
	
	 for(i=0; i<RECV_DATA_MAX_SIZE; i++) RecvData[i]=i; 

   link_no = 0;

   while(1)
	 {
			if(M8266WIFI_SPI_Has_DataReceived()) //if received data (Chinese: 如果接收到数据)
			{
				//Receive the data using M8266WIFI_SPI_RecvData_ex() which could return the address and port of the sender
				//(Chinese: 单片机使用函数M8266WIFI_SPI_RecvData_ex()来接收数据，这个函数可以返回发送方的地址和端口)
		
        //u16 M8266WIFI_SPI_RecvData_ex(u8 Data[], u16 max_len, uint16_t max_wait_in_ms, u8* link_no, u8 remote_ip[4], u16* remote_port, u16* status)
				received = M8266WIFI_SPI_RecvData_ex(RecvData, RECV_DATA_MAX_SIZE, 5*1000, &link_no, remote_ip, &remote_port, &status);

				if(received!=0) //if received data length is not 0  (Chinese: 如果单片机的确接收到了数据，即长度不等于0)
				{

					//format the address in hex to ascii(Chinese: 将16进制格式的ip地址转化字符串形式的ip地址，因为下面的函数传递的是字符串形似的地址参数)
				  char dest_addr[15+1]={0};
				  sprintf(dest_addr, "%d.%d.%d.%d", remote_ip[0], remote_ip[1], remote_ip[2], remote_ip[3]);

					u16 tcp_packet_size = 1024;
          u16 loops     = 0;
		      u16 max_loops = 5000;
			    u32 len       = received; 
		     
          for(sent=0, loops=0; (sent<len)&&(loops<=max_loops); loops++)
          {		
#if ( TEST_CONNECTION_TYPE==0 )  //if UDP, (Chinese: UDP实现向多个对等的UDP服务发送数据
						//If UDP, Send the data using M8266WIFI_SPI_Send_Udp_Data() which could send the data to the specified UDP peer
						//(Chinese: 如果是UDP，单片机使用函数M8266WIFI_SPI_Send_Udp_Data()来发送数据，这个函数可以传递目标地址和端口，也就是可以指定发送到哪个对等的UDP服务节点)
						  //u16 M8266WIFI_SPI_Send_Udp_Data(u8 Data[], u16 Data_len, u8 link_no, char* udp_dest_addr, u16 udp_dest_port, u16* status)
				    sent += M8266WIFI_SPI_Send_Udp_Data(RecvData+sent, ((len-sent)>tcp_packet_size)?tcp_packet_size:(len-sent), link_no, dest_addr, remote_port, &status);
            if(sent>=len)  break;
			      if((status&0xFF) == 0x00)
			      {
							 loops = 0;
			      }
		        else
			      {
				      if((status&0xFF) == 0x14)      // 0x14 = connection of link_no not present (Chinese: 该套接字不存在)
	            {
								 M8266HostIf_delay_us(99);
          			 //need to re-establish the socket connection (Chinese: 需要重建建立套接字)
	            }
	            else
	            {
	               M8266HostIf_delay_us(250);
	            }
			      }            
#elif ( TEST_CONNECTION_TYPE==2 )
						//If tcp server, Send the data using M8266WIFI_SPI_Send_Data_to_TcpClient() which could send the data to the specified client
						//(Chinese: 如果是TCP服务器，单片机使用函数M8266WIFI_SPI_Send_Data_to_TcpClient()来发送数据，这个函数可以传递目标地址和端口，也就是可以指定发送到哪个客户端)
						  //u16 M8266WIFI_SPI_Send_Data_to_TcpClient(u8 Data[], u16 Data_len, u8 server_link_no, char* tcp_client_dest_addr, u16 tcp_client_dest_port, u16* status)
				    sent += M8266WIFI_SPI_Send_Data_to_TcpClient(RecvData+sent, ((len-sent)>tcp_packet_size)?tcp_packet_size:(len-sent), link_no, dest_addr, remote_port, &status);
            if(sent>=len)  break;
			      if((status&0xFF) == 0x00)
			      {
							 loops = 0;
			      }
		        else
			      {
				      if(   ((status&0xFF) == 0x14)      // 0x14 = connection of link_no not present (Chinese: 该套接字不存在)
                 || ((status&0xFF) == 0x15) )    // 0x15 = connection of link_no closed(Chinese: 该套接字已经关闭或断开)
	            {
								 M8266HostIf_delay_us(99);
          			 //need to re-establish the socket connection (Chinese: 需要重建建立套接字)
	            }
							else if( (status&0xFF) == 0x18 )        // 0x18 = TCP server in listening states and no tcp clients have connected. (Chinese: 这个TCP服务器还没有客户端连接着它)
			        {
				         M8266HostIf_delay_us(100);
			        }
	            else
	            {
	               M8266HostIf_delay_us(250);
	            }
			      }
#endif						
          } // end of for(...
		    } // end of if(received!=0)
			}
		} // end of while(1)
}	 
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
#else
#error NOT Supported Test Type! should be 1~4!		
#endif

} // end of M8266WIFI_Test
