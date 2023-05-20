/*
*@Description: ��ʱ��
*@Author: Yang
*@Date: 2023-05-11 19:42:10
*/
#include "myTIMER.h"

/*====================================������ BEGIN====================================*/
MyTIMER_TypeDef MyTimer = 
{
    .bTimer2_IC_Over_Flag = 0,
    .ucTimer2_IC_Duty = 0,
    .ulTimer2_IC_Fre = 0,
    .vTIMER5_init = &vTIMER5_init,
    .vTIMER1_Pwm_Init = &vTIMER1_Pwm_Init,
    .vTIMER2_Init = &vTIMER2_Init,
    .vTIMER0_Pwn_Init = &vTIMER0_Pwn_Init
};
/*====================================������    END====================================*/


/*
 * @description: ��ʱ��5��ʼ��
 * @param {uint16_t} psc Ԥ��Ƶֵ
 * @param {uint16_t} arr ��װ��ֵ
 * @return {*}
 * @Date: 2023-05-11 21:27:05
 */
// ��ʱ��5��ʼ��
void vTIMER5_init(uint16_t psc,uint16_t arr)
{
    timer_parameter_struct myTIMER5;
    // ������ʱ��ʱ��
    rcu_periph_clock_enable(RCU_TIMER5);
    // �ṹ�帴λ��ʼ��
    timer_deinit(TIMER5);
    // ��ʼ����ʱ���ṹ��
    timer_struct_para_init(&myTIMER5);
    // Ԥ��Ƶ--pscֵ
    myTIMER5.prescaler = psc;
    // ����ģʽ
    myTIMER5.alignedmode = TIMER_COUNTER_EDGE;
    // ��������--���ϼ���
    myTIMER5.counterdirection = TIMER_COUNTER_UP;
    // ��װ��ֵ--arr
    myTIMER5.period = arr;
    // ʱ�ӷ�Ƶ����--����Ƶ
    myTIMER5.clockdivision = TIMER_CKDIV_DIV1;
    // �������ظ���������-- 0~255(�߼���ʱ������)
    myTIMER5.repetitioncounter = 0;
    // ��ʼ��
    timer_init(TIMER5,&myTIMER5);
    // ��ʽ����жϱ�־λ
    timer_interrupt_flag_clear(TIMER5,TIMER_INT_FLAG_UP);
    // �����ж�
    timer_interrupt_enable(TIMER5, TIMER_INT_UP);
    // �����жϺ��������ȼ�
    nvic_irq_enable(TIMER5_IRQn,0,0);
    // ʹ��
    timer_enable(TIMER5);
}


/*
 * @description: ��ʱ��5�жϺ���
 * @return {*}
 * @Date: 2023-05-12 08:40:32
 */
// ��ʱ��5�жϺ���
void TIMER5_IRQHandler(void)
{
    if(SET == timer_interrupt_flag_get(TIMER5, TIMER_INT_UP))
    {
        // �����־λ
        timer_interrupt_flag_clear(TIMER5, TIMER_INT_UP);

        static uint16_t Led_Cnt = 0;
        static uint16_t Adc_Cnt = 0;
        static uint16_t Dog_Fwdgt_Cnt = 0;

        Led_Cnt++;
        Adc_Cnt++;
        Dog_Fwdgt_Cnt++;
        if(100 == Led_Cnt)
        {
            Led_Cnt = 0;
            MyLed.bLed1_State = !MyLed.bLed1_State;
            MyLed.vLED_Control(MyLed.bLed1_State);
        }
        if(200 == Adc_Cnt)
        {
            Adc_Cnt = 0;
            MyADC.bADC_Start_Flag = 1;
        }
        if(800 == Dog_Fwdgt_Cnt)
        {
            Dog_Fwdgt_Cnt = 0;
            // ι��
            fwdgt_counter_reload();
        }
    }
}


/*
 * @description: ��ʱ��1PWM��ʼ��
 * @param {uint16_t} psc Ԥ��Ƶ
 * @param {uint16_t} arr ��װ��ֵ
 * @param {uint16_t} ch0_duty ͨ��0ռ�ձ�
 * @param {uint16_t} ch1_duty ͨ��1ռ�ձ�
 * @param {uint16_t} ch2_duty ͨ��2ռ�ձ�
 * @param {uint16_t} ch3_duty ͨ��3ռ�ձ�
 * @return {*}
 * @Date: 2023-05-12 15:52:50
 */
