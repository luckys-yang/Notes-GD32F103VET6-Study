/*
 *@Description: GD32ѧϰ
 *@Author: Yang
 *@Date: 2023-05-08 11:19:41
 */

#include "gd32f10x.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "AllHead.h"

/*====================================������ BEGIN====================================*/
// ΨһID
uint8_t Sys_Id[12];
/*====================================������    END====================================*/

/*====================================��̬�ڲ����������� BEGIN====================================*/
static void Hardware_Init(void);
static void svSYS_Id_Check(void);
static void svFLASH_Test(uint32_t InData);
/*====================================��̬�ڲ�����������    END====================================*/

void SysTick_Handler(void)
{
    // �Դ�����Ҫ��
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
 * @description: Ӳ����ʼ��
 * @return {*}
 * @Date: 2023-05-09 09:27:24
 */
// Ӳ����ʼ��
static void Hardware_Init(void)
{
    // ����ϵͳʱ��
    systick_config();
    // LED��ʼ��
    MyLed.vLED_Init();
    // ������ʼ��
    MyKey.vKEY_Init();
    // ����1��ʼ��
    MyUart0.vUART0_Init();
    // ��ʱ��5��ʼ��(��ʱ1ms)
    MyTimer.vTIMER5_init(107,999);
    // ��ʱ��1��ʼ��(Ƶ��2KHz,ռ�ձȷֱ���20%,45%,70%,90%)
    MyTimer.vTIMER1_Pwm_Init(107,499,100,225,350,450);
    // ��ʱ��2��ʼ��(PWM���벶��)
    MyTimer.vTIMER2_Init();
    // �ⲿ�жϳ�ʼ��
    MyKey.vKEY_Exti_Init();
    // ��ʱ��0��ʼ��(Ƶ��20KHz��ռ�ձȷֱ���20%,45%,70%)
    // MyTimer.vTIMER0_Pwn_Init(107,499,100,225,350);
    // ADC��ʼ��
    MyADC.vADC_Init();
    // RTC��ʼ��
    MyRTC.vRTC_Init();
    // ���ӳ�ʼ������
    MyRTC.vRTC_Alarm_Create();
    //  �������Ź���ʼ��
    MyDog.vDOG_Fwdgt_Init();
    // ���ڿ��Ź���ʼ��
    MyDog.vDOG_Wwdgt_Init();
    // ID���
    svSYS_Id_Check();
    // Flash����
    svFLASH_Test(12345678);
    printf("��ʼ�����\r\n");
}

/*
 * @description: оƬID���
 * @return {*}
 * @Date: 2023-05-16 11:23:45
 */
// оƬID���
static void svSYS_Id_Check(void)
{
    for(uint8_t i = 0; i < 12; i++)
    {
        Sys_Id[i] = *(uint8_t*)(0x1FFFF7E8 + i);
        // �ҵ��ǣ�25 7b 48 53 39 36 32 04 50 36 31 33
        // printf("%0.2x ",Sys_Id[i]);
    }
    // �ж�
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
 * @description: Flash��д����
 * @param {uint32_t} Data
 * @return {*}
 * @Date: 2023-05-16 12:13:40
 */
// Flash��д����
static void svFLASH_Test(uint32_t InData)
{
    #define FLASH_ADD 0x0807F800

    uint32_t OutData = 0;
    // ����
    fmc_unlock();
    // �����־λ
    fmc_flag_clear(FMC_FLAG_BANK0_PGERR | FMC_FLAG_BANK0_WPERR | FMC_FLAG_BANK0_END | FMC_FLAG_BANK1_PGERR | FMC_FLAG_BANK1_WPERR | FMC_FLAG_BANK1_END);
    // Ҫ����ҳ��ʼ��ַ
    fmc_page_erase(FLASH_ADD);
    // д����
    fmc_word_program(FLASH_ADD,InData);
    // ����
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
