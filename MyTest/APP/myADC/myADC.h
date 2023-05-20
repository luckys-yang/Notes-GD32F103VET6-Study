/*
*@Description: 
*@Author: Yang
*@Date: 2023-05-14 17:02:00
*/
#ifndef __MYADC_H
#define __MYADC_H
#include "AllHead.h"


typedef struct 
{
    // ADC DMA 中断标志位
    bool bADC_Dma_Flag;
    // ADC存储值
    uint32_t ulADC_Value[4];    
    // ADC开始采集标志位
    bool bADC_Start_Flag;
    void (*vADC_Init)(void);
    uint16_t (*usADC_Get_Value)(uint8_t);
}MyADC_TypeDef;

extern MyADC_TypeDef MyADC;

void vADC_Init(void);
uint16_t usADC_Get_Value(uint8_t channel);
#endif