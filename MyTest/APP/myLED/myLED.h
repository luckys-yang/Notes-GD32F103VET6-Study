/*
*@Description: LED
*@Author: Yang
*@Date: 2023-05-09 07:53:49
*/
#ifndef __MYLED_H
#define __MYLED_H
#include "AllHead.h"

// LED亮
#define LED_ON  1
// LED灭
#define LED_OFF 0

// 寄存器操作
#define LED1 PCout(13)


typedef struct
{
    // LED状态
    bool bLed1_State;
    void (*vLED_Init)(void);
    void (*vLED_Control)(uint8_t);
}MyLed_TypeDef;

extern MyLed_TypeDef MyLed;

void vLED_Init(void);
void vLED_Control(uint8_t swch);
#endif