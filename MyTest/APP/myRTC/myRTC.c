#include "myRTC.h"

/*====================================变量区 BEGIN====================================*/
MyRTC_TypeDef MyRTC = 
{
    .bRTC_Alarm_Flag = 0,
    .bRTC_Sec_Flag = 0,
    .ucYear = 0,
    .ucMon = 0,
    .ucDay = 0,
    .ucHour = 0,
    .ucMin = 0,
    .ucSecond = 0,
    .vRTC_Init = &vRTC_Init,
    .vRTC_Convert_Time = &vRTC_Convert_Time,
    .vRTC_Alarm_Create = &vRTC_Alarm_Create
};

static int DAYS = 24 * 3600;
static int FOURSYEARS = 365 * 3 + 366;
static int norMoth[] = {31,28,31,30,31,30,31,31,30,31,30,31};
static int leapMoth[] = {31,29,31,30,31,30,31,31,30,31,30,31};

/*====================================变量区    END====================================*/

/*====================================静态内部函数声明区 BEGIN====================================*/
static void svRTC_Config(void);
static void svRTC_Time_Set(uint32_t timestamp_value);
static void svRTC_Get_Hour_Min_Sec(uint32_t timestamp, uint16_t *hour, uint16_t *min, uint16_t *sec);
static void svRTC_Get_Mon_Day(bool year_state,int nDays,uint16_t *nMon,uint16_t *nDay);
/*====================================静态内部函数声明区    END====================================*/

/*
 * @description: RTC配置
 * @return {*}
 * @Date: 2023-05-15 15:26:05
 */
// RTC配置
static void svRTC_Config(void)
{
    // 备份区域的时钟使能
    rcu_periph_clock_enable(RCU_BKPI);
    // 电源管理时钟使能
    rcu_periph_clock_enable(RCU_PMU);
    // 备份区允许访问
    pmu_backup_write_enable();
    // 备份域复位
    bkp_deinit();
    // 打开振荡器---使能外部低速时钟32.768K
    rcu_osci_on(RCU_LXTAL);
    // 等待低速振荡器稳定
    rcu_osci_stab_wait(RCU_LXTAL);
    // RTC时钟源选择
    rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);
    // 使能RTC时钟
    rcu_periph_clock_enable(RCU_RTC);
    // 进入RTC配置模式
    rtc_configuration_mode_enter();
     // 等待寄存器与APB1时钟同步
    rtc_register_sync_wait();
    // 等待最后一次操作完成
    rtc_lwoff_wait();
    // 使能秒中断
    rtc_interrupt_enable(RTC_INT_SECOND);
    // 等待配置完成
    rtc_lwoff_wait();
    // 设置预分频,1s
    rtc_prescaler_set(32767);
    // 等待配置完成
    rtc_lwoff_wait();
}

/*
 * @description: 设置时间
 * @param {uint32_t} timestamp_value 时间戳
 * @return {*}
 * @Date: 2023-05-15 15:28:46
 */
// 设置时间
static void svRTC_Time_Set(uint32_t timestamp_value)
{
    // 等待最后一次操作完成
    rtc_lwoff_wait();
    // 改变当前时间
    rtc_counter_set(timestamp_value);
    // 等待最后一次操作完成
    rtc_lwoff_wait();
}

/*
 * @description: RTC初始化
 * @return {*}
 * @Date: 2023-05-15 15:26:31
 */
// RTC初始化
void vRTC_Init(void)
{
    printf("RTC中断初始化\r\n");
    // 读取备份区寄存器0的数据，检查是否已经配置
    if(bkp_data_read(BKP_DATA0) != 0xA5A5)
    {
        // 说明没配置过
        printf("RTC 没有配置过\r\n");
        // 配置
        svRTC_Config();
        printf("RTC配置完成\r\n");
        // 首次调整时间，传入一个时间戳
        svRTC_Time_Set(1684142909);
        bkp_data_write(BKP_DATA_0,0xA5A5);
    }
    else
    {
        // 判断是不是电源复位标志
        if(SET == rcu_flag_get(RCU_FLAG_PORRST))
        {
            printf("电源复位\r\n");
        }
        // 判断是不是软件复位标志
        else if(SET == rcu_flag_get(RCU_FLAG_SWRST))
        {
            printf("软件复位\r\n");
        }
        // 判断是不是外部引脚复位标志
        else if(SET == rcu_flag_get(RCU_FLAG_EPRST))
        {
            printf("外部引脚复位\r\n");
        }
        // 等待寄存器与APB1时钟同步
        rtc_register_sync_wait();
        // 使能秒中断
        rtc_interrupt_enable(RTC_INT_SECOND);
        // 等待配置完成
        rtc_lwoff_wait();
    }
    // 使能中断
    nvic_irq_enable(RTC_IRQn,1,0);
    // 退出配置模式， 更新配置
    rtc_configuration_mode_exit();
    // 清除所有标志
    rcu_all_reset_flag_clear();
}

/*
 * @description: RTC中断函数
 * @return {*}
 * @Date: 2023-05-15 16:11:08
 */
