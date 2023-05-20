#include "myRTC.h"

/*====================================������ BEGIN====================================*/
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

/*====================================������    END====================================*/

/*====================================��̬�ڲ����������� BEGIN====================================*/
static void svRTC_Config(void);
static void svRTC_Time_Set(uint32_t timestamp_value);
static void svRTC_Get_Hour_Min_Sec(uint32_t timestamp, uint16_t *hour, uint16_t *min, uint16_t *sec);
static void svRTC_Get_Mon_Day(bool year_state,int nDays,uint16_t *nMon,uint16_t *nDay);
/*====================================��̬�ڲ�����������    END====================================*/

/*
 * @description: RTC����
 * @return {*}
 * @Date: 2023-05-15 15:26:05
 */
// RTC����
static void svRTC_Config(void)
{
    // ���������ʱ��ʹ��
    rcu_periph_clock_enable(RCU_BKPI);
    // ��Դ����ʱ��ʹ��
    rcu_periph_clock_enable(RCU_PMU);
    // �������������
    pmu_backup_write_enable();
    // ������λ
    bkp_deinit();
    // ������---ʹ���ⲿ����ʱ��32.768K
    rcu_osci_on(RCU_LXTAL);
    // �ȴ����������ȶ�
    rcu_osci_stab_wait(RCU_LXTAL);
    // RTCʱ��Դѡ��
    rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);
    // ʹ��RTCʱ��
    rcu_periph_clock_enable(RCU_RTC);
    // ����RTC����ģʽ
    rtc_configuration_mode_enter();
     // �ȴ��Ĵ�����APB1ʱ��ͬ��
    rtc_register_sync_wait();
    // �ȴ����һ�β������
    rtc_lwoff_wait();
    // ʹ�����ж�
    rtc_interrupt_enable(RTC_INT_SECOND);
    // �ȴ��������
    rtc_lwoff_wait();
    // ����Ԥ��Ƶ,1s
    rtc_prescaler_set(32767);
    // �ȴ��������
    rtc_lwoff_wait();
}

/*
 * @description: ����ʱ��
 * @param {uint32_t} timestamp_value ʱ���
 * @return {*}
 * @Date: 2023-05-15 15:28:46
 */
// ����ʱ��
static void svRTC_Time_Set(uint32_t timestamp_value)
{
    // �ȴ����һ�β������
    rtc_lwoff_wait();
    // �ı䵱ǰʱ��
    rtc_counter_set(timestamp_value);
    // �ȴ����һ�β������
    rtc_lwoff_wait();
}

/*
 * @description: RTC��ʼ��
 * @return {*}
 * @Date: 2023-05-15 15:26:31
 */
// RTC��ʼ��
void vRTC_Init(void)
{
    printf("RTC�жϳ�ʼ��\r\n");
    // ��ȡ�������Ĵ���0�����ݣ�����Ƿ��Ѿ�����
    if(bkp_data_read(BKP_DATA0) != 0xA5A5)
    {
        // ˵��û���ù�
        printf("RTC û�����ù�\r\n");
        // ����
        svRTC_Config();
        printf("RTC�������\r\n");
        // �״ε���ʱ�䣬����һ��ʱ���
        svRTC_Time_Set(1684142909);
        bkp_data_write(BKP_DATA_0,0xA5A5);
    }
    else
    {
        // �ж��ǲ��ǵ�Դ��λ��־
        if(SET == rcu_flag_get(RCU_FLAG_PORRST))
        {
            printf("��Դ��λ\r\n");
        }
        // �ж��ǲ��������λ��־
        else if(SET == rcu_flag_get(RCU_FLAG_SWRST))
        {
            printf("�����λ\r\n");
        }
        // �ж��ǲ����ⲿ���Ÿ�λ��־
        else if(SET == rcu_flag_get(RCU_FLAG_EPRST))
        {
            printf("�ⲿ���Ÿ�λ\r\n");
        }
        // �ȴ��Ĵ�����APB1ʱ��ͬ��
        rtc_register_sync_wait();
        // ʹ�����ж�
        rtc_interrupt_enable(RTC_INT_SECOND);
        // �ȴ��������
        rtc_lwoff_wait();
    }
    // ʹ���ж�
    nvic_irq_enable(RTC_IRQn,1,0);
    // �˳�����ģʽ�� ��������
    rtc_configuration_mode_exit();
    // ������б�־
    rcu_all_reset_flag_clear();
}

/*
 * @description: RTC�жϺ���
 * @return {*}
 * @Date: 2023-05-15 16:11:08
 */
// RTC�жϺ���
void RTC_IRQHandler(void)
{
    if(SET == rtc_interrupt_flag_get(RTC_INT_FLAG_SECOND))
    {
        // �����־λ
        rtc_interrupt_flag_clear(RTC_INT_FLAG_SECOND);
        MyRTC.bRTC_Sec_Flag = 1;
    }
    if(SET == rtc_interrupt_flag_get(RTC_FLAG_ALARM))
    {
        // �����־λ
        rtc_interrupt_flag_clear(RTC_FLAG_ALARM);
        MyRTC.bRTC_Alarm_Flag = 1;
    }
}

