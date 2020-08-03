#pragma once

#include <asf.h>

#include <lora/micro-driver-sx1276.h>
#include <print_array.h>

#define V6_ANTENNA_MATCHING_SELECTION_PIN  PIN_PA13
#define LORA_MAX_TRANSMIT_HANG_MS 1000

/**
 * Default values for LoRa radio parameters. These are used when performing a reset,
 * as well as when the value in storage is invalid.
 */
#define LORA_DEFAULT_FREQUENCY         868500000
#define LORA_DEFAULT_TRANSMIT_POWER    14
#define LORA_DEFAULT_BANDWIDTH         SX1276_BW_125K
#define LORA_DEFAULT_CODING_RATE       SX1276_CR_4_5
#define LORA_DEFAULT_SPREADING_FACTOR  10
#define LORA_DEFAULT_AMP_USES_RFO      true    // true=RFO LF/HF   false=PA_BOOST


bool init_update(void);
bool listen_update(void);
