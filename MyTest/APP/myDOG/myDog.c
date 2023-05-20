#include "myDOG.h"

/*====================================变量区 BEGIN====================================*/
MyDog_TypeDef MyDog = 
{
    .vDOG_Fwdgt_Init = &vDOG_Fwdgt_Init,
    .vDOG_Wwdgt_Init = &vDOG_Wwdgt_Init
};
/*====================================变量区    END====================================*/

/*
 * @description: 独立看门狗初始化
 * @return {*}
 * @Date: 2023-05-16 08:06:29
 */
// 独立看门狗初始化
void vDOG_Fwdgt_Init(void)
{
    // 使能写功能
    fwdgt_write_enable();
    // 设置预分频 40000/64=625Hz，1/625 = 1.6ms
    // 设置初值 800*1.6=1.28s
    fwdgt_config(800,FWDGT_PSC_DIV64);
    // 喂狗
    fwdgt_counter_reload();
    // 使能独立看门狗
    fwdgt_enable();
}

/*
 * @description: 窗口看门狗初始化
 * @return {*}
 * @Date: 2023-05-16 08:06:26
 */
// 窗口看门狗初始化
void vDOG_Wwdgt_Init(void)
{
    // 使能窗口看门狗时钟
    rcu_periph_clock_enable(RCU_WWDGT);
    // 窗口上限值
    wwdgt_config(0x7F,0x5F,WWDGT_CFG_PSC_DIV8);
    // 使能窗口看门狗
    wwdgt_enable();
    // 清除中断标志
    wwdgt_flag_clear();
    // 开启中断
    wwdgt_interrupt_enable();
    nvic_irq_enable(WWDGT_IRQn,2,0);
}

/*
 * @description: 窗口看门狗中断函数
 * @return {*}
 * @Date: 2023-05-16 08:14:18
 */
// 窗口看门狗中断函数
void WWDGT_IRQHandler(void)
{
    wwdgt_counter_update(0x7F);
    wwdgt_flag_clear();
    // printf("WWDG\r\n");
}