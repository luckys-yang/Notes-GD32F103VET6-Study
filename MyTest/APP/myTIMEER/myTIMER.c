/*
*@Description: 定时器
*@Author: Yang
*@Date: 2023-05-11 19:42:10
*/
#include "myTIMER.h"

/*====================================变量区 BEGIN====================================*/
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
/*====================================变量区    END====================================*/


/*
 * @description: 定时器5初始化
 * @param {uint16_t} psc 预分频值
 * @param {uint16_t} arr 重装载值
 * @return {*}
 * @Date: 2023-05-11 21:27:05
 */
// 定时器5初始化
void vTIMER5_init(uint16_t psc,uint16_t arr)
{
    timer_parameter_struct myTIMER5;
    // 开启定时器时钟
    rcu_periph_clock_enable(RCU_TIMER5);
    // 结构体复位初始化
    timer_deinit(TIMER5);
    // 初始化定时器结构体
    timer_struct_para_init(&myTIMER5);
    // 预分频--psc值
    myTIMER5.prescaler = psc;
    // 对齐模式
    myTIMER5.alignedmode = TIMER_COUNTER_EDGE;
    // 计数方向--向上计数
    myTIMER5.counterdirection = TIMER_COUNTER_UP;
    // 重装载值--arr
    myTIMER5.period = arr;
    // 时钟分频因子--不分频
    myTIMER5.clockdivision = TIMER_CKDIV_DIV1;
    // 计数器重复计数次数-- 0~255(高级定时器才有)
    myTIMER5.repetitioncounter = 0;
    // 初始化
    timer_init(TIMER5,&myTIMER5);
    // 显式清除中断标志位
    timer_interrupt_flag_clear(TIMER5,TIMER_INT_FLAG_UP);
    // 开启中断
    timer_interrupt_enable(TIMER5, TIMER_INT_UP);
    // 配置中断函数和优先级
    nvic_irq_enable(TIMER5_IRQn,0,0);
    // 使能
    timer_enable(TIMER5);
}


/*
 * @description: 定时器5中断函数
 * @return {*}
 * @Date: 2023-05-12 08:40:32
 */
// 定时器5中断函数
void TIMER5_IRQHandler(void)
{
    if(SET == timer_interrupt_flag_get(TIMER5, TIMER_INT_UP))
    {
        // 清除标志位
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
            // 喂狗
            fwdgt_counter_reload();
        }
    }
}


/*
 * @description: 定时器1PWM初始化
 * @param {uint16_t} psc 预分频
 * @param {uint16_t} arr 重装载值
 * @param {uint16_t} ch0_duty 通道0占空比
 * @param {uint16_t} ch1_duty 通道1占空比
 * @param {uint16_t} ch2_duty 通道2占空比
 * @param {uint16_t} ch3_duty 通道3占空比
 * @return {*}
 * @Date: 2023-05-12 15:52:50
 */
