/*
*@Description: 看门狗
*@Author: Yang
*@Date: 2023-05-15 21:01:33
*/
#ifndef __MYDOG_H
#define __MYDOG_H
#include "AllHead.h"

typedef struct
{
    void (*vDOG_Fwdgt_Init)(void);
    void (*vDOG_Wwdgt_Init)(void);
}MyDog_TypeDef;

extern MyDog_TypeDef MyDog;

void vDOG_Fwdgt_Init(void);
void vDOG_Wwdgt_Init(void);
#endif