// ��ʱ��1PWM��ʼ��
void vTIMER1_Pwm_Init(uint16_t psc,uint16_t arr,uint16_t ch0_duty,uint16_t ch1_duty,uint16_t ch2_duty,uint16_t ch3_duty)
{
    timer_parameter_struct myTIMER1;
    timer_oc_parameter_struct myTIMER1_OC;

    // ������ʱ��ʱ�Ӻ͸���ʱ��
    rcu_periph_clock_enable(RCU_TIMER1);
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
    // �ṹ�帴λ��ʼ��
    timer_deinit(TIMER1);

    // ��ʼ����ʱ���ṹ��
    timer_struct_para_init(&myTIMER1);
    // Ԥ��Ƶ--pscֵ
    myTIMER1.prescaler = psc;
    // ����ģʽ
    myTIMER1.alignedmode = TIMER_COUNTER_EDGE;
    // ��������--���ϼ���
    myTIMER1.counterdirection = TIMER_COUNTER_UP;
    // ��װ��ֵ--arr
    myTIMER1.period = arr;
    // ʱ�ӷ�Ƶ����--����Ƶ
    myTIMER1.clockdivision = TIMER_CKDIV_DIV1;
    // ��ʼ��
    timer_init(TIMER1,&myTIMER1);

    // ͨ��ʹ��
    myTIMER1_OC.outputstate = TIMER_CCX_ENABLE;
    // ͨ������--�ߵ�ƽ��Ч
    myTIMER1_OC.ocpolarity = TIMER_OC_POLARITY_HIGH;
    // ----------------ͨ��0����----------------
    // ����TIMERx��ͨ���������
    timer_channel_output_config(TIMER1,TIMER_CH_0,&myTIMER1_OC);
    // ͨ��ռ�ձ�����
    timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_0,ch0_duty);
    // ͨ��ģʽ---PWM0ģʽ
    timer_channel_output_mode_config(TIMER1,TIMER_CH_0,TIMER_OC_MODE_PWM0);
    // ��ʹ������Ƚ�Ӱ�ӼĴ���
    timer_channel_output_shadow_config(TIMER1,TIMER_CH_0,TIMER_OC_SHADOW_DISABLE);
    // ----------------ͨ��1����----------------
    // ����TIMERx��ͨ���������
    timer_channel_output_config(TIMER1,TIMER_CH_1,&myTIMER1_OC);
    // ͨ��ռ�ձ�����
    timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_1,ch1_duty);
    // ͨ��ģʽ---PWM0ģʽ
    timer_channel_output_mode_config(TIMER1,TIMER_CH_1,TIMER_OC_MODE_PWM0);
    // ��ʹ������Ƚ�Ӱ�ӼĴ���
    timer_channel_output_shadow_config(TIMER1,TIMER_CH_1,TIMER_OC_SHADOW_DISABLE);
    // ----------------ͨ��2����----------------
    // ����TIMERx��ͨ���������
    timer_channel_output_config(TIMER1,TIMER_CH_2,&myTIMER1_OC);
    // ͨ��ռ�ձ�����
    timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_2,ch2_duty);
    // ͨ��ģʽ---PWM0ģʽ
    timer_channel_output_mode_config(TIMER1,TIMER_CH_2,TIMER_OC_MODE_PWM0);
    // ��ʹ������Ƚ�Ӱ�ӼĴ���
    timer_channel_output_shadow_config(TIMER1,TIMER_CH_2,TIMER_OC_SHADOW_DISABLE);
    // ----------------ͨ��3����----------------
    // ����TIMERx��ͨ���������
    timer_channel_output_config(TIMER1,TIMER_CH_3,&myTIMER1_OC);
    // ͨ��ռ�ձ�����
    timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_3,ch3_duty);
    // ͨ��ģʽ---PWM0ģʽ
    timer_channel_output_mode_config(TIMER1,TIMER_CH_3,TIMER_OC_MODE_PWM0);
    // ��ʹ������Ƚ�Ӱ�ӼĴ���
    timer_channel_output_shadow_config(TIMER1,TIMER_CH_3,TIMER_OC_SHADOW_DISABLE);
    // ʹ���Զ���װ��
    timer_auto_reload_shadow_enable(TIMER1);
    // ʹ�ܶ�ʱ��1
    timer_enable(TIMER1);
}

