#include "myADC.h"

/*====================================变量区 BEGIN====================================*/
MyADC_TypeDef MyADC = 
{
    .bADC_Dma_Flag = 0,
    .ulADC_Value = {0},
    .bADC_Start_Flag = 0,
    .vADC_Init = &vADC_Init,
    .usADC_Get_Value = &usADC_Get_Value
};
/*====================================变量区    END====================================*/


/*
 * @description: ADC初始化
 * @return {*}
 * @Date: 2023-05-14 17:03:43
 */
// ADC初始化
void vADC_Init(void)
{
    // 使能GPIOA时钟
    rcu_periph_clock_enable(RCU_GPIOA);
    // 使能ADC时钟
    rcu_periph_clock_enable(RCU_ADC0);
    // 配置ADC时钟，ADC最大14MHz
    rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV8);
    // 端口初始化--一定要设置成模拟输入
    gpio_init(GPIOA,GPIO_MODE_AIN,GPIO_OSPEED_50MHZ,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4);
    // ----------------DMA配置----------------
    // 使能DMA时钟
    rcu_adc_clock_config(RCU_DMA0);
    dma_parameter_struct myDMA0;
    // 复位DMA-CH0
    dma_deinit(DMA0,DMA_CH0);
    // 外设基地址---ADC的规则数据寄存器
    myDMA0.periph_addr = (uint32_t)(&ADC_RDATA(ADC0));
    // 外设数据传输宽度---16位
    myDMA0.periph_width = DMA_PERIPHERAL_WIDTH_32BIT;
    // 存储器基地址 
    myDMA0.memory_addr = (uint32_t)(&MyADC.ulADC_Value);
    // 存储器数据传输宽度---16位
    myDMA0.memory_width = DMA_MEMORY_WIDTH_32BIT;
    // DMA通道数据传输数量---4个通道
    myDMA0.number = 4;
    // DMA通道传输软件优先级---高
    myDMA0.priority = DMA_PRIORITY_HIGH;
    // 外设地址生成算法模式---外设基地址不增加
    myDMA0.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    // 存储器地址生成算法模式---内存基地址自增
    myDMA0.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    // DMA通道数据传输方向---外设到内存
    myDMA0.direction = DMA_PERIPHERAL_TO_MEMORY;
    // DMA初始化
    dma_init(DMA0,DMA_CH0,&myDMA0);
    // 循环模式---开启
    dma_circulation_enable(DMA0,DMA_CH0);
    // 打开DMA通道中断
    nvic_irq_enable(DMA0_Channel0_IRQn,0,1);
    // 转换结束产生中断
    dma_interrupt_enable(DMA0,DMA_CH0,DMA_INT_FTF);
    // DMA通道使能
    dma_channel_enable(DMA0,DMA_CH0);


    // ----------------ADC配置----------------
    // ADC工作模式---独立模式(只使用了一个ADC)
    adc_mode_config(ADC_MODE_FREE);
    // 数据对齐方式---右对齐
    adc_data_alignment_config(ADC0,ADC_DATAALIGN_RIGHT);
    // 是否开启连续转换模式---使能
    adc_special_function_config(ADC0,ADC_CONTINUOUS_MODE,ENABLE);
    // 扫描模式开启
    adc_special_function_config(ADC0,ADC_SCAN_MODE,ENABLE);
    // 规则序列的长度(通道数)---常规组，4
    adc_channel_length_config(ADC0,ADC_REGULAR_CHANNEL,4);
    // 配置4个规则通道采集---采样时间为7.5个时钟周期
    adc_regular_channel_config(ADC0,0,ADC_CHANNEL_0,ADC_SAMPLETIME_7POINT5);
    adc_regular_channel_config(ADC0,1,ADC_CHANNEL_1,ADC_SAMPLETIME_7POINT5);
    adc_regular_channel_config(ADC0,2,ADC_CHANNEL_2,ADC_SAMPLETIME_7POINT5);
    adc_regular_channel_config(ADC0,3,ADC_CHANNEL_3,ADC_SAMPLETIME_7POINT5);
    // ADC规则通道触发源选择---软件触发
    adc_external_trigger_source_config(ADC0,ADC_REGULAR_CHANNEL,ADC0_1_2_EXTTRIG_REGULAR_NONE);
    // 配置ADC外部触发---使能
    adc_external_trigger_config(ADC0,ADC_REGULAR_CHANNEL,ENABLE);
    // 使能ADC
    adc_enable(ADC0);
    // 延时1ms
    delay_1ms(1);
    // ADC校准复位
    adc_calibration_enable(ADC0);
    // ADC DMA使能
    adc_dma_mode_enable(ADC0);
    // ADC 软件触发使能
    adc_software_trigger_enable(ADC0,ADC_REGULAR_CHANNEL);
}

/*
 * @description: ADC采样
 * @return {*}
 * @Date: 2023-05-14 18:11:17
 */
// ADC采样
uint16_t usADC_Get_Value(uint8_t channel)
{
    // 配置规则通道采集---采样时间为7.5个时钟周期
    adc_regular_channel_config(ADC0,0,channel,ADC_SAMPLETIME_7POINT5);
    // 软件触发使能
    adc_software_trigger_enable(ADC0,ADC_REGULAR_CHANNEL);
    // 等待采样完成
    while(!adc_flag_get(ADC0,ADC_FLAG_EOC));
    // 清除标志
    adc_flag_clear(ADC0,ADC_FLAG_EOC);

    return (adc_regular_data_read(ADC0));
}

/*
 * @description: DMA中断函数
 * @return {*}
 * @Date: 2023-05-15 14:54:16
 */
void DMA0_Channel0_IRQHandler(void)
{
    if(SET == dma_interrupt_flag_get(DMA0,DMA_CH0,DMA_INT_FLAG_FTF))
    {
        // 清除标志位
        dma_interrupt_flag_clear(DMA0,DMA_CH0,DMA_INT_FLAG_FTF);
        MyADC.bADC_Dma_Flag = 1;
    }
}

