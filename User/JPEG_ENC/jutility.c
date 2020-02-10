/*************************************************************************
*项目代码或者名称： 基于STM32+0V7670+NRF24L01的无线视频智能小车
*创建日期：         2011.4.9
*创建人:            陈志强
*模块功能：         JUTLILITY.c
*修改日期：
*产考文档：          IJG JPEG LIBRARY
*特别说明:   		改压缩算法来源于(JPEG Group's software)IJG JPEG LIBRARY，
                    www.ouravr.com的VERYVERY朋友对其进行了移植，我对其进行了
                    优化和修改！
                                       
*************************************************************************/
#include <math.h>
#include <stdio.h>
#include <assert.h>
#include "stdlib.h"
#include <string.h>
#include "jinclude.h"

void jutl_write_byte(UINT8 c, unsigned char *f);
void jutl_zigzag_sort (DCTBLOCK dptr);
void jutl_cc_rgb2ycc(JSAMPLE *data, int num);
//Z排序
void jutl_zigzag_sort(DCTBLOCK dptr) 
{
  int i;
  static const char zz_order[DCTSIZE2] = 
  {
     0,  1,  5,  6, 14, 15, 27, 28,
     2,  4,  7, 13, 16, 26, 29, 42,
     3,  8, 12, 17, 25, 30, 41, 43,
     9, 11, 18, 24, 31, 40, 44, 53,
    10, 19, 23, 32, 39, 45, 52, 54,
    20, 22, 33, 38, 46, 51, 55, 60,
    21, 34, 37, 47, 50, 56, 59, 61,
    35, 36, 48, 49, 57, 58, 62, 63
  };
  DCTBLOCK dtmp;//DCTVAL DCTBLOCK[DCTSIZE2];//8*8的像素块
  memcpy(&dtmp,dptr,sizeof(DCTBLOCK));//将DCT变换后的数据写入一个8*8的缓冲后进行Z排序
  for(i=0;i<DCTSIZE2;i++)//DCTSIZE2=64 
  {
    dptr[zz_order[i]]=dtmp[i];//将排序后的数据从小写入原始数据位置
  }
}
//RGB TO YUV色彩模式转换
void jutl_cc_rgb2ycc(JSAMPLE *data, int num) 
{
  int i;
  float a, b, c;
  struct three_component_color
  {
    JSAMPLE a,b,c;
  } *pcolor;
  pcolor = (struct three_component_color *)data;
  for (i=0; i<num; i++) 
  {
    a = pcolor[i].a;//R
    b = pcolor[i].b;//G
    c = pcolor[i].c;//B
    /* Y  color */
    pcolor[i].a = (JSAMPLE)( 0.29900f * a + 0.58700f * b + 0.11400f * c);
    /* Cb color */
    pcolor[i].b = (JSAMPLE)(-0.16874f * a - 0.33126f * b + 0.50000f * c + 128); 
    /* Cr color */
    pcolor[i].c = (JSAMPLE)( 0.50000f * a - 0.41869f * b - 0.08131f * c + 128); 
  }
}
//把压缩后的数据写入f中
extern unsigned int pt_buf;
void jutl_write_byte(UINT8 c,unsigned char *f) 
{
  f[pt_buf]=c;
  if(pt_buf<30000)
    pt_buf++;
}

