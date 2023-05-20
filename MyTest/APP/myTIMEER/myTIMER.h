/*
*@Description: 定时器
*@Author: Yang
*@Date: 2023-05-11 19:42:48
*/
#ifndef __MYTIMER_H
#define __MYTIMER_H
#include "AllHead.h"

typedef struct 
{
    // 捕获完成标志位
    bool bTimer2_IC_Over_Flag;
    // 定时器2输入捕获的占空比
    uint8_t ucTimer2_IC_Duty;
    // 定时器2输入捕获的频率
    uint32_t ulTimer2_IC_Fre;
    void (*vTIMER5_init)(uint16_t,uint16_t);
    void (*vTIMER1_Pwm_Init)(uint16_t,uint16_t,uint16_t,uint16_t,uint16_t,uint16_t);
    void (*vTIMER2_Init)(void);
    void (*vTIMER0_Pwn_Init)(uint16_t,uint16_t,uint16_t,uint16_t,uint16_t);
}MyTIMER_TypeDef;


extern MyTIMER_TypeDef MyTimer;

void vTIMER5_init(uint16_t psc,uint16_t arr);
void vTIMER1_Pwm_Init(uint16_t psc,uint16_t arr,uint16_t ch0_duty,uint16_t ch1_duty,uint16_t ch2_duty,uint16_t ch3_duty);
void vTIMER2_Init(void);
void vTIMER0_Pwn_Init(uint16_t psc,uint16_t arr,uint16_t ch0_duty,uint16_t ch1_duty,uint16_t ch2_duty);
#endif