// 定时器1PWM初始化
void vTIMER1_Pwm_Init(uint16_t psc,uint16_t arr,uint16_t ch0_duty,uint16_t ch1_duty,uint16_t ch2_duty,uint16_t ch3_duty)
{
    timer_parameter_struct myTIMER1;
    timer_oc_parameter_struct myTIMER1_OC;

    // 开启定时器时钟和复用时钟
    rcu_periph_clock_enable(RCU_TIMER1);
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
    // 结构体复位初始化
    timer_deinit(TIMER1);

    // 初始化定时器结构体
    timer_struct_para_init(&myTIMER1);
    // 预分频--psc值
    myTIMER1.prescaler = psc;
    // 对齐模式
    myTIMER1.alignedmode = TIMER_COUNTER_EDGE;
    // 计数方向--向上计数
    myTIMER1.counterdirection = TIMER_COUNTER_UP;
    // 重装载值--arr
    myTIMER1.period = arr;
    // 时钟分频因子--不分频
    myTIMER1.clockdivision = TIMER_CKDIV_DIV1;
    // 初始化
    timer_init(TIMER1,&myTIMER1);

    // 通道使能
    myTIMER1_OC.outputstate = TIMER_CCX_ENABLE;
    // 通道极性--高电平有效
    myTIMER1_OC.ocpolarity = TIMER_OC_POLARITY_HIGH;
    // ----------------通道0配置----------------
    // 外设TIMERx的通道输出配置
    timer_channel_output_config(TIMER1,TIMER_CH_0,&myTIMER1_OC);
    // 通道占空比设置
    timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_0,ch0_duty);
    // 通道模式---PWM0模式
    timer_channel_output_mode_config(TIMER1,TIMER_CH_0,TIMER_OC_MODE_PWM0);
    // 不使用输出比较影子寄存器
    timer_channel_output_shadow_config(TIMER1,TIMER_CH_0,TIMER_OC_SHADOW_DISABLE);
    // ----------------通道1配置----------------
    // 外设TIMERx的通道输出配置
    timer_channel_output_config(TIMER1,TIMER_CH_1,&myTIMER1_OC);
    // 通道占空比设置
    timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_1,ch1_duty);
    // 通道模式---PWM0模式
    timer_channel_output_mode_config(TIMER1,TIMER_CH_1,TIMER_OC_MODE_PWM0);
    // 不使用输出比较影子寄存器
    timer_channel_output_shadow_config(TIMER1,TIMER_CH_1,TIMER_OC_SHADOW_DISABLE);
    // ----------------通道2配置----------------
    // 外设TIMERx的通道输出配置
    timer_channel_output_config(TIMER1,TIMER_CH_2,&myTIMER1_OC);
    // 通道占空比设置
    timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_2,ch2_duty);
    // 通道模式---PWM0模式
    timer_channel_output_mode_config(TIMER1,TIMER_CH_2,TIMER_OC_MODE_PWM0);
    // 不使用输出比较影子寄存器
    timer_channel_output_shadow_config(TIMER1,TIMER_CH_2,TIMER_OC_SHADOW_DISABLE);
    // ----------------通道3配置----------------
    // 外设TIMERx的通道输出配置
    timer_channel_output_config(TIMER1,TIMER_CH_3,&myTIMER1_OC);
    // 通道占空比设置
    timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_3,ch3_duty);
    // 通道模式---PWM0模式
    timer_channel_output_mode_config(TIMER1,TIMER_CH_3,TIMER_OC_MODE_PWM0);
    // 不使用输出比较影子寄存器
    timer_channel_output_shadow_config(TIMER1,TIMER_CH_3,TIMER_OC_SHADOW_DISABLE);
    // 使能自动重装载
    timer_auto_reload_shadow_enable(TIMER1);
    // 使能定时器1
    timer_enable(TIMER1);
}

/*
 * @description: 定时器2初始化PWM捕获
 * @return {*}
 * @Date: 2023-05-12 21:43:32
 */
// 定时器2初始化PWM捕获
void vTIMER2_Init(void)
{
    timer_parameter_struct myTIMER2;
    timer_ic_parameter_struct myTIMER2_IC;

    // 使能时钟
    rcu_periph_clock_enable(RCU_GPIOA);
    // 使能复用时钟
    rcu_periph_clock_enable(RCU_AF);
    // 配置 PA6为CH0的复用模式
    gpio_init(GPIOA,GPIO_MODE_IN_FLOATING,GPIO_OSPEED_50MHZ,GPIO_PIN_6);
    // 使能定时器2
    rcu_periph_clock_enable(RCU_TIMER2);
    // -----------------定时器基本+中断配置-----------------
   // 结构体复位初始化
    timer_deinit(TIMER2);
    // 预分频--psc值
    myTIMER2.prescaler = 107;
    // 对齐模式
    myTIMER2.alignedmode = TIMER_COUNTER_EDGE;
    // 计数方向--向上计数
    myTIMER2.counterdirection = TIMER_COUNTER_UP;
    // 重装载值--arr(设置为最大值即可)
    myTIMER2.period = 65535;
    // 时钟分频因子--不分频
    myTIMER2.clockdivision = TIMER_CKDIV_DIV1;
    // 初始化
    timer_init(TIMER2,&myTIMER2);
    // 清除中断标志---通道0
    timer_interrupt_flag_clear(TIMER2,TIMER_INT_FLAG_CH0);
    // 开启中断
    timer_interrupt_enable(TIMER2, TIMER_INT_FLAG_CH0);
    // 配置中断函数和优先级
    nvic_irq_enable(TIMER2_IRQn,0,1);
    // -----------------定时器输入捕获配置-----------------
    // 通道输入极性---上升沿
    myTIMER2_IC.icpolarity = TIMER_IC_POLARITY_RISING;
    // 通道输入模式选择---直连
    myTIMER2_IC.icselection = TIMER_IC_SELECTION_DIRECTTI;
    // 通道输入捕获预分频---不分频
    myTIMER2_IC.icprescaler = TIMER_IC_PSC_DIV1;
    // 通道输入捕获滤波(0~15)
    myTIMER2_IC.icfilter = 0;
#if 0
    // 配置输入捕获参数
    timer_input_capture_config(TIMER2,TIMER_CH_0,&myTIMER2_IC);
    // 自动重装载使能
    timer_auto_reload_shadow_enable(TIMER2);
#endif
    // 捕获PWM输入参数
    timer_input_pwm_capture_config(TIMER2,TIMER_CH_0,&myTIMER2_IC);
    // 输入触发源选择---滤波后的通道0输入
    timer_input_trigger_source_select(TIMER2,TIMER_SMCFG_TRGSEL_CI0FE0);
    // 从模式配置---复位模式
    timer_slave_mode_select(TIMER2,TIMER_SLAVE_MODE_RESTART);
    // 主从模式配置---使能
    timer_master_slave_mode_config(TIMER2,TIMER_MASTER_SLAVE_MODE_ENABLE);
    // 自动重装载使能
    timer_auto_reload_shadow_enable(TIMER2);
    // 使能
    timer_enable(TIMER2);
}

