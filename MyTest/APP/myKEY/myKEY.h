/*
*@Description: 按键
*@Author: Yang
*@Date: 2023-05-09 07:54:19
*/
#ifndef __MYKEY_H
#define __MYKEY_H
#include "AllHead.h"

// 按键1
#define KEY1 gpio_input_bit_get(GPIOA,GPIO_PIN_15)
// 按键1按下
#define KEY1_VALUE  1
// 无按键按下
#define KEY_NULL    0


typedef struct
{
    uint8_t Key_Down_State[5];
    uint16_t Key_Long_Time;
    void (*vKEY_Init)(void);
    void (*vKey_Sacn)(void);
    void (*vKEY_Comply_Function)(void);
    void (*vKEY_Exti_Init)(void);
}MyKey_TypeDef;

extern MyKey_TypeDef MyKey;

void vKEY_Init(void);
void vKey_Sacn(void);
void vKEY_Comply_Function(void);
void vKEY_Exti_Init(void);
#endif