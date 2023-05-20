#include "myDOG.h"

/*====================================������ BEGIN====================================*/
MyDog_TypeDef MyDog = 
{
    .vDOG_Fwdgt_Init = &vDOG_Fwdgt_Init,
    .vDOG_Wwdgt_Init = &vDOG_Wwdgt_Init
};
/*====================================������    END====================================*/

/*
 * @description: �������Ź���ʼ��
 * @return {*}
 * @Date: 2023-05-16 08:06:29
 */
// �������Ź���ʼ��
void vDOG_Fwdgt_Init(void)
{
    // ʹ��д����
    fwdgt_write_enable();
    // ����Ԥ��Ƶ 40000/64=625Hz��1/625 = 1.6ms
    // ���ó�ֵ 800*1.6=1.28s
    fwdgt_config(800,FWDGT_PSC_DIV64);
    // ι��
    fwdgt_counter_reload();
    // ʹ�ܶ������Ź�
    fwdgt_enable();
}

/*
 * @description: ���ڿ��Ź���ʼ��
 * @return {*}
 * @Date: 2023-05-16 08:06:26
 */
// ���ڿ��Ź���ʼ��
void vDOG_Wwdgt_Init(void)
{
    // ʹ�ܴ��ڿ��Ź�ʱ��
    rcu_periph_clock_enable(RCU_WWDGT);
    // ��������ֵ
    wwdgt_config(0x7F,0x5F,WWDGT_CFG_PSC_DIV8);
    // ʹ�ܴ��ڿ��Ź�
    wwdgt_enable();
    // ����жϱ�־
    wwdgt_flag_clear();
    // �����ж�
    wwdgt_interrupt_enable();
    nvic_irq_enable(WWDGT_IRQn,2,0);
}

/*
 * @description: ���ڿ��Ź��жϺ���
 * @return {*}
 * @Date: 2023-05-16 08:14:18
 */
// ���ڿ��Ź��жϺ���
void WWDGT_IRQHandler(void)
{
    wwdgt_counter_update(0x7F);
    wwdgt_flag_clear();
    // printf("WWDG\r\n");
}