/*
 * @description: ��ʱ��2��ʼ��PWM����
 * @return {*}
 * @Date: 2023-05-12 21:43:32
 */
// ��ʱ��2��ʼ��PWM����
void vTIMER2_Init(void)
{
    timer_parameter_struct myTIMER2;
    timer_ic_parameter_struct myTIMER2_IC;

    // ʹ��ʱ��
    rcu_periph_clock_enable(RCU_GPIOA);
    // ʹ�ܸ���ʱ��
    rcu_periph_clock_enable(RCU_AF);
    // ���� PA6ΪCH0�ĸ���ģʽ
    gpio_init(GPIOA,GPIO_MODE_IN_FLOATING,GPIO_OSPEED_50MHZ,GPIO_PIN_6);
    // ʹ�ܶ�ʱ��2
    rcu_periph_clock_enable(RCU_TIMER2);
    // -----------------��ʱ������+�ж�����-----------------
   // �ṹ�帴λ��ʼ��
    timer_deinit(TIMER2);
    // Ԥ��Ƶ--pscֵ
    myTIMER2.prescaler = 107;
    // ����ģʽ
    myTIMER2.alignedmode = TIMER_COUNTER_EDGE;
    // ��������--���ϼ���
    myTIMER2.counterdirection = TIMER_COUNTER_UP;
    // ��װ��ֵ--arr(����Ϊ���ֵ����)
    myTIMER2.period = 65535;
    // ʱ�ӷ�Ƶ����--����Ƶ
    myTIMER2.clockdivision = TIMER_CKDIV_DIV1;
    // ��ʼ��
    timer_init(TIMER2,&myTIMER2);
    // ����жϱ�־---ͨ��0
    timer_interrupt_flag_clear(TIMER2,TIMER_INT_FLAG_CH0);
    // �����ж�
    timer_interrupt_enable(TIMER2, TIMER_INT_FLAG_CH0);
    // �����жϺ��������ȼ�
    nvic_irq_enable(TIMER2_IRQn,0,1);
    // -----------------��ʱ�����벶������-----------------
    // ͨ�����뼫��---������
    myTIMER2_IC.icpolarity = TIMER_IC_POLARITY_RISING;
    // ͨ������ģʽѡ��---ֱ��
    myTIMER2_IC.icselection = TIMER_IC_SELECTION_DIRECTTI;
    // ͨ�����벶��Ԥ��Ƶ---����Ƶ
    myTIMER2_IC.icprescaler = TIMER_IC_PSC_DIV1;
    // ͨ�����벶���˲�(0~15)
    myTIMER2_IC.icfilter = 0;
#if 0
    // �������벶�����
    timer_input_capture_config(TIMER2,TIMER_CH_0,&myTIMER2_IC);
    // �Զ���װ��ʹ��
    timer_auto_reload_shadow_enable(TIMER2);
#endif
    // ����PWM�������
    timer_input_pwm_capture_config(TIMER2,TIMER_CH_0,&myTIMER2_IC);
    // ���봥��Դѡ��---�˲����ͨ��0����
    timer_input_trigger_source_select(TIMER2,TIMER_SMCFG_TRGSEL_CI0FE0);
    // ��ģʽ����---��λģʽ
    timer_slave_mode_select(TIMER2,TIMER_SLAVE_MODE_RESTART);
    // ����ģʽ����---ʹ��
    timer_master_slave_mode_config(TIMER2,TIMER_MASTER_SLAVE_MODE_ENABLE);
    // �Զ���װ��ʹ��
    timer_auto_reload_shadow_enable(TIMER2);
    // ʹ��
    timer_enable(TIMER2);
}

/*
 * @description: ��ʱ��2�жϺ���
 * @return {*}
 * @Date: 2023-05-12 21:44:27
 */
