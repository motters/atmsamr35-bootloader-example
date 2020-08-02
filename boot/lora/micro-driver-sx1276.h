#pragma once

#include "stdint.h"


#include <asf.h>

#include "../print_array.h"

#define LORA_NOT_REQUIRED 0

typedef enum
{
    SX1276_SLEEP = 0,
    SX1276_STANDBY = 1,
    SX1276_TRANSMIT = 2,
    SX1276_RECEIVE_CONTINUOUS = 3,
    SX1276_RECEIVE_SINGLE = 4
} SX1276_device_mode_TypeDef;
typedef enum
{
    SX1276_BW_7K8 = 0,
    SX1276_BW_10K4 = 1,
    SX1276_BW_15K6 = 2,
    SX1276_BW_20K8 = 3,
    SX1276_BW_31K25 = 4,
    SX1276_BW_41K7 = 5,
    SX1276_BW_62K5 = 6,
    SX1276_BW_125K = 7,
    SX1276_BW_250K = 8,
    SX1276_BW_500K = 9
} SX1276_bandwidth_TypeDef;
typedef enum
{
    SX1276_CR_4_5 = 0,
    SX1276_CR_4_6 = 1,
    SX1276_CR_4_7 = 2,
    SX1276_CR_4_8 = 3
} SX1276_coding_rate_TypeDef;

void SX1276_configure_hardware(void);
void SX1276_reset_device(void);
bool SX1276_setup_device(uint32_t frequency,
                         uint8_t transmit_power,
                         SX1276_bandwidth_TypeDef bandwidth,
                         SX1276_coding_rate_TypeDef coding_rate,
                         uint8_t spreading_factor,
                         bool use_rfo);

uint8_t SX1276_read_register(uint8_t address);
void SX1276_write_register(uint8_t address, uint8_t value);

void SX1276_send_message(uint8_t *pTxData, uint8_t length);
uint8_t SX1276_receive_message(uint8_t *pRxData);

void SX1276_set_device_mode(SX1276_device_mode_TypeDef mode);
SX1276_device_mode_TypeDef SX1276_get_device_mode(void);

void SX1276_set_long_range_mode(uint8_t is_lora);
uint8_t SX1276_get_long_range_mode(void);

void SX1276_set_preamble_length(uint16_t preamble_length);

void SX1276_set_frequency(uint32_t frequency);

void SX1276_set_transmit_power(uint8_t transmit_power, bool use_rfo);

void SX1276_set_bandwidth(SX1276_bandwidth_TypeDef bandwidth);
void SX1276_set_coding_rate(SX1276_coding_rate_TypeDef coding_rate);
void SX1276_set_header_mode(uint8_t is_implicit);
void SX1276_set_spreading_factor(uint8_t spreading_factor);
void SX1276_set_rx_payload_crc(uint8_t is_extracted);

uint8_t SX1276_read_interrupt_flags(void);
void SX1276_clear_recieve_done_flag(void);
void SX1276_clear_payload_crc_error_flag(void);
void SX1276_clear_transmit_done_flag(void);


#if LORA_NOT_REQUIRED == 1
uint32_t SX1276_get_frequency(void);
#endif

#define SX1276_RECEIVE_DONE_FLAG       0x40
#define SX1276_PAYLOAD_CRC_ERROR_FLAG  0x20
#define SX1276_TRANSMIT_DONE_FLAG      0x08

