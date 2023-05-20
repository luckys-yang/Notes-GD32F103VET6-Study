#include "myADC.h"

/*====================================������ BEGIN====================================*/
MyADC_TypeDef MyADC = 
{
    .bADC_Dma_Flag = 0,
    .ulADC_Value = {0},
    .bADC_Start_Flag = 0,
    .vADC_Init = &vADC_Init,
    .usADC_Get_Value = &usADC_Get_Value
};
/*====================================������    END====================================*/


/*
 * @description: ADC��ʼ��
 * @return {*}
 * @Date: 2023-05-14 17:03:43
 */
// ADC��ʼ��
void vADC_Init(void)
{
    // ʹ��GPIOAʱ��
    rcu_periph_clock_enable(RCU_GPIOA);
    // ʹ��ADCʱ��
    rcu_periph_clock_enable(RCU_ADC0);
    // ����ADCʱ�ӣ�ADC���14MHz
    rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV8);
    // �˿ڳ�ʼ��--һ��Ҫ���ó�ģ������
    gpio_init(GPIOA,GPIO_MODE_AIN,GPIO_OSPEED_50MHZ,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4);
    // ----------------DMA����----------------
    // ʹ��DMAʱ��
    rcu_adc_clock_config(RCU_DMA0);
    dma_parameter_struct myDMA0;
    // ��λDMA-CH0
    dma_deinit(DMA0,DMA_CH0);
    // �������ַ---ADC�Ĺ������ݼĴ���
    myDMA0.periph_addr = (uint32_t)(&ADC_RDATA(ADC0));
    // �������ݴ�����---16λ
    myDMA0.periph_width = DMA_PERIPHERAL_WIDTH_32BIT;
    // �洢������ַ 
    myDMA0.memory_addr = (uint32_t)(&MyADC.ulADC_Value);
    // �洢�����ݴ�����---16λ
    myDMA0.memory_width = DMA_MEMORY_WIDTH_32BIT;
    // DMAͨ�����ݴ�������---4��ͨ��
    myDMA0.number = 4;
    // DMAͨ������������ȼ�---��
    myDMA0.priority = DMA_PRIORITY_HIGH;
    // �����ַ�����㷨ģʽ---�������ַ������
    myDMA0.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    // �洢����ַ�����㷨ģʽ---�ڴ����ַ����
    myDMA0.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    // DMAͨ�����ݴ��䷽��---���赽�ڴ�
    myDMA0.direction = DMA_PERIPHERAL_TO_MEMORY;
    // DMA��ʼ��
    dma_init(DMA0,DMA_CH0,&myDMA0);
    // ѭ��ģʽ---����
    dma_circulation_enable(DMA0,DMA_CH0);
    // ��DMAͨ���ж�
    nvic_irq_enable(DMA0_Channel0_IRQn,0,1);
    // ת�����������ж�
    dma_interrupt_enable(DMA0,DMA_CH0,DMA_INT_FTF);
    // DMAͨ��ʹ��
    dma_channel_enable(DMA0,DMA_CH0);


    // ----------------ADC����----------------
    // ADC����ģʽ---����ģʽ(ֻʹ����һ��ADC)
    adc_mode_config(ADC_MODE_FREE);
    // ���ݶ��뷽ʽ---�Ҷ���
    adc_data_alignment_config(ADC0,ADC_DATAALIGN_RIGHT);
    // �Ƿ�������ת��ģʽ---ʹ��
    adc_special_function_config(ADC0,ADC_CONTINUOUS_MODE,ENABLE);
    // ɨ��ģʽ����
    adc_special_function_config(ADC0,ADC_SCAN_MODE,ENABLE);
    // �������еĳ���(ͨ����)---�����飬4
    adc_channel_length_config(ADC0,ADC_REGULAR_CHANNEL,4);
    // ����4������ͨ���ɼ�---����ʱ��Ϊ7.5��ʱ������
    adc_regular_channel_config(ADC0,0,ADC_CHANNEL_0,ADC_SAMPLETIME_7POINT5);
    adc_regular_channel_config(ADC0,1,ADC_CHANNEL_1,ADC_SAMPLETIME_7POINT5);
    adc_regular_channel_config(ADC0,2,ADC_CHANNEL_2,ADC_SAMPLETIME_7POINT5);
    adc_regular_channel_config(ADC0,3,ADC_CHANNEL_3,ADC_SAMPLETIME_7POINT5);
    // ADC����ͨ������Դѡ��---�������
    adc_external_trigger_source_config(ADC0,ADC_REGULAR_CHANNEL,ADC0_1_2_EXTTRIG_REGULAR_NONE);
    // ����ADC�ⲿ����---ʹ��
    adc_external_trigger_config(ADC0,ADC_REGULAR_CHANNEL,ENABLE);
    // ʹ��ADC
    adc_enable(ADC0);
    // ��ʱ1ms
    delay_1ms(1);
    // ADCУ׼��λ
    adc_calibration_enable(ADC0);
    // ADC DMAʹ��
    adc_dma_mode_enable(ADC0);
    // ADC �������ʹ��
    adc_software_trigger_enable(ADC0,ADC_REGULAR_CHANNEL);
}

/*
 * @description: ADC����
 * @return {*}
 * @Date: 2023-05-14 18:11:17
 */
// ADC����
uint16_t usADC_Get_Value(uint8_t channel)
{
    // ���ù���ͨ���ɼ�---����ʱ��Ϊ7.5��ʱ������
    adc_regular_channel_config(ADC0,0,channel,ADC_SAMPLETIME_7POINT5);
    // �������ʹ��
    adc_software_trigger_enable(ADC0,ADC_REGULAR_CHANNEL);
    // �ȴ��������
    while(!adc_flag_get(ADC0,ADC_FLAG_EOC));
    // �����־
    adc_flag_clear(ADC0,ADC_FLAG_EOC);

    return (adc_regular_data_read(ADC0));
}

/*
 * @description: DMA�жϺ���
 * @return {*}
 * @Date: 2023-05-15 14:54:16
 */
void DMA0_Channel0_IRQHandler(void)
{
    if(SET == dma_interrupt_flag_get(DMA0,DMA_CH0,DMA_INT_FLAG_FTF))
    {
        // �����־λ
        dma_interrupt_flag_clear(DMA0,DMA_CH0,DMA_INT_FLAG_FTF);
        MyADC.bADC_Dma_Flag = 1;
    }
}

