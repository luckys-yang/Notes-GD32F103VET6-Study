#ifndef __MYRTC_H
#define __MYRTC_H
#include "AllHead.h"

typedef struct
{
    // 闹钟触发标志位
    bool bRTC_Alarm_Flag;
    // 秒中断标志位
    bool bRTC_Sec_Flag;
    uint16_t ucYear;
    uint16_t ucMon;
    uint16_t ucDay;
    uint16_t ucHour;
    uint16_t ucMin;
    uint16_t ucSecond;
    void (*vRTC_Init)(void);
    void (*vRTC_Convert_Time)(uint32_t);
    void (*vRTC_Alarm_Create)(void);
}MyRTC_TypeDef;

extern MyRTC_TypeDef MyRTC;

void vRTC_Init(void);
void vRTC_Convert_Time(uint32_t timestamp_value);
void vRTC_Alarm_Create(void);
#endif