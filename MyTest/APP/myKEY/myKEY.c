#include "myKEY.h"

/*====================================������ BEGIN====================================*/
MyKey_TypeDef MyKey = 
{
    .Key_Down_State = {0},
    .Key_Long_Time = 0,
    .vKEY_Init = &vKEY_Init,
    .vKey_Sacn = &vKey_Sacn,
    .vKEY_Comply_Function = &vKEY_Comply_Function,
    .vKEY_Exti_Init = &vKEY_Exti_Init
};

// ����һ˲��
uint8_t Key_Down;
// ̧��һ˲��
uint8_t Key_Up;
// ��ֵ
uint8_t Key_Value;


/*====================================������    END====================================*/

/*====================================��̬�ڲ����������� BEGIN====================================*/
static uint8_t sucKEY_Get_Value(void);
/*====================================��̬�ڲ�����������    END====================================*/

/*
 * @description: ������ʼ��
 * @return {*}
 * @Date: 2023-05-09 09:46:13
 */
// ������ʼ��
void vKEY_Init(void)
{
    // ʹ��ʱ��
    rcu_periph_clock_enable(RCU_GPIOA);
    // ��ʼ������
    gpio_init(GPIOA,GPIO_MODE_IPU,GPIO_OSPEED_50MHZ,GPIO_PIN_15);
}


/*
 * @description: ��ȡ��ֵ
 * @return {*}
 * @Date: 2023-05-09 09:42:20
 */
// ��ȡ��ֵ
static uint8_t sucKEY_Get_Value(void)
{
    if(!KEY1)
    {
        return KEY1_VALUE;
    }
    return KEY_NULL;
}

/*
 * @description: ����ɨ��
 * @return {*}
 * @Date: 2023-05-09 09:51:52
 */
// ����ɨ��
void vKey_Sacn(void)
{
    static uint8_t Key_Old;

    Key_Value = sucKEY_Get_Value();
    Key_Down = Key_Value & (Key_Old ^ Key_Value);
    Key_Up = ~Key_Value & (Key_Old ^ Key_Value);
    Key_Old = Key_Value;

    if(Key_Down)
    {
        MyKey.Key_Long_Time = 0;
    }

    if(MyKey.Key_Long_Time < 10)
    {
        switch(Key_Up)
        {
            case 1:
            {
                MyKey.Key_Down_State[0] = 1;
                break;
            }
            default:break;
        }
    }
    else
    {
        switch(Key_Down)
        {
            case 1:
            {
                MyKey.Key_Down_State[1] = 1;
                break;
            }
            default:break;
        }
    }
}

/*
 * @description: ��������ִ��
 * @return {*}
 * @Date: 2023-05-09 09:59:38
 */
// ��������ִִ��
void vKEY_Comply_Function(void)
{
    if(MyKey.Key_Down_State[0])
    {
        MyKey.Key_Down_State[0] = 0;
        printf("����1����\r\n");
        // MyLed.bLed1_State = !MyLed.bLed1_State;
        // MyLed.vLED_Control(MyLed.bLed1_State);
    }
}

/*
 * @description: �����ⲿ�жϳ�ʼ��
 * @return {*}
 * @Date: 2023-05-13 20:28:22
 */
// �����ⲿ�жϳ�ʼ��
void vKEY_Exti_Init(void)
{
    // ʹ��GPIOʱ��
    rcu_periph_clock_enable(RCU_GPIOA);
    // ��ʼ������
    gpio_init(GPIOA,GPIO_MODE_IPU,GPIO_OSPEED_50MHZ,GPIO_PIN_15);
    // ʹ�ܸ���ʱ��
    rcu_periph_clock_enable(RCU_AF);
    // �������ȼ�---15
    nvic_irq_enable(EXTI10_15_IRQn,2,2);
    // �����ж���---PA15
    gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOA,GPIO_PIN_SOURCE_15);
    // �����ж��ߺ��жϱ���---15�½��ش���
    exti_init(EXTI_15,EXTI_INTERRUPT,EXTI_TRIG_FALLING);
    // ����жϱ��
    exti_interrupt_flag_clear(EXTI_15);
}

/*
 * @description: �ⲿ�ж�--Line15����
 * @return {*}
 * @Date: 2023-05-13 20:51:42
 */
// �ⲿ�ж�--Line15����
void EXTI10_15_IRQHandler(void)
{
    if(SET == exti_interrupt_flag_get(EXTI_15))
    {
        MyKey.Key_Down_State[0] = 1;
        // �����־λ
        exti_interrupt_flag_clear(EXTI_15);
    }
}