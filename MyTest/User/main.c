/*
 *@Description: GD32学习
 *@Author: Yang
 *@Date: 2023-05-08 11:19:41
 */

#include "gd32f10x.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "AllHead.h"

/*====================================变量区 BEGIN====================================*/
// 唯一ID
uint8_t Sys_Id[12];
/*====================================变量区    END====================================*/

/*====================================静态内部函数声明区 BEGIN====================================*/
static void Hardware_Init(void);
static void svSYS_Id_Check(void);
static void svFLASH_Test(uint32_t InData);
/*====================================静态内部函数声明区    END====================================*/

void SysTick_Handler(void)
{
    // 自带不需要管
    delay_decrement();
}

int main(void)
{
    Hardware_Init();

    while (1)
    {
        // MyKey.vKey_Sacn();
        MyKey.vKEY_Comply_Function();
        // MyUart0.vUSART0_Data_Process();
    }
}

/*
 * @description: 硬件初始化
 * @return {*}
 * @Date: 2023-05-09 09:27:24
 */
// 硬件初始化
static void Hardware_Init(void)
{
    // 配置系统时钟
    systick_config();
    // LED初始化
    MyLed.vLED_Init();
    // 按键初始化
    MyKey.vKEY_Init();
    // 串口1初始化
    MyUart0.vUART0_Init();
    // 定时器5初始化(定时1ms)
    MyTimer.vTIMER5_init(107,999);
    // 定时器1初始化(频率2KHz,占空比分别是20%,45%,70%,90%)
    MyTimer.vTIMER1_Pwm_Init(107,499,100,225,350,450);
    // 定时器2初始化(PWM输入捕获)
    MyTimer.vTIMER2_Init();
    // 外部中断初始化
    MyKey.vKEY_Exti_Init();
    // 定时器0初始化(频率20KHz，占空比分别是20%,45%,70%)
    // MyTimer.vTIMER0_Pwn_Init(107,499,100,225,350);
    // ADC初始化
    MyADC.vADC_Init();
    // RTC初始化
    MyRTC.vRTC_Init();
    // 闹钟初始化启动
    MyRTC.vRTC_Alarm_Create();
    //  独立看门狗初始化
    MyDog.vDOG_Fwdgt_Init();
    // 窗口看门狗初始化
    MyDog.vDOG_Wwdgt_Init();
    // ID检查
    svSYS_Id_Check();
    // Flash测试
    svFLASH_Test(12345678);
    printf("初始化完成\r\n");
}

/*
 * @description: 芯片ID检查
 * @return {*}
 * @Date: 2023-05-16 11:23:45
 */
// 芯片ID检查
static void svSYS_Id_Check(void)
{
    for(uint8_t i = 0; i < 12; i++)
    {
        Sys_Id[i] = *(uint8_t*)(0x1FFFF7E8 + i);
        // 我的是：25 7b 48 53 39 36 32 04 50 36 31 33
        // printf("%0.2x ",Sys_Id[i]);
    }
    // 判断
    if(0x25 == Sys_Id[0] && 0x7b == Sys_Id[1] && 0x48 == Sys_Id[2] && 0x53 == Sys_Id[3] &&
        0x39 == Sys_Id[4] && 0x36 == Sys_Id[5] && 0x32 == Sys_Id[6] && 0x04 == Sys_Id[7] &&
        0x50 == Sys_Id[8] && 0x36 == Sys_Id[9] && 0x31 == Sys_Id[10] && 0x33 == Sys_Id[11])
    {
        printf("\r\nPASS\r\n");
    }
    else
    {
        printf("\r\nFAIL\r\n");
        while(1);
    }
}

/*
 * @description: Flash读写测试
 * @param {uint32_t} Data
 * @return {*}
 * @Date: 2023-05-16 12:13:40
 */
// Flash读写测试
static void svFLASH_Test(uint32_t InData)
{
    #define FLASH_ADD 0x0807F800

    uint32_t OutData = 0;
    // 解锁
    fmc_unlock();
    // 清除标志位
    fmc_flag_clear(FMC_FLAG_BANK0_PGERR | FMC_FLAG_BANK0_WPERR | FMC_FLAG_BANK0_END | FMC_FLAG_BANK1_PGERR | FMC_FLAG_BANK1_WPERR | FMC_FLAG_BANK1_END);
    // 要擦除页起始地址
    fmc_page_erase(FLASH_ADD);
    // 写数据
    fmc_word_program(FLASH_ADD,InData);
    // 锁定
    fmc_lock();
    OutData = (*(__IO uint32_t*)(FLASH_ADD));
    printf("InData:%d--OutData:%d\r\n",InData,OutData);
    if(InData == OutData)
    {
        printf("Flash PASS\r\n");
    }
    else
    {
        printf("Flash FAIL\r\n");
    }
}
