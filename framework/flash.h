#pragma once

#include <stdint.h>

#include <nvm.h>


#ifdef __cplusplus
extern "C" {
#endif

void config_nvm(void);
void write_firmware_demo(void);
void debug_read_app_flash(uint8_t eitherSide, uint8_t* page_buffer);
void erase_application(void);
void write_page(uint32_t page, uint8_t* change_page_buffer);
void update_page(uint32_t page, uint8_t *change_page_buffer);
void read_page(uint32_t page, uint8_t *output_page);
#ifdef __cplusplus
}
#endif