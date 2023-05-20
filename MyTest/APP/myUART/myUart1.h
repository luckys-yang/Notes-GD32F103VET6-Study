/*
*@Description: 串口1
*@Author: Yang
*@Date: 2023-05-09 21:06:37
*/
#ifndef __MYUART1_H
#define __MYUART1_H
#include "AllHead.h"

// 重映射需要打开这个宏
// #define USART0_REMAP
// 串口0接收的最大长度
#define UART0_MAX_LEN   50


typedef struct
{
    // 串口0接收数据完成标志位
    bool bUart0_Rx_Over_Flag;
    // 串口0接收的数据长度
    uint8_t ucUart0_Rx_Len;
    // 串口0接收数组
    uint8_t ucUart0_Rx_Buff[UART0_MAX_LEN];
    void (*vUART0_Init)(void);
    void (*vUSART0_Data_Process)(void);
}MyUart0_TypeDef;


extern MyUart0_TypeDef MyUart0;

void vUART0_Init(void);
int fputc(int ch,FILE *f);
void vUSART0_Data_Process(void);
#endif