// ��ʱ��2�жϺ���
void TIMER2_IRQHandler(void)
{
    static uint32_t Last_ic_Value = 0;
    uint32_t ic_Value = 0,ic_Value1 = 0,ic_Value2 = 0;
#if 0
    if(SET == timer_interrupt_flag_get(TIMER2,TIMER_INT_FLAG_CH0))
    {
        // ����жϱ�־λ
        timer_interrupt_flag_clear(TIMER2,TIMER_INT_FLAG_CH0);
        // ��ȡͨ������ֵ
        ic_Value = timer_channel_capture_value_register_read(TIMER2,TIMER_CH_0);
        // �����һ��ֵ����
        if(Last_ic_Value)
        {
            if(ic_Value > Last_ic_Value)
            {
                MyTimer.ulTimer2_IC_Fre = 1000000 / (ic_Value - Last_ic_Value);
            }
            else
            {
                // ˵���Ѿ��������Ҫ�������װ��ֵ(����16λ������0xFFFF��32λ��0xFFFFFFFF)
                MyTimer.ulTimer2_IC_Fre = 1000000 / (0xFFFF + ic_Value - Last_ic_Value);
            }
            MyTimer.bTimer2_IC_Over_Flag = 1;
        }
        Last_ic_Value = ic_Value;
    }
#endif
    if(SET == timer_interrupt_flag_get(TIMER2,TIMER_INT_FLAG_CH0))
    {
        // ����жϱ�־λ
        timer_interrupt_flag_clear(TIMER2,TIMER_INT_FLAG_CH0);
        // ��ȡͨ��0����ֵ---����ʱ��
        ic_Value1 = TIMER_CH0CV(TIMER2) + 1;
        if(ic_Value1 != 0)
        {
            // ��ȡͨ��1����ֵ---�ߵ�ƽʱ��
            ic_Value2 = TIMER_CH1CV(TIMER2) + 1;
            MyTimer.ulTimer2_IC_Fre = (float)1000000 / ic_Value1;
            MyTimer.ucTimer2_IC_Duty = (ic_Value2*100) / ic_Value1;
            MyTimer.bTimer2_IC_Over_Flag = 1;
        }
        else
        {
            MyTimer.ulTimer2_IC_Fre = 0;
            MyTimer.ucTimer2_IC_Duty = 0;
        }
    }
}

/*
 * @description: ��ʱ��0PWM��ʼ��
 * @return {*}
 * @Date: 2023-05-13 22:00:00
 */
// ��ʱ��0PWM��ʼ��
void vTIMER0_Pwn_Init(uint16_t psc,uint16_t arr,uint16_t ch0_duty,uint16_t ch1_duty,uint16_t ch2_duty)
{
    timer_parameter_struct myTIMER0;
    timer_oc_parameter_struct myTIMER0_OC;

    // ʹ��GPIOA��Bʱ��
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    // ʹ�ܸ���ʱ��
    rcu_periph_clock_enable(RCU_AF);
    rcu_periph_clock_enable(RCU_TIMER0);
    // PWM������� ��ʼ��
    gpio_init(GPIOA,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_8);
    gpio_init(GPIOA,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_9);
    gpio_init(GPIOA,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_10);
    // PWM����������� ��ʼ��
    gpio_init(GPIOB,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_12); // ɲ������
    gpio_init(GPIOB,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_13);
    gpio_init(GPIOB,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_14);
    gpio_init(GPIOB,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_15);
    //------------------��ʱ����������--------------------
    // ��ʼ����ʱ���ṹ��
    timer_struct_para_init(&myTIMER0);
    timer_deinit(TIMER0);
    // Ԥ��Ƶ
    myTIMER0.prescaler = psc;
    // ����ģʽ
    myTIMER0.alignedmode = TIMER_COUNTER_EDGE;
    // ��������
    myTIMER0.counterdirection = TIMER_COUNTER_UP;
    // ��װ��ֵ
    myTIMER0.period = arr;
    // ��Ƶ����
    myTIMER0.clockdivision = TIMER_CKDIV_DIV1;
    myTIMER0.repetitioncounter = 0;
    timer_init(TIMER0,&myTIMER0);
    // ---------------��ʱ��PWM�������------------------
    // ���ʹ��
    myTIMER0_OC.outputstate = TIMER_CCX_ENABLE;
    // �������ʹ��
    myTIMER0_OC.outputnstate = TIMER_CCXN_ENABLE;
    // �������---��
    myTIMER0_OC.ocpolarity = TIMER_OC_POLARITY_HIGH;
    // �����������---��
    myTIMER0_OC.ocnpolarity = TIMER_OCN_POLARITY_HIGH;
    // ���������ƽ---��
    myTIMER0_OC.ocidlestate = TIMER_OC_IDLE_STATE_LOW;
    // �������������ƽ---��
    myTIMER0_OC.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
    // ����TIMERx��ͨ���������
    timer_channel_output_config(TIMER0,TIMER_CH_0,&myTIMER0_OC);
    timer_channel_output_config(TIMER0,TIMER_CH_1,&myTIMER0_OC);
    timer_channel_output_config(TIMER0,TIMER_CH_2,&myTIMER0_OC);
    // --------ͨ��0----------
    // ͨ��ռ�ձ�����
    timer_channel_output_pulse_value_config(TIMER0,TIMER_CH_0,ch0_duty);
    // ͨ��ģʽ---PWM0ģʽ
    timer_channel_output_mode_config(TIMER0,TIMER_CH_0,TIMER_OC_MODE_PWM0);
    // ��ʹ������Ƚ�Ӱ�ӼĴ���
    timer_channel_output_shadow_config(TIMER0,TIMER_CH_0,TIMER_OC_SHADOW_DISABLE);    
    // --------ͨ��1----------
    // ͨ��ռ�ձ�����
    timer_channel_output_pulse_value_config(TIMER0,TIMER_CH_1,ch1_duty);
    // ͨ��ģʽ---PWM0ģʽ
    timer_channel_output_mode_config(TIMER0,TIMER_CH_1,TIMER_OC_MODE_PWM0);
    // ��ʹ������Ƚ�Ӱ�ӼĴ���
    timer_channel_output_shadow_config(TIMER0,TIMER_CH_1,TIMER_OC_SHADOW_DISABLE);    
    // --------ͨ��2----------
    // ͨ��ռ�ձ�����
    timer_channel_output_pulse_value_config(TIMER0,TIMER_CH_2,ch2_duty);
    // ͨ��ģʽ---PWM0ģʽ
    timer_channel_output_mode_config(TIMER0,TIMER_CH_2,TIMER_OC_MODE_PWM0);
    // ��ʹ������Ƚ�Ӱ�ӼĴ���
    timer_channel_output_shadow_config(TIMER0,TIMER_CH_2,TIMER_OC_SHADOW_DISABLE);            
    // �߼���ʱ����Ҫ��
    timer_primary_output_config(TIMER0,ENABLE);
    // �Զ���װ��ʹ��
    timer_auto_reload_shadow_enable(TIMER0);
    // -------------------ɲ������-------------------    
    timer_break_parameter_struct myTIMER0_Break;
    // ����ģʽ�¡��ر�״̬������---ʹ��
    myTIMER0_Break.runoffstate = TIMER_ROS_STATE_ENABLE;
    // ����ģʽ�¡��ر�״̬������---ʹ��
    myTIMER0_Break.ideloffstate = TIMER_IOS_STATE_ENABLE;
    // ����ʱ��(0~255)
    myTIMER0_Break.deadtime = 255;
    // ��ֹ�źż���---�ߵ�ƽ(����ɲ����ƽ)
    myTIMER0_Break.breakpolarity = TIMER_BREAK_POLARITY_HIGH;
    // �Զ����ʹ��---ʹ��
    myTIMER0_Break.outputautostate = TIMER_OUTAUTO_ENABLE;
    // �����Ĵ�����������
    myTIMER0_Break.protectmode = TIMER_CCHP_PROT_0;
    // ��ֹʹ��---ʹ��
    myTIMER0_Break.breakstate = TIMER_BREAK_ENABLE;
    // ɲ������
    timer_break_config(TIMER0,&myTIMER0_Break);
    // Ĭ�ϵ͵�ƽ
    gpio_bit_write(GPIOB,GPIO_PIN_12,RESET);
    // ʹ�ܶ�ʱ��
    timer_enable(TIMER0);
}