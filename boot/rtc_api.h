#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void rtc_api_init(void);
void rtc_api_deinit(void);

uint32_t rtc_api_count(void);

#ifdef __cplusplus
}
#endif