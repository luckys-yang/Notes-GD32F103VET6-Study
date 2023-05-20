#include "myKEY.h"

/*====================================变量区 BEGIN====================================*/
MyKey_TypeDef MyKey = 
{
    .Key_Down_State = {0},
    .Key_Long_Time = 0,
    .vKEY_Init = &vKEY_Init,
    .vKey_Sacn = &vKey_Sacn,
    .vKEY_Comply_Function = &vKEY_Comply_Function,
    .vKEY_Exti_Init = &vKEY_Exti_Init
};

// 按下一瞬间
uint8_t Key_Down;
// 抬起一瞬间
uint8_t Key_Up;
// 键值
uint8_t Key_Value;


/*====================================变量区    END====================================*/

/*====================================静态内部函数声明区 BEGIN====================================*/
static uint8_t sucKEY_Get_Value(void);
/*====================================静态内部函数声明区    END====================================*/

/*
 * @description: 按键初始化
 * @return {*}
 * @Date: 2023-05-09 09:46:13
 */
// 按键初始化
void vKEY_Init(void)
{
    // 使能时钟
    rcu_periph_clock_enable(RCU_GPIOA);
    // 初始化引脚
    gpio_init(GPIOA,GPIO_MODE_IPU,GPIO_OSPEED_50MHZ,GPIO_PIN_15);
}


/*
 * @description: 获取键值
 * @return {*}
 * @Date: 2023-05-09 09:42:20
 */
// 获取键值
static uint8_t sucKEY_Get_Value(void)
{
    if(!KEY1)
    {
        return KEY1_VALUE;
    }
    return KEY_NULL;
}

/*
 * @description: 按键扫描
 * @return {*}
 * @Date: 2023-05-09 09:51:52
 */
// 按键扫描
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
 * @description: 按键功能执行
 * @return {*}
 * @Date: 2023-05-09 09:59:38
 */
// 按键功能执执行
void vKEY_Comply_Function(void)
{
    if(MyKey.Key_Down_State[0])
    {
        MyKey.Key_Down_State[0] = 0;
        printf("按键1按下\r\n");
        // MyLed.bLed1_State = !MyLed.bLed1_State;
        // MyLed.vLED_Control(MyLed.bLed1_State);
    }
}

/*
 * @description: 按键外部中断初始化
 * @return {*}
 * @Date: 2023-05-13 20:28:22
 */
// 按键外部中断初始化
void vKEY_Exti_Init(void)
{
    // 使能GPIO时钟
    rcu_periph_clock_enable(RCU_GPIOA);
    // 初始化引脚
    gpio_init(GPIOA,GPIO_MODE_IPU,GPIO_OSPEED_50MHZ,GPIO_PIN_15);
    // 使能复用时钟
    rcu_periph_clock_enable(RCU_AF);
    // 设置优先级---15
    nvic_irq_enable(EXTI10_15_IRQn,2,2);
    // 设置中断线---PA15
    gpio_exti_source_select(GPIO_PORT_SOURCE_GPIOA,GPIO_PIN_SOURCE_15);
    // 设置中断线和中断边沿---15下降沿触发
    exti_init(EXTI_15,EXTI_INTERRUPT,EXTI_TRIG_FALLING);
    // 清除中断标记
    exti_interrupt_flag_clear(EXTI_15);
}

/*
 * @description: 外部中断--Line15函数
 * @return {*}
 * @Date: 2023-05-13 20:51:42
 */
// 外部中断--Line15函数
void EXTI10_15_IRQHandler(void)
{
    if(SET == exti_interrupt_flag_get(EXTI_15))
    {
        MyKey.Key_Down_State[0] = 1;
        // 清除标志位
        exti_interrupt_flag_clear(EXTI_15);
    }
}