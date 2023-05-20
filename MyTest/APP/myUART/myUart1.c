#include "myUart1.h"

/*====================================������ BEGIN====================================*/
MyUart0_TypeDef MyUart0 = 
{
    .bUart0_Rx_Over_Flag = 0,
    .ucUart0_Rx_Len = 0,
    .ucUart0_Rx_Buff = {0},
    .vUART0_Init = &vUART0_Init,
    .vUSART0_Data_Process = &vUSART0_Data_Process
};

/*====================================������    END====================================*/

/*====================================��̬�ڲ����������� BEGIN====================================*/
static void svUART0_Interrupt_Init(void);
static void svUSART0_Dma_Init(uint32_t hope_len);
static void svUSART0_Enable_Idle_Interrupt(void);
static void svUSART0_Again_Dma_Tx(void);
/*====================================��̬�ڲ�����������    END====================================*/


/*
 * @description: ����1��ʼ��
 * @return {*}
 * @Date: 2023-05-09 21:21:23
 */
// ����1��ʼ��
void vUART0_Init(void)
{
    // ʹ�ܴ���1ʱ��
    rcu_periph_clock_enable(RCU_USART0);

// ���������Ҫ��ӳ��
#ifdef USART0_REMAP
    // ʹ��GPIOʱ��
    rcu_periph_clock_enable(GPIOB);
    rcu_periph_clock_enable(RCU_AF);
    // ����1��ӳ��ʹ��
    gpio_pin_remap_config(GPIO_USART0_REMAP, ENABLE);
    // TX--��������
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
    // RX--��������
    gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_7);
#else
    // ʹ��GPIOʱ��
    rcu_periph_clock_enable(RCU_GPIOA);
    // TX--��������
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
    // RX--��������
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
#endif
    // ���ڲ�����ʼ��
    // ���ô���1
    usart_deinit(USART0);
    // ���ò�����
    usart_baudrate_set(USART0,115200U);
    // �������ݳ���
    usart_word_length_set(USART0,USART_WL_8BIT);
    // ����ֹͣλ
    usart_stop_bit_set(USART0,USART_STB_1BIT);
    // ���ü���λ
    usart_parity_config(USART0,USART_PM_NONE);
    // Ӳ��������--�ر�
    usart_hardware_flow_rts_config(USART0,USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(USART0,USART_CTS_DISABLE);
    // ���ڽ���ʹ��
    usart_receive_config(USART0,USART_RECEIVE_ENABLE);
    // ���ڷ���ʹ��
    usart_transmit_config(USART0,USART_TRANSMIT_ENABLE);
    // ʹ�ܴ���
    usart_enable(USART0);
    //�����жϳ�ʼ��
    // svUART0_Interrupt_Init();
    // ����DMA��ʼ��
    svUSART0_Dma_Init(UART0_MAX_LEN);
    // ʹ�ܿ����ж�
    svUSART0_Enable_Idle_Interrupt();
}



/*
 * @description: ����1�ض���
 * @param {int} ch
 * @param {FILE} *f
 * @return {*}
 * @Date: 2023-05-10 14:53:40
 */
// ����1�ض���
int fputc(int ch,FILE *f)
{
    usart_data_transmit(USART0,(uint8_t)ch);
    int Cnt = 1000;
    while(RESET == usart_flag_get(USART0,USART_FLAG_TBE) && Cnt--);
    return ch;
}

/*
 * @description: ʹ�ܴ���0�ж�
 * @return {*}
 * @Date: 2023-05-10 16:02:09
 */
// ʹ�ܴ���0�ж�
static void svUART0_Interrupt_Init(void)
{
    // �жϹ�����ʹ�ܣ����������ȼ�
    nvic_irq_enable(USART0_IRQn,1,1);
    // ����жϱ�־
    usart_interrupt_flag_clear(USART0,USART_INT_FLAG_RBNE);
    usart_interrupt_flag_clear(USART0,USART_INT_FLAG_IDLE);
    // ʹ�ܴ����ж�
    usart_interrupt_enable(USART0,USART_INT_RBNE);  // �����ݻ������ǿ��жϺ͹��ش����ж�
    usart_interrupt_enable(USART0,USART_INT_IDLE);  // IDLE�߼���ж�
}

/*
 * @description: ����0�жϷ�����
 * @return {*}
 * @Date: 2023-05-10 18:43:08
 */
// ����0�жϷ�����
void USART0_IRQHandler(void)
{
#if 0
    // ��ȡ��������Ϊ��
    if(SET == usart_interrupt_flag_get(USART0,USART_INT_FLAG_RBNE))
    {
        // ��ȡ���ݴ洢����
        MyUart0.ucUart0_Rx_Buff[MyUart0.ucUart0_Rx_Len++] = usart_data_receive(USART0);
    }
    else if(SET == usart_interrupt_flag_get(USART0,USART_INT_FLAG_IDLE))
    {
        // ��������жϱ�־λ
        usart_data_receive(USART0);
        // ��һ����������жϱ�־λ����
        usart_interrupt_flag_clear(USART0,USART_INT_FLAG_IDLE);
        // ������һ֡���ݱ�־λ
        MyUart0.bUart0_Rx_Over_Flag = 1;
    }
#endif
    if (SET == usart_interrupt_flag_get(USART0, USART_INT_FLAG_IDLE))
    {
        // �����־λ
        usart_data_receive(USART0);
        // ��������DMAx��ͨ��y����
        dma_channel_disable(DMA0,DMA_CH4);
        if (UART0_MAX_LEN)
        {
            MyUart0.ucUart0_Rx_Len = UART0_MAX_LEN - dma_transfer_number_get(DMA0, DMA_CH4);
            if((MyUart0.ucUart0_Rx_Len != 0) && (MyUart0.ucUart0_Rx_Len < UART0_MAX_LEN))
            {
                MyUart0.bUart0_Rx_Over_Flag = 1;
            }
        }
    }
}

/*
 * @description: DMA�жϺ���
 * @return {*}
 * @Date: 2023-05-11 14:49:08
 */
// DMA�жϺ���
void DMA0_Channel4_IRQHandler(void)
{
    // DMAͨ��������ɱ�־
    if(dma_interrupt_flag_get(DMA0,DMA_CH4,DMA_INT_FLAG_FTF))
    {
        // ���DMA�жϱ�־
        dma_interrupt_flag_clear(DMA0,DMA_CH4,DMA_INT_FLAG_G);
    }
}

/*
 * @description: �Խ��յ����ݽ��д���
 * @return {*}
 * @Date: 2023-05-10 18:45:09
 */
// �Խ��յ����ݽ��д���
void vUSART0_Data_Process(void)
{
    if(MyUart0.bUart0_Rx_Over_Flag)
    {
        MyUart0.bUart0_Rx_Over_Flag = 0;
        printf("Read Len: %d ",MyUart0.ucUart0_Rx_Len);
        // ������0��������0
        for(uint8_t i = 0; i < MyUart0.ucUart0_Rx_Len; i++)
        {
            printf("%02x  ",(uint32_t)MyUart0.ucUart0_Rx_Buff[i]);
        }
        printf("\r\n");
        memset(MyUart0.ucUart0_Rx_Buff,0,sizeof(MyUart0.ucUart0_Rx_Buff));
        MyUart0.ucUart0_Rx_Len = 0;
        // ���´�DMA����
        svUSART0_Again_Dma_Tx();
    }
    // if(MyTimer.bTimer2_IC_Over_Flag)
    // {
    //     MyTimer.bTimer2_IC_Over_Flag = 0;
    //     printf("CH0-Fre:%u Hz CH0-Duty:%d%%\r\n",MyTimer.ulTimer2_IC_Fre,MyTimer.ucTimer2_IC_Duty);
    // }
    // if(MyADC.bADC_Start_Flag && MyADC.bADC_Dma_Flag)
    // {
    //     float adc_temp[4] = {0.0};
    //     // MyADC.fADC_Value[0] = (float)MyADC.usADC_Get_Value(ADC_CHANNEL_1);
    //     adc_temp[0] = (float)MyADC.ulADC_Value[0]/4096*3.3f;
    //     // MyADC.fADC_Value[1] = (float)MyADC.usADC_Get_Value(ADC_CHANNEL_2);
    //     adc_temp[1] = (float)MyADC.ulADC_Value[1]/4096*3.3f;
    //     // MyADC.fADC_Value[2] = (float)MyADC.usADC_Get_Value(ADC_CHANNEL_3);
    //     adc_temp[2] = (float)MyADC.ulADC_Value[2]/4096*3.3f;
    //     // MyADC.fADC_Value[3] = (float)MyADC.usADC_Get_Value(ADC_CHANNEL_4);
    //     adc_temp[3] = (float)MyADC.ulADC_Value[3]/4096*3.3f;
    //     printf("%.1f--%.1f--%.1f--%.1f\r\n",adc_temp[0],adc_temp[1],adc_temp[2],adc_temp[3]);
    //     MyADC.bADC_Start_Flag = 0;
    //     MyADC.bADC_Dma_Flag = 0;
    // }
    if(MyRTC.bRTC_Sec_Flag)
    {
        int timestamp_temp;

        MyRTC.bRTC_Sec_Flag = 0;
        // ��ȡʱ���
        timestamp_temp = rtc_counter_get();
        MyRTC.vRTC_Convert_Time(timestamp_temp);
        printf("%d-%d-%d %d:%d:%d\r\n",MyRTC.ucYear,MyRTC.ucMon,MyRTC.ucDay,MyRTC.ucHour,MyRTC.ucMin,MyRTC.ucSecond);
    }
    if(MyRTC.bRTC_Alarm_Flag)
    {
        MyRTC.bRTC_Alarm_Flag = 0;
        MyRTC.vRTC_Alarm_Create();
        printf("���Ӵ���\r\n");
    }
}


