/*
*@Description: ����1
*@Author: Yang
*@Date: 2023-05-09 21:06:37
*/
#ifndef __MYUART1_H
#define __MYUART1_H
#include "AllHead.h"

// ��ӳ����Ҫ�������
// #define USART0_REMAP
// ����0���յ���󳤶�
#define UART0_MAX_LEN   50


typedef struct
{
    // ����0����������ɱ�־λ
    bool bUart0_Rx_Over_Flag;
    // ����0���յ����ݳ���
    uint8_t ucUart0_Rx_Len;
    // ����0��������
    uint8_t ucUart0_Rx_Buff[UART0_MAX_LEN];
    void (*vUART0_Init)(void);
    void (*vUSART0_Data_Process)(void);
}MyUart0_TypeDef;


extern MyUart0_TypeDef MyUart0;

void vUART0_Init(void);
int fputc(int ch,FILE *f);
void vUSART0_Data_Process(void);
#endif