#ifndef _BEEP_H_
#define _BEEP_H_


#include "stm32f10x.h"


/************************** BEEP 数据类型定义********************************/
typedef struct
{
	_Bool beep_status;
}BEEP_INFO;


/************************** BEEP 函数宏定义********************************/
#define BEEP_ON		1
#define BEEP_OFF	0


/************************** BEEP 结构体定义********************************/
extern BEEP_INFO beepInfo;


/************************** BEEP 函数宏定义********************************/
void Beep_Init(void);
void Beep_Set(_Bool status);


#endif
