/*
*@Description: 头文件汇总
*@Author: Yang
*@Date: 2023-05-09 07:55:52
*/
#ifndef __ALLHEAD_H
#define __ALLHEAD_H
#include "gd32f10x.h"
#include "systick.h"
#include "main.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "myLED.h"
#include "myKEY.h"
#include "myUart1.h"
#include "myTIMER.h"
#include "myADC.h"
#include "myRTC.h"
#include "myDOG.h"


#define BITBAND(addr, bitnum) ((addr & 0xF0000000) + 0x2000000 + ((addr & 0xFFFFF)<<5)+ (bitnum<<2))
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr))
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum))
#define GPIOC_ODR_Addr    (GPIOC+12)
#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)

#endif