/*
 * @description: 定时器2中断函数
 * @return {*}
 * @Date: 2023-05-12 21:44:27
 */
// 定时器2中断函数
void TIMER2_IRQHandler(void)
{
    static uint32_t Last_ic_Value = 0;
    uint32_t ic_Value = 0,ic_Value1 = 0,ic_Value2 = 0;
#if 0
    if(SET == timer_interrupt_flag_get(TIMER2,TIMER_INT_FLAG_CH0))
    {
        // 清除中断标志位
        timer_interrupt_flag_clear(TIMER2,TIMER_INT_FLAG_CH0);
        // 读取通道捕获值
        ic_Value = timer_channel_capture_value_register_read(TIMER2,TIMER_CH_0);
        // 如果上一个值存在
        if(Last_ic_Value)
        {
            if(ic_Value > Last_ic_Value)
            {
                MyTimer.ulTimer2_IC_Fre = 1000000 / (ic_Value - Last_ic_Value);
            }
            else
            {
                // 说明已经溢出，需要加最大重装载值(都是16位所以是0xFFFF，32位则0xFFFFFFFF)
                MyTimer.ulTimer2_IC_Fre = 1000000 / (0xFFFF + ic_Value - Last_ic_Value);
            }
            MyTimer.bTimer2_IC_Over_Flag = 1;
        }
        Last_ic_Value = ic_Value;
    }
#endif
    if(SET == timer_interrupt_flag_get(TIMER2,TIMER_INT_FLAG_CH0))
    {
        // 清除中断标志位
        timer_interrupt_flag_clear(TIMER2,TIMER_INT_FLAG_CH0);
        // 读取通道0捕获值---周期时间
        ic_Value1 = TIMER_CH0CV(TIMER2) + 1;
        if(ic_Value1 != 0)
        {
            // 读取通道1捕获值---高电平时间
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
 * @description: 定时器0PWM初始化
 * @return {*}
 * @Date: 2023-05-13 22:00:00
 */
// 定时器0PWM初始化
void vTIMER0_Pwn_Init(uint16_t psc,uint16_t arr,uint16_t ch0_duty,uint16_t ch1_duty,uint16_t ch2_duty)
{
    timer_parameter_struct myTIMER0;
    timer_oc_parameter_struct myTIMER0_OC;

    // 使能GPIOA、B时钟
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    // 使能复用时钟
    rcu_periph_clock_enable(RCU_AF);
    rcu_periph_clock_enable(RCU_TIMER0);
    // PWM输出引脚 初始化
    gpio_init(GPIOA,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_8);
    gpio_init(GPIOA,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_9);
    gpio_init(GPIOA,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_10);
    // PWM互补输出引脚 初始化
    gpio_init(GPIOB,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_12); // 刹车引脚
    gpio_init(GPIOB,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_13);
    gpio_init(GPIOB,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_14);
    gpio_init(GPIOB,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_15);
    //------------------定时器基本配置--------------------
    // 初始化定时器结构体
    timer_struct_para_init(&myTIMER0);
    timer_deinit(TIMER0);
    // 预分频
    myTIMER0.prescaler = psc;
    // 对齐模式
    myTIMER0.alignedmode = TIMER_COUNTER_EDGE;
    // 计数方向
    myTIMER0.counterdirection = TIMER_COUNTER_UP;
    // 重装载值
    myTIMER0.period = arr;
    // 分频因子
    myTIMER0.clockdivision = TIMER_CKDIV_DIV1;
    myTIMER0.repetitioncounter = 0;
    timer_init(TIMER0,&myTIMER0);
    // ---------------定时器PWM输出配置------------------
    // 输出使能
    myTIMER0_OC.outputstate = TIMER_CCX_ENABLE;
    // 互补输出使能
    myTIMER0_OC.outputnstate = TIMER_CCXN_ENABLE;
    // 输出极性---高
    myTIMER0_OC.ocpolarity = TIMER_OC_POLARITY_HIGH;
    // 互补输出极性---高
    myTIMER0_OC.ocnpolarity = TIMER_OCN_POLARITY_HIGH;
    // 空闲输出电平---低
    myTIMER0_OC.ocidlestate = TIMER_OC_IDLE_STATE_LOW;
    // 互补空闲输出电平---低
    myTIMER0_OC.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
    // 外设TIMERx的通道输出配置
    timer_channel_output_config(TIMER0,TIMER_CH_0,&myTIMER0_OC);
    timer_channel_output_config(TIMER0,TIMER_CH_1,&myTIMER0_OC);
    timer_channel_output_config(TIMER0,TIMER_CH_2,&myTIMER0_OC);
    // --------通道0----------
    // 通道占空比设置
    timer_channel_output_pulse_value_config(TIMER0,TIMER_CH_0,ch0_duty);
    // 通道模式---PWM0模式
    timer_channel_output_mode_config(TIMER0,TIMER_CH_0,TIMER_OC_MODE_PWM0);
    // 不使用输出比较影子寄存器
    timer_channel_output_shadow_config(TIMER0,TIMER_CH_0,TIMER_OC_SHADOW_DISABLE);    
    // --------通道1----------
    // 通道占空比设置
    timer_channel_output_pulse_value_config(TIMER0,TIMER_CH_1,ch1_duty);
    // 通道模式---PWM0模式
    timer_channel_output_mode_config(TIMER0,TIMER_CH_1,TIMER_OC_MODE_PWM0);
    // 不使用输出比较影子寄存器
    timer_channel_output_shadow_config(TIMER0,TIMER_CH_1,TIMER_OC_SHADOW_DISABLE);    
    // --------通道2----------
    // 通道占空比设置
    timer_channel_output_pulse_value_config(TIMER0,TIMER_CH_2,ch2_duty);
    // 通道模式---PWM0模式
    timer_channel_output_mode_config(TIMER0,TIMER_CH_2,TIMER_OC_MODE_PWM0);
    // 不使用输出比较影子寄存器
    timer_channel_output_shadow_config(TIMER0,TIMER_CH_2,TIMER_OC_SHADOW_DISABLE);            
    // 高级定时器需要打开
    timer_primary_output_config(TIMER0,ENABLE);
    // 自动重装载使能
    timer_auto_reload_shadow_enable(TIMER0);
    // -------------------刹车配置-------------------    
    timer_break_parameter_struct myTIMER0_Break;
    // 运行模式下“关闭状态”配置---使能
    myTIMER0_Break.runoffstate = TIMER_ROS_STATE_ENABLE;
    // 空闲模式下“关闭状态”配置---使能
    myTIMER0_Break.ideloffstate = TIMER_IOS_STATE_ENABLE;
    // 死区时间(0~255)
    myTIMER0_Break.deadtime = 255;
    // 中止信号极性---高电平(触发刹车电平)
    myTIMER0_Break.breakpolarity = TIMER_BREAK_POLARITY_HIGH;
    // 自动输出使能---使能
    myTIMER0_Break.outputautostate = TIMER_OUTAUTO_ENABLE;
    // 互补寄存器保护控制
    myTIMER0_Break.protectmode = TIMER_CCHP_PROT_0;
    // 中止使能---使能
    myTIMER0_Break.breakstate = TIMER_BREAK_ENABLE;
    // 刹车配置
    timer_break_config(TIMER0,&myTIMER0_Break);
    // 默认低电平
    gpio_bit_write(GPIOB,GPIO_PIN_12,RESET);
    // 使能定时器
    timer_enable(TIMER0);
}