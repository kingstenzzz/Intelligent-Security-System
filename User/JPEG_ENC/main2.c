/*************************************************************************
*项目代码或者名称： 基于STM32+0V7670+NRF24L01的无线视频智能小车
*创建日期：         2011.4.9
*创建人:            陈志强
*模块功能：         MAIN2.c
*修改日期：
*产考文档：          IJG JPEG LIBRARY
*特别说明:   	     改压缩算法来源于(JPEG Group's software)IJG JPEG LIBRARY，
                    www.ouravr.com的VERYVERY朋友对其进行了移植，我对其进行了
                    优化和修改！
                                       
*************************************************************************/
#include "jinclude.h"
#include "jcapi.h"
#include "test.h"
/*************************************************************************
*函数描述：全局变量和位定义
*参数入口：
*参数出口:
*函数说明:   
*************************************************************************/
unsigned char JPG_enc_buf[20000];//jpeg 输出
volatile unsigned int pt_buf;//缓冲区指针
__no_init unsigned char inbuf_buf[230400] @ 0x68000000;//输入缓冲，用外部SRAM
//注意：原始RGB565数据和输入缓冲用同一段外扩RAM！我这样做是节省空间!!
jpeg_compress_info info1;
JQUANT_TBL  JQUANT_TBL_2[2];
JHUFF_TBL   JHUFF_TBL_4[4];
unsigned char dcttab[3][512];
//压缩
void Compression(int width,int height,float quality) 
{
  jpeg_compress_info *cinfo;

  pt_buf=0;
  cinfo=jpeg_create_compress();
  cinfo->image_width=width;//设定图片大小
  cinfo->image_height=height;
  cinfo->output=JPG_enc_buf;//设定输出缓冲位置
  jpeg_set_default(cinfo,quality);  
  jpeg_start_compress(cinfo);
  while(cinfo->next_line<cinfo->image_height)//一行一行的扫描进输入缓冲中 
  {
    jpeg_write_scanline(cinfo,&inbuf_buf[(cinfo->next_line*cinfo->image_width*3)]);
  }
  jpeg_finish_compress(cinfo);
}