/*
 * @description: ��ȡʱ����
 * @param {uint32_t} timestamp_value
 * @return {*}
 * @Date: 2023-05-15 16:41:50
 */
// ��ȡʱ����
static void svRTC_Get_Hour_Min_Sec(uint32_t timestamp, uint16_t *hour, uint16_t *min, uint16_t *sec)
{
    // ���㵱ǰʱ�������
    uint32_t current_seconds = timestamp % 60;
    // �ֱ���㵱ǰʱ����ܷ������ͷ��Ӳ���
    uint32_t total_minutes = timestamp / 60;
    uint32_t current_minutes = total_minutes % 60;
    // �ֱ���㵱ǰʱ�����Сʱ����Сʱ����
    uint32_t total_hours = total_minutes / 60;
    uint32_t current_hours = (total_hours + 8) % 24;  // �Զ������ı���ʱ��Ϊ׼
    // ��ֵ
    *hour = current_hours;
    *min = current_minutes;
    *sec = current_seconds;
}

/*
 * @description: ��ȡ�·�����
 * @param {bool} year_state
 * @param {int} nDay
 * @param {uint16_t} *nMon
 * @param {uint16_t} *nDay
 * @return {*}
 * @Date: 2023-05-15 16:45:01
 */
// ��ȡ�·�����
static void svRTC_Get_Mon_Day(bool year_state,int nDays,uint16_t *nMon,uint16_t *nDay)
{
    int nTemp = 0;
    // // �����Ƿ���������ѡ���·���������
    int *pMon = year_state ? leapMoth : norMoth;

    // �����·�
    for(uint8_t i = 0;i < 12; i++)
    {
        // ��������
        nTemp = nDays - pMon[i];
        // �ҵ���Ӧ���·�
        if(nTemp <= 0)
        {
            // �洢�·�
            *nMon = i + 1;
            // ������õ��ڱ������һ��
            if(0 == nTemp)
            {
                // �洢�������һ��
                *nDay = pMon[i];
            }
            // ����ʹ洢ʣ������
            else
            {
                // �洢����
                *nDay = nDays;
            }
            break;
        }
        // ����ʣ������
        nDays = nTemp;
    }
    return;
}

void vRTC_Convert_Time(uint32_t timestamp_value)
{
    // ��������������������Ϊ0���ټ�һ��
    int nDays = timestamp_value / DAYS + ((timestamp_value % DAYS) ? 1 : 0);
    // ������������ĸ���
    int nYear4 = nDays / FOURSYEARS;
    // ����ʣ������
    int nRemain = nDays % FOURSYEARS;
    // ���㹫�����
    int nDecyear = 1970 + nYear4 * 4;
    int nDemon = 0;
    int nDeday = 0;
    // �Ƿ�����ı�־
    bool year_Flag = 0;

    // ʣ������С��365��˵���ǵ��������
    if(nRemain < 365)
    {
        ;
    }
    // ʣ������С��365*2��˵���ǵڶ��������
    else if(nRemain < (365 * 2))
    {
        nDecyear += 1;
        // ��ȥ��һ�������
        nRemain -= 365;
    }
    // ʣ������С��365*3��˵���ǵ����������
    else if(nRemain < (365 * 3))
    {
        nDecyear += 2;
        // ��ȥǰ���������
        nRemain -= 365 * 2;
    }
    // ������ǵ����������
    else
    {
        nDecyear += 3;
        // ��ȥǰ���������
        nRemain -= 365 * 3;
        // ��Ϊ����
        year_Flag = 1;
    }
    svRTC_Get_Mon_Day(year_Flag,nRemain,&MyRTC.ucMon,&MyRTC.ucDay);
    svRTC_Get_Hour_Min_Sec(timestamp_value,&MyRTC.ucHour,&MyRTC.ucMin,&MyRTC.ucSecond);
    MyRTC.ucYear = nDecyear;
}

/*
 * @description: ����һ������
 * @return {*}
 * @Date: 2023-05-15 18:31:46
 */
// ����һ������
void vRTC_Alarm_Create(void)
{
    // ���������ʱ��ʹ��
    rcu_periph_clock_enable(RCU_BKPI);
    // ��Դ����ʱ��ʹ��
    rcu_periph_clock_enable(RCU_PMU);
    // ���������������
    pmu_backup_write_enable();
    // �ȴ��Ĵ�����APB1ʱ��ͬ��
    rtc_register_sync_wait();
    // �ȴ����һ�����
    rtc_lwoff_wait();
    uint32_t tick = rtc_counter_get() + 10;
    // �ȴ����һ�����
    rtc_lwoff_wait();
    rtc_alarm_config(tick);
    // ʹ�������ж�
    rtc_interrupt_enable(RTC_INT_ALARM);
    rtc_alarm_config(tick);
    pmu_backup_write_disable();
}
