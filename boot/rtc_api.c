#include "rtc_api.h"

#include <port.h>
#include <rtc_count.h>

static struct rtc_module rtc_instance;

void rtc_api_init(void)
{
    // Configure "RTC" counter
    struct rtc_count_config config_rtc_count;
    rtc_count_get_config_defaults(&config_rtc_count);
    config_rtc_count.prescaler = RTC_COUNT_PRESCALER_DIV_1;
    config_rtc_count.mode = RTC_COUNT_MODE_32BIT;
    rtc_count_init(&rtc_instance, RTC, &config_rtc_count);
    rtc_count_enable(&rtc_instance);
}

void rtc_api_deinit(void)
{
    rtc_count_disable(&rtc_instance);
}

uint32_t rtc_api_count(void)
{
    return rtc_count_get_count(&rtc_instance);
}