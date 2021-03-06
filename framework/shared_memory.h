#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void shared_memory_init(void);
bool shared_memory_is_dfu_requested(void);
void shared_memory_set_dfu_requested(bool yes);
void shared_memory_increment_boot_counter(void);
void shared_memory_clear_boot_counter(void);
uint8_t shared_memory_get_boot_counter(void);

void shared_memory_set_update_id(uint8_t id);
uint8_t shared_memory_get_update_id();

#ifdef __cplusplus
}
#endif