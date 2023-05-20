#include "myUart1.h"

/*====================================变量区 BEGIN====================================*/
MyUart0_TypeDef MyUart0 = 
{
    .bUart0_Rx_Over_Flag = 0,
    .ucUart0_Rx_Len = 0,
    .ucUart0_Rx_Buff = {0},
    .vUART0_Init = &vUART0_Init,
    .vUSART0_Data_Process = &vUSART0_Data_Process
};

/*====================================变量区    END====================================*/

/*====================================静态内部函数声明区 BEGIN====================================*/
static void svUART0_Interrupt_Init(void);
static void svUSART0_Dma_Init(uint32_t hope_len);
static void svUSART0_Enable_Idle_Interrupt(void);
static void svUSART0_Again_Dma_Tx(void);
/*====================================静态内部函数声明区    END====================================*/


/*
 * @description: 串口1初始化
 * @return {*}
 * @Date: 2023-05-09 21:21:23
 */
// 串口1初始化
void vUART0_Init(void)
{
    // 使能串口1时钟
    rcu_periph_clock_enable(RCU_USART0);

// 如果引脚需要重映射
#ifdef USART0_REMAP
    // 使能GPIO时钟
    rcu_periph_clock_enable(GPIOB);
    rcu_periph_clock_enable(RCU_AF);
    // 串口1重映射使能
    gpio_pin_remap_config(GPIO_USART0_REMAP, ENABLE);
    // TX--复用推挽
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
    // RX--浮空输入
    gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_7);
#else
    // 使能GPIO时钟
    rcu_periph_clock_enable(RCU_GPIOA);
    // TX--复用推挽
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
    // RX--浮空输入
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
#endif
    // 串口参数初始化
    // 重置串口1
    usart_deinit(USART0);
    // 设置波特率
    usart_baudrate_set(USART0,115200U);
    // 设置数据长度
    usart_word_length_set(USART0,USART_WL_8BIT);
    // 设置停止位
    usart_stop_bit_set(USART0,USART_STB_1BIT);
    // 设置检验位
    usart_parity_config(USART0,USART_PM_NONE);
    // 硬件流控制--关闭
    usart_hardware_flow_rts_config(USART0,USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(USART0,USART_CTS_DISABLE);
    // 串口接收使能
    usart_receive_config(USART0,USART_RECEIVE_ENABLE);
    // 串口发送使能
    usart_transmit_config(USART0,USART_TRANSMIT_ENABLE);
    // 使能串口
    usart_enable(USART0);
    //串口中断初始化
    // svUART0_Interrupt_Init();
    // 串口DMA初始化
    svUSART0_Dma_Init(UART0_MAX_LEN);
    // 使能空闲中断
    svUSART0_Enable_Idle_Interrupt();
}



/*
 * @description: 串口1重定向
 * @param {int} ch
 * @param {FILE} *f
 * @return {*}
 * @Date: 2023-05-10 14:53:40
 */
// 串口1重定向
int fputc(int ch,FILE *f)
{
    usart_data_transmit(USART0,(uint8_t)ch);
    int Cnt = 1000;
    while(RESET == usart_flag_get(USART0,USART_FLAG_TBE) && Cnt--);
    return ch;
}

/*
 * @description: 使能串口0中断
 * @return {*}
 * @Date: 2023-05-10 16:02:09
 */
// 使能串口0中断
static void svUART0_Interrupt_Init(void)
{
    // 中断管理器使能，并分配优先级
    nvic_irq_enable(USART0_IRQn,1,1);
    // 清除中断标志
    usart_interrupt_flag_clear(USART0,USART_INT_FLAG_RBNE);
    usart_interrupt_flag_clear(USART0,USART_INT_FLAG_IDLE);
    // 使能串口中断
    usart_interrupt_enable(USART0,USART_INT_RBNE);  // 读数据缓冲区非空中断和过载错误中断
    usart_interrupt_enable(USART0,USART_INT_IDLE);  // IDLE线检测中断
}

/*
 * @description: 串口0中断服务函数
 * @return {*}
 * @Date: 2023-05-10 18:43:08
 */
