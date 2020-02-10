/*************************************************************************
*项目代码或者名称： 基于STM32+0V7670+NRF24L01的无线视频智能小车
*创建日期：         2011.4.9
*创建人:            陈志强
*模块功能：         DEBUG..c
*修改日期：
*产考文档：          IJG JPEG LIBRARY
*特别说明:   		改压缩算法来源于(JPEG Group's software)IJG JPEG LIBRARY，
                    www.ouravr.com的VERYVERY朋友对其进行了移植，我对其进行了
                    优化和修改！
                    调试过程中提示的信息                   
*************************************************************************/
#include <stdio.h>
#include "jinclude.h"

void show_inbuf(jpeg_compress_info *cinfo, char *message) ;

void show_comp_dcttable(jpeg_compress_info *cinfo, char *message);
void show_message(char *message);
void show_msg(char *message);
void  show_dcttable(DCTBLOCK val, char *message);

void show_inbuf(jpeg_compress_info *cinfo, char *message) 
{
  int aa, bb, cc;
  printf("%s\n", message);
  printf("next_line: %d\n", cinfo->next_line);
  for (aa=0; aa<cinfo->inbuf_height; aa++) {
    printf("%03d:\t", aa);
    for (bb=0; bb<cinfo->inbuf_width; bb++) {
      for (cc=0; cc<1; cc++) {
        printf("%02X", cinfo->inbuf[(aa*cinfo->inbuf_width+bb)*3+cc]);
      }
      printf(",");
      if ( (bb%8) == 7) printf("  ");
    }
    printf("\n");
  }
}
/*
void show_comp_dcttable(jpeg_compress_info *cinfo, char *message) 
{
  int mm, nn, aa, bb, cc;
  printf("%s\n", message);
  for (mm=0; mm<cinfo->num_comp; mm++) {
    printf("component<%d>:\t\n", mm);
    for (nn=0; nn<cinfo->comp[mm].num_dct_table; nn++) {
      printf("dct table %d:\t\n", nn);
      for (aa=0; aa<DCTSIZE; aa++) {
        for (bb=0; bb<DCTSIZE; bb++) {
          printf("%02X,", cinfo->comp[mm].dct_table[nn][aa*DCTSIZE+bb]);
        }
        printf("\n");
      }
    }
  }
}
*/

volatile int temp_dct[64];
void  show_dcttable(DCTBLOCK val, char *message) 
{
  int aa, bb;
  
  
  //printf("\t%s\n", message);
  for (aa=0; aa<DCTSIZE; aa++) 
  {
    //printf("\t");
    for (bb=0; bb<DCTSIZE; bb++) 
    {
      //printf("%-3d ", val[aa*DCTSIZE + bb]);
      temp_dct[aa*8 + 8] = val[aa*DCTSIZE + bb];
    }
    //printf("\n");
  }
}

void show_message(char *message) 
{
  printf("%s\n", message);
}

void show_msg(char *message) 
{
  printf("%s", message);
}