// RTC中断函数
void RTC_IRQHandler(void)
{
    if(SET == rtc_interrupt_flag_get(RTC_INT_FLAG_SECOND))
    {
        // 清除标志位
        rtc_interrupt_flag_clear(RTC_INT_FLAG_SECOND);
        MyRTC.bRTC_Sec_Flag = 1;
    }
    if(SET == rtc_interrupt_flag_get(RTC_FLAG_ALARM))
    {
        // 清除标志位
        rtc_interrupt_flag_clear(RTC_FLAG_ALARM);
        MyRTC.bRTC_Alarm_Flag = 1;
    }
}

/*
 * @description: 获取时分秒
 * @param {uint32_t} timestamp_value
 * @return {*}
 * @Date: 2023-05-15 16:41:50
 */
// 获取时分秒
static void svRTC_Get_Hour_Min_Sec(uint32_t timestamp, uint16_t *hour, uint16_t *min, uint16_t *sec)
{
    // 计算当前时间的秒数
    uint32_t current_seconds = timestamp % 60;
    // 分别计算当前时间的总分钟数和分钟部分
    uint32_t total_minutes = timestamp / 60;
    uint32_t current_minutes = total_minutes % 60;
    // 分别计算当前时间的总小时数和小时部分
    uint32_t total_hours = total_minutes / 60;
    uint32_t current_hours = (total_hours + 8) % 24;  // 以东八区的北京时间为准
    // 赋值
    *hour = current_hours;
    *min = current_minutes;
    *sec = current_seconds;
}

/*
 * @description: 获取月份日期
 * @param {bool} year_state
 * @param {int} nDay
 * @param {uint16_t} *nMon
 * @param {uint16_t} *nDay
 * @return {*}
 * @Date: 2023-05-15 16:45:01
 */
// 获取月份日期
static void svRTC_Get_Mon_Day(bool year_state,int nDays,uint16_t *nMon,uint16_t *nDay)
{
    int nTemp = 0;
    // // 根据是否是闰年来选择月份天数数组
    int *pMon = year_state ? leapMoth : norMoth;

    // 遍历月份
    for(uint8_t i = 0;i < 12; i++)
    {
        // 计算天数
        nTemp = nDays - pMon[i];
        // 找到对应的月份
        if(nTemp <= 0)
        {
            // 存储月份
            *nMon = i + 1;
            // 如果正好等于本月最后一天
            if(0 == nTemp)
            {
                // 存储本月最后一天
                *nDay = pMon[i];
            }
            // 否则就存储剩余天数
            else
            {
                // 存储天数
                *nDay = nDays;
            }
            break;
        }
        // 计算剩余天数
        nDays = nTemp;
    }
    return;
}

void vRTC_Convert_Time(uint32_t timestamp_value)
{
    // 计算天数，加上余数不为0就再加一天
    int nDays = timestamp_value / DAYS + ((timestamp_value % DAYS) ? 1 : 0);
    // 计算整除四年的个数
    int nYear4 = nDays / FOURSYEARS;
    // 计算剩余天数
    int nRemain = nDays % FOURSYEARS;
    // 计算公历年份
    int nDecyear = 1970 + nYear4 * 4;
    int nDemon = 0;
    int nDeday = 0;
    // 是否闰年的标志
    bool year_Flag = 0;

    // 剩余天数小于365，说明是当年的日期
    if(nRemain < 365)
    {
        ;
    }
    // 剩余天数小于365*2，说明是第二年的日期
    else if(nRemain < (365 * 2))
    {
        nDecyear += 1;
        // 减去第一年的天数
        nRemain -= 365;
    }
    // 剩余天数小于365*3，说明是第三年的日期
    else if(nRemain < (365 * 3))
    {
        nDecyear += 2;
        // 减去前两年的天数
        nRemain -= 365 * 2;
    }
    // 否则就是第四年的日期
    else
    {
        nDecyear += 3;
        // 减去前三年的天数
        nRemain -= 365 * 3;
        // 设为闰年
        year_Flag = 1;
    }
    svRTC_Get_Mon_Day(year_Flag,nRemain,&MyRTC.ucMon,&MyRTC.ucDay);
    svRTC_Get_Hour_Min_Sec(timestamp_value,&MyRTC.ucHour,&MyRTC.ucMin,&MyRTC.ucSecond);
    MyRTC.ucYear = nDecyear;
}

/*
 * @description: 创建一个闹钟
 * @return {*}
 * @Date: 2023-05-15 18:31:46
 */
// 创建一个闹钟
void vRTC_Alarm_Create(void)
{
    // 备份区域的时钟使能
    rcu_periph_clock_enable(RCU_BKPI);
    // 电源管理时钟使能
    rcu_periph_clock_enable(RCU_PMU);
    // 备份区域允许访问
    pmu_backup_write_enable();
    // 等待寄存器与APB1时钟同步
    rtc_register_sync_wait();
    // 等待最后一次完成
    rtc_lwoff_wait();
    uint32_t tick = rtc_counter_get() + 10;
    // 等待最后一次完成
    rtc_lwoff_wait();
    rtc_alarm_config(tick);
    // 使能闹钟中断
    rtc_interrupt_enable(RTC_INT_ALARM);
    rtc_alarm_config(tick);
    pmu_backup_write_disable();
}