// 串口0中断服务函数
void USART0_IRQHandler(void)
{
#if 0
    // 读取缓冲区不为空
    if(SET == usart_interrupt_flag_get(USART0,USART_INT_FLAG_RBNE))
    {
        // 读取数据存储起来
        MyUart0.ucUart0_Rx_Buff[MyUart0.ucUart0_Rx_Len++] = usart_data_receive(USART0);
    }
    else if(SET == usart_interrupt_flag_get(USART0,USART_INT_FLAG_IDLE))
    {
        // 清除空闲中断标志位
        usart_data_receive(USART0);
        // 另一种清除空闲中断标志位方法
        usart_interrupt_flag_clear(USART0,USART_INT_FLAG_IDLE);
        // 接收完一帧数据标志位
        MyUart0.bUart0_Rx_Over_Flag = 1;
    }
#endif
    if (SET == usart_interrupt_flag_get(USART0, USART_INT_FLAG_IDLE))
    {
        // 清除标志位
        usart_data_receive(USART0);
        // 禁能外设DMAx的通道y传输
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
 * @description: DMA中断函数
 * @return {*}
 * @Date: 2023-05-11 14:49:08
 */
// DMA中断函数
void DMA0_Channel4_IRQHandler(void)
{
    // DMA通道传输完成标志
    if(dma_interrupt_flag_get(DMA0,DMA_CH4,DMA_INT_FLAG_FTF))
    {
        // 清除DMA中断标志
        dma_interrupt_flag_clear(DMA0,DMA_CH4,DMA_INT_FLAG_G);
    }
}

/*
 * @description: 对接收的数据进行处理
 * @return {*}
 * @Date: 2023-05-10 18:45:09
 */
// 对接收的数据进行处理
void vUSART0_Data_Process(void)
{
    if(MyUart0.bUart0_Rx_Over_Flag)
    {
        MyUart0.bUart0_Rx_Over_Flag = 0;
        printf("Read Len: %d ",MyUart0.ucUart0_Rx_Len);
        // 数组清0，索引清0
        for(uint8_t i = 0; i < MyUart0.ucUart0_Rx_Len; i++)
        {
            printf("%02x  ",(uint32_t)MyUart0.ucUart0_Rx_Buff[i]);
        }
        printf("\r\n");
        memset(MyUart0.ucUart0_Rx_Buff,0,sizeof(MyUart0.ucUart0_Rx_Buff));
        MyUart0.ucUart0_Rx_Len = 0;
        // 重新打开DMA接收
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
        // 获取时间戳
        timestamp_temp = rtc_counter_get();
        MyRTC.vRTC_Convert_Time(timestamp_temp);
        printf("%d-%d-%d %d:%d:%d\r\n",MyRTC.ucYear,MyRTC.ucMon,MyRTC.ucDay,MyRTC.ucHour,MyRTC.ucMin,MyRTC.ucSecond);
    }
    if(MyRTC.bRTC_Alarm_Flag)
    {
        MyRTC.bRTC_Alarm_Flag = 0;
        MyRTC.vRTC_Alarm_Create();
        printf("闹钟触发\r\n");
    }
}


/*
 * @description: 串口0DMA初始化
 * @param {uint32_t} hope_len 希望接收的数据个数
 * @return {*}
 * @Date: 2023-05-11 13:20:59
 */
// 串口0DMA初始化
static void svUSART0_Dma_Init(uint32_t hope_len)
{
    #define USART0_DATA_ADDRESS ((uint32_t)&USART_DATA(USART0))

    dma_parameter_struct myDMA1;
    // 使能DMA0时钟
    rcu_periph_clock_enable(RCU_DMA0);
    // 开启通道4中断
    nvic_irq_enable(DMA0_Channel4_IRQn,0,1);
    // 重置
    dma_deinit(DMA0,DMA_CH4);
    dma_struct_para_init(&myDMA1);
    // 方向--外设到内存
    myDMA1.direction = DMA_PERIPHERAL_TO_MEMORY;
    // 内存自动增长
    myDMA1.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    // 外设地址不自动增长
    myDMA1.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    // DMA优先级--高
    myDMA1.priority = DMA_PRIORITY_HIGH;
    // 缓冲区大小
    myDMA1.number = hope_len;
    // 数据长度--8位
    myDMA1.memory_width = DMA_MEMORY_WIDTH_8BIT;
    // 接收缓冲区开始地址
    myDMA1.memory_addr = (uint32_t)MyUart0.ucUart0_Rx_Buff;
    // 外输数据宽度
    myDMA1.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    // 外设寄存器地址
    myDMA1.periph_addr = USART0_DATA_ADDRESS;
    // 初始化
    dma_init(DMA0,DMA_CH4,&myDMA1);
    // 循环传输模式关闭
    dma_circulation_disable(DMA0,DMA_CH4);
    // 数据传输方式不是内存--->内存
    dma_memory_to_memory_disable(DMA0,DMA_CH4);
    // 串口DMA数据接收使能
    usart_dma_receive_config(USART0,USART_RECEIVE_DMA_ENABLE);
    // 串口DMA接收完成中断使能(这个无所谓)
    dma_interrupt_enable(DMA0,DMA_CH4,DMA_INT_FTF);
    // 启动指定的DMA通道
    dma_channel_enable(DMA0,DMA_CH4);
}


/*
 * @description: 开启串口空闲中断
 * @return {*}
 * @Date: 2023-05-11 14:29:22
 */
// 开启串口空闲中断
static void svUSART0_Enable_Idle_Interrupt(void)
{
    nvic_irq_enable(USART0_IRQn,0,0);
    usart_interrupt_enable(USART0,USART_INT_IDLE);
}

/*
 * @description: 不使用连续模式下重新发起一次代码传输
 * @return {*}
 * @Date: 2023-05-11 14:32:21
 */
// 不使用连续模式下重新发起一次代码传输
static void svUSART0_Again_Dma_Tx(void)
{
    svUSART0_Enable_Idle_Interrupt();
    dma_channel_disable(DMA0,DMA_CH4);
    DMA_CHCNT(DMA0,DMA_CH4) = UART0_MAX_LEN;
    dma_channel_enable(DMA0,DMA_CH4);
}