/*
 * @description: ����0DMA��ʼ��
 * @param {uint32_t} hope_len ϣ�����յ����ݸ���
 * @return {*}
 * @Date: 2023-05-11 13:20:59
 */
// ����0DMA��ʼ��
static void svUSART0_Dma_Init(uint32_t hope_len)
{
    #define USART0_DATA_ADDRESS ((uint32_t)&USART_DATA(USART0))

    dma_parameter_struct myDMA1;
    // ʹ��DMA0ʱ��
    rcu_periph_clock_enable(RCU_DMA0);
    // ����ͨ��4�ж�
    nvic_irq_enable(DMA0_Channel4_IRQn,0,1);
    // ����
    dma_deinit(DMA0,DMA_CH4);
    dma_struct_para_init(&myDMA1);
    // ����--���赽�ڴ�
    myDMA1.direction = DMA_PERIPHERAL_TO_MEMORY;
    // �ڴ��Զ�����
    myDMA1.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    // �����ַ���Զ�����
    myDMA1.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    // DMA���ȼ�--��
    myDMA1.priority = DMA_PRIORITY_HIGH;
    // ��������С
    myDMA1.number = hope_len;
    // ���ݳ���--8λ
    myDMA1.memory_width = DMA_MEMORY_WIDTH_8BIT;
    // ���ջ�������ʼ��ַ
    myDMA1.memory_addr = (uint32_t)MyUart0.ucUart0_Rx_Buff;
    // �������ݿ��
    myDMA1.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    // ����Ĵ�����ַ
    myDMA1.periph_addr = USART0_DATA_ADDRESS;
    // ��ʼ��
    dma_init(DMA0,DMA_CH4,&myDMA1);
    // ѭ������ģʽ�ر�
    dma_circulation_disable(DMA0,DMA_CH4);
    // ���ݴ��䷽ʽ�����ڴ�--->�ڴ�
    dma_memory_to_memory_disable(DMA0,DMA_CH4);
    // ����DMA���ݽ���ʹ��
    usart_dma_receive_config(USART0,USART_RECEIVE_DMA_ENABLE);
    // ����DMA��������ж�ʹ��(�������ν)
    dma_interrupt_enable(DMA0,DMA_CH4,DMA_INT_FTF);
    // ����ָ����DMAͨ��
    dma_channel_enable(DMA0,DMA_CH4);
}


/*
 * @description: �������ڿ����ж�
 * @return {*}
 * @Date: 2023-05-11 14:29:22
 */
// �������ڿ����ж�
static void svUSART0_Enable_Idle_Interrupt(void)
{
    nvic_irq_enable(USART0_IRQn,0,0);
    usart_interrupt_enable(USART0,USART_INT_IDLE);
}

/*
 * @description: ��ʹ������ģʽ�����·���һ�δ��봫��
 * @return {*}
 * @Date: 2023-05-11 14:32:21
 */
// ��ʹ������ģʽ�����·���һ�δ��봫��
static void svUSART0_Again_Dma_Tx(void)
{
    svUSART0_Enable_Idle_Interrupt();
    dma_channel_disable(DMA0,DMA_CH4);
    DMA_CHCNT(DMA0,DMA_CH4) = UART0_MAX_LEN;
    dma_channel_enable(DMA0,DMA_CH4);
}

