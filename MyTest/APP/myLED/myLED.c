#include "myLED.h"

/*====================================变量区 BEGIN====================================*/
MyLed_TypeDef MyLed = {
    .bLed1_State = LED_OFF,
    .vLED_Init = &vLED_Init,
    .vLED_Control = &vLED_Control
};


/*====================================变量区    END====================================*/


/*
 * @description: LED初始化
 * @return {*}
 * @Date: 2023-05-09 08:44:30
 */
// LED初始化
void vLED_Init(void)
{
    rcu_periph_clock_enable(RCU_GPIOC);
    gpio_init(GPIOC,GPIO_MODE_OUT_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_13);
    // 默认灭
    MyLed.vLED_Control(LED_OFF);
}

/*
 * @description: LED控制
 * @param {uint8_t} swch LED_ON--亮 LED_OFF--灭
 * @return {*}
 * @Date: 2023-05-09 08:43:15
 */
// LED控制
void vLED_Control(uint8_t swch)
{
    if(LED_ON == swch)
    {
        gpio_bit_reset(GPIOC,GPIO_PIN_13);
    }
    else
    {
        gpio_bit_set(GPIOC,GPIO_PIN_13);
    }
    MyLed.bLed1_State = swch;
}