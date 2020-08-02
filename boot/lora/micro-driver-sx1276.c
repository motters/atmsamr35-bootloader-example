#include "micro-driver-sx1276.h"


#include "micro-sx1276-registers.h"
#include <spi.h>
#include <port.h>
#include <delay.h>


const size_t NUM_DEVICE_MODES = 5;
const uint8_t DEVICE_MODE_VALUES[5] = {0b000, 0b001, 0b011, 0b101, 0b110};

const uint8_t BANDWIDTH_VALUES[10] = {
    0x00, 0x10, 0x20, 0x30, 0x40,
    0x50, 0x60, 0x70, 0x80, 0x90};
const uint8_t CODING_RATE_VALUES[4] = {0x02, 0x04, 0x06, 0x08};

const uint8_t DIO_MAPPING_RECEIVE = 0x00;
const uint8_t DIO_MAPPING_TRANSMIT = 0x40;

const uint8_t LORA_MODE = 0x80;
const uint8_t FSK_OOK_MODE = 0x00;

const uint8_t MODEM_CONFIG_VAL_3 = 0x00;

const uint32_t OSCILLATOR_FREQUENCY = 32000000;

static struct spi_module sx_spi_master;
struct spi_slave_inst sx_spi_cs_instance;

/**
 * @brief Sets up the GPIO and SPI hardware used to communicate with the
 *        SX1276 device.
 */
void SX1276_configure_hardware(void) {
    // Configure GPIO
    struct port_config pin_conf;
    port_get_config_defaults(&pin_conf);
    pin_conf.direction  = PORT_PIN_DIR_OUTPUT;
    port_pin_set_config(SX_RF_SPI_SCK, &pin_conf);
    port_pin_set_config(SX_RF_SPI_MOSI, &pin_conf);
    port_pin_set_config(SX_RF_SPI_CS, &pin_conf);
    port_pin_set_config(SX_RF_RESET_PIN, &pin_conf);
    port_pin_set_output_level(SX_RF_SPI_SCK, true);
    port_pin_set_output_level(SX_RF_SPI_MOSI, true);
    port_pin_set_output_level(SX_RF_SPI_CS, true);
    port_pin_set_output_level(SX_RF_RESET_PIN, true);
    pin_conf.direction  = PORT_PIN_DIR_INPUT;
    port_pin_set_config(SX_RF_SPI_MISO, &pin_conf);



    // Configure SPI
    struct spi_config config_sx_spi_master;
    struct spi_slave_inst_config sx_spi_cs_config;
    spi_slave_inst_get_config_defaults(&sx_spi_cs_config);
    sx_spi_cs_config.ss_pin = SX_RF_SPI_CS;
    spi_attach_slave(&sx_spi_cs_instance, &sx_spi_cs_config);
    spi_get_config_defaults(&config_sx_spi_master);
    config_sx_spi_master.mode_specific.master.baudrate = SX_RF_SPI_BAUDRATE;
    config_sx_spi_master.mux_setting = SX_RF_SPI_SERCOM_MUX_SETTING;
    config_sx_spi_master.pinmux_pad0 = SX_RF_SPI_SERCOM_PINMUX_PAD0;
    config_sx_spi_master.pinmux_pad1 = PINMUX_UNUSED;
    config_sx_spi_master.pinmux_pad2 = SX_RF_SPI_SERCOM_PINMUX_PAD2;
    config_sx_spi_master.pinmux_pad3 = SX_RF_SPI_SERCOM_PINMUX_PAD3;
    spi_init(&sx_spi_master, SX_RF_SPI, &config_sx_spi_master);
    spi_enable(&sx_spi_master);
}

/**
 * @brief Resets the SX1276 device using the reset pin.
 */
void SX1276_reset_device(void) {
    port_pin_set_output_level(SX_RF_RESET_PIN, SX_RF_RESET_HIGH);
    delay_ms(5);
    port_pin_set_output_level(SX_RF_RESET_PIN, SX_RF_RESET_LOW);
    delay_ms(1);
    port_pin_set_output_level(SX_RF_RESET_PIN, SX_RF_RESET_HIGH);
    delay_ms(5);
}


/**
 * @brief Sets up the device for LoRa use. Puts the device in standby mode, from
 * which messages can be sent or received.
 * @param frequency: The carrier frequency, in Hz.
 * @param transmit_power: The transmit power in dBm.
 * @param bandwidth: The signal bandwidth.
 * @param coding_rate: The coding rate.
 * @param spreading_factor: The spreading factor.
 * @param use_rfo: Whether to use the RFO (true/1) or PA_BOOST (false/0) power
 *     amplifier pin.
 */
bool SX1276_setup_device(uint32_t frequency,
                         uint8_t transmit_power,
                         SX1276_bandwidth_TypeDef bandwidth,
                         SX1276_coding_rate_TypeDef coding_rate,
                         uint8_t spreading_factor,
                         bool use_rfo) {
    // Reset before setup
    SX1276_reset_device();

    // Put in sleep mode, so that other config parameters can be set.
    SX1276_set_device_mode(SX1276_SLEEP);
    delay_ms(10);
    if (SX1276_get_device_mode() != SX1276_SLEEP)
        return false;

    // Set LoRa mode
    SX1276_set_long_range_mode(1);
    if (!SX1276_get_long_range_mode())
        return false;

    // Set Tx and Rf FIFO pointers to 0 for maximum FIFO size (but no
    // simultaneous Tx+Rx)
    SX1276_write_register(SX1276_FIFO_TX_BASE_ADDR, 0x00);
    SX1276_write_register(SX1276_FIFO_RX_BASE_ADDR, 0x00);

    SX1276_set_device_mode(SX1276_STANDBY);

    // Set frequency and modem config
    SX1276_set_bandwidth(bandwidth);
    SX1276_set_coding_rate(coding_rate);
    SX1276_set_header_mode(0);
    SX1276_set_spreading_factor(spreading_factor);
    SX1276_set_rx_payload_crc(0);
    SX1276_write_register(SX1276_MODEM_CONFIG_3, MODEM_CONFIG_VAL_3);
    SX1276_set_preamble_length(8);
    SX1276_set_frequency(frequency);
    SX1276_set_transmit_power(transmit_power, use_rfo);

    // Disable all interrupts, no interrupt masks
    SX1276_write_register(SX1276_DIO_MAPPING1, 0xFF);
    SX1276_write_register(SX1276_INTERRUPT_FLAGS_MASK, 0x00);
}


uint8_t SX1276_read_register(uint8_t address) {
    // Set read bit
    uint8_t read_address = address & 0x7F;

    uint8_t read_transfer[2] = {read_address, 0};
    uint8_t read_receive[2] = {0, 0};

    spi_select_slave(&sx_spi_master, &sx_spi_cs_instance, true);
    spi_transceive_buffer_wait(&sx_spi_master, read_transfer, read_receive, 2);
    spi_select_slave(&sx_spi_master, &sx_spi_cs_instance, false);

    return read_receive[1];
}

void SX1276_write_register(uint8_t address, uint8_t value) {
    // Set write bit
    uint8_t write_address = address | 0x80;

    uint8_t write_transfer[2] = {write_address, value};
    uint8_t write_receive[2] = {0, 0};

    spi_select_slave(&sx_spi_master, &sx_spi_cs_instance, true);
    spi_transceive_buffer_wait(&sx_spi_master, write_transfer, write_receive, 2);
    spi_select_slave(&sx_spi_master, &sx_spi_cs_instance, false);
}

/**
 * @brief Initiates sending a message.
 * @param pTxData: A pointer to the outgoing message string.
 * @param length: The number of bytes in the outgoing message. May not be
 *     greater than 256 (the size of the FIFO).
 */
void SX1276_send_message(uint8_t *pTxData, uint8_t length) {
    SX1276_set_device_mode(SX1276_STANDBY);

    // Reset FIFO queue to beginning
    SX1276_write_register(SX1276_FIFO_ADDR_PTR, 0x00);

    // Write message to FIFO
    uint8_t msg_i = 0;
    while (msg_i < length) {
        SX1276_write_register(SX1276_FIFO, pTxData[msg_i]);
        msg_i++;
    }

    // Set payload length
    SX1276_write_register(SX1276_PAYLOAD_LENGTH, length);

    // Start transmitting
    SX1276_set_device_mode(SX1276_TRANSMIT);
}

/**
 * @brief Receives a buffered message from the SX1276 device, then resets the FIFO pointer.
 * @param pRxData: A pointer to the receive data buffer. Should be 256 bytes long (the size
 *     of the SX1276 FIFO). The buffered message will be placed in pRxData.
 * @retval The length of the received message.
 */
uint8_t SX1276_receive_message(uint8_t *pRxData) {
    // Check that a message has been received
    uint8_t message_length = SX1276_read_register(SX1276_NUMBER_OF_BYTES_RECEIVED);
    if (message_length == 0) {
        return 0;
    }

    // Set read pointer to beginning of received packet
    SX1276_write_register(SX1276_FIFO_ADDR_PTR,
                          SX1276_read_register(SX1276_FIFO_RX_CURRENT_ADDR));

    // Read FIFO
    uint8_t read_value = SX1276_FIFO & 0x7F;
    spi_select_slave(&sx_spi_master, &sx_spi_cs_instance, true);
    spi_write_buffer_wait(&sx_spi_master, &read_value, 1);
    spi_read_buffer_wait(&sx_spi_master, pRxData, message_length, 0x00);
    spi_select_slave(&sx_spi_master, &sx_spi_cs_instance, false);

    // Reset read pointer to beginning of FIFO
    SX1276_write_register(SX1276_FIFO_ADDR_PTR, 0x00);

    return message_length;
}


void SX1276_set_device_mode(SX1276_device_mode_TypeDef mode) {
    // Get existing values, set Mode bits
    uint8_t old_value = SX1276_read_register(SX1276_OP_MODE);
    uint8_t write_value = (old_value & 0xF8) | (DEVICE_MODE_VALUES[mode] & 0x07);

    // Set device mode
    SX1276_write_register(SX1276_OP_MODE, write_value);
}

SX1276_device_mode_TypeDef SX1276_get_device_mode(void) {
    uint8_t device_mode_value = SX1276_read_register(SX1276_OP_MODE) & 0x07;

    SX1276_device_mode_TypeDef mode = 0;
    while ((mode < NUM_DEVICE_MODES) && (DEVICE_MODE_VALUES[mode] != device_mode_value)) {
        mode++;
    }

    return mode;

}

/**
 * Sets the device to either LoRa or FSK/OOK mode.
 *
 * is_lora: Whether to set to LoRa (true) or FSK/OOK (false/0).
 */
void SX1276_set_long_range_mode(uint8_t is_lora) {
    uint8_t lora_command = LORA_MODE;
    if (!is_lora) {
        lora_command = FSK_OOK_MODE;
    }
    uint8_t write_value = lora_command | DEVICE_MODE_VALUES[SX1276_SLEEP];
    SX1276_write_register(SX1276_OP_MODE, write_value);
}

/**
 * Determines whether the device is in LoRa or FSK/OOK mode.
 *
 * Returns true for LoRa mode and false/0 for FSK/OOK mode.
 */
uint8_t SX1276_get_long_range_mode(void) {
    uint8_t lora_mode_bit = SX1276_read_register(SX1276_OP_MODE) & LORA_MODE;
    return (lora_mode_bit == LORA_MODE);
}

/**
 * @brief Sets the length of the message preamble.
 * @param preamble_length: The 16-bit message preamble length.
 */
void SX1276_set_preamble_length(uint16_t preamble_length) {
    SX1276_write_register(SX1276_PREAMBLE_HIGH, (preamble_length >> 8) & 0xFF);
    SX1276_write_register(SX1276_PREAMBLE_LOW, preamble_length & 0xFF);
}

/**
 * @brief Sets the Rx/Tx carrier frequency. Note that LoRa operates over a
 * frequency range; this sets the center.
 * @param frequency: The target frequency, in Hz.
 */
void SX1276_set_frequency(uint32_t frequency) {
    uint32_t value = frequency / (OSCILLATOR_FREQUENCY / (1 << 19));
    SX1276_write_register(SX1276_FREQUENCY_HIGH, (value >> 16) & 0xFF);
    SX1276_write_register(SX1276_FREQUENCY_MID, (value >> 8) & 0xFF);
    SX1276_write_register(SX1276_FREQUENCY_LOW, value & 0xFF);
}

/**
 * @brief Gets the Rx/Tx carrier frequency.
 * @param frequency: The carrier frequency, in Hz.
 */
#if LORA_NOT_REQUIRED == 1
uint32_t SX1276_get_frequency(void) {
    uint8_t value_high = SX1276_read_register(SX1276_FREQUENCY_HIGH);
    uint8_t value_mid = SX1276_read_register(SX1276_FREQUENCY_MID);
    uint8_t value_low = SX1276_read_register(SX1276_FREQUENCY_LOW);

    uint32_t value = ((value_low & 0xFF) |
                      ((value_mid << 8) & 0xFF00) |
                      ((value_high << 16) & 0xFF0000));
    return value * (OSCILLATOR_FREQUENCY / (1 << 19));
}
#endif

/**
 * @brief Sets the transmit power and power amplifier output pin of the device.
 *     Implementation varies depending on whether PA_BOOST or RFO is in use.
 * @param transmit_power: The desired transmit power in dBm.
 * @param use_rfo: Whether to use the RFO (true/1) or PA_BOOST (false/0) power
 *     amplifier pin.
 */
void SX1276_set_transmit_power(uint8_t transmit_power, bool use_rfo) {
    /*(if (use_rfo) {
        if (transmit_power > 14) {
            PRINT("SX1276: transmit_power must be less than 15 when use_rfo is true.\r\n");
            while (1) {}
        }
        else if (transmit_power < -1) {
            PRINT("SX1276: transmit_power must be greater than -2 when use_rfo is true.\r\n");
            while (1) {}
        }*/
        SX1276_write_register(SX1276_POWER_AMPLIFIER_CONFIG, 0x70 | (transmit_power + 1));
    /*}
    else {
     */
        /*if (transmit_power > 23) {
            PRINT("SX1276: transmit_power must be less than 24 when use_rfo is false.\r\n");
            while (1) {}
        }
        else if (transmit_power < 5) {
            PRINT("SX1276: transmit_power must be greater than 4 when use_rfo is false.\r\n");
            while (1) {}
        }

        // Enable high power DAC if desired power is greater than 20 dBm
        if (transmit_power > 20) {
            SX1276_write_register(SX1276_HIGH_POWER_SETTINGS, 0x07);
        } else {
            SX1276_write_register(SX1276_HIGH_POWER_SETTINGS, 0x04);
        }

        SX1276_write_register(SX1276_POWER_AMPLIFIER_CONFIG, 0x80 | (transmit_power - 5));
    }*/
}

/**
 * @brief Sets the signal bandwidth. Higher bandwidth means higher bit rate, at
 *     the cost of link budget.
 * @param bandwidth: The bandwitdh to set.
 */
void SX1276_set_bandwidth(SX1276_bandwidth_TypeDef bandwidth) {
    uint8_t old_value = SX1276_read_register(SX1276_MODEM_CONFIG_1);
    SX1276_write_register(SX1276_MODEM_CONFIG_1,
                          (BANDWIDTH_VALUES[bandwidth] & 0xF0) | (old_value & (0x0F)));
}

/**
 * @brief Sets the coding rate. Higher coding rate means stronger error correction,
 *     at the cost of bit rate.
 * @param coding_rate: The coding rate to set.
 */
void SX1276_set_coding_rate(SX1276_coding_rate_TypeDef coding_rate) {
    uint8_t old_value = SX1276_read_register(SX1276_MODEM_CONFIG_1);
    SX1276_write_register(SX1276_MODEM_CONFIG_1,
                          (CODING_RATE_VALUES[coding_rate] & 0x0E) | (old_value & (0xF1)));
}

/**
 * @brief Sets the header mode.
 * @param is_implicit: Whether the headers should be implicit (1/true) or explicit
 *     (0/false).
 */
void SX1276_set_header_mode(uint8_t is_implicit) {
    uint8_t old_value = SX1276_read_register(SX1276_MODEM_CONFIG_1);
    uint8_t new_value;
    if (is_implicit) {
        new_value = 0x01 | (old_value & 0xFE);
    } else {
        new_value = old_value & 0xFE;
    }
    SX1276_write_register(SX1276_MODEM_CONFIG_1, new_value);
}

/**
 * @brief Sets the sprading factor. Higher spreading factor means higher link
 *     budget, at the cost of bit rate.
 * @param spreading_factor: The desired spreading factor. Must be an integer
 *     between 7 and 12, inclusive.
 */
void SX1276_set_spreading_factor(uint8_t spreading_factor) {
    uint8_t old_value = SX1276_read_register(SX1276_MODEM_CONFIG_2);
    SX1276_write_register(SX1276_MODEM_CONFIG_2,
                          ((spreading_factor << 4) & 0xF0) | (old_value & 0x0F));
}

/**
 * @brief Sets the Rx payload CRC mode. Determines whether the coding rate for
 *     received packets is extracted from the incoming packet header (1/true)
 *     or fixed to the configured coding rate (0/false).
 * @param is_extracted: The value to set.
 */
void SX1276_set_rx_payload_crc(uint8_t is_extracted) {
    uint8_t old_value = SX1276_read_register(SX1276_MODEM_CONFIG_2);
    uint8_t write_value;
    if (is_extracted) {
        write_value = 0x04 & (old_value & 0xFB);
    } else {
        write_value = old_value & 0xFB;
    }
    SX1276_write_register(SX1276_MODEM_CONFIG_2, write_value);
}

/**
 * @brief Gets the set of interrupt flags.
 * @retval The 8-bit set of interrupt flags.
 */
uint8_t SX1276_read_interrupt_flags(void) {
    return SX1276_read_register(SX1276_INTERRUPT_FLAGS);
}

/**
 * @brief Clears the receiveDone interrupt flag.
 */
void SX1276_clear_recieve_done_flag(void) {
    SX1276_write_register(SX1276_INTERRUPT_FLAGS, SX1276_RECEIVE_DONE_FLAG);
}

/**
 * @brief Clears the payloadCRCError interrupt flag.
 */
void SX1276_clear_payload_crc_error_flag(void) {
    SX1276_write_register(SX1276_INTERRUPT_FLAGS, SX1276_PAYLOAD_CRC_ERROR_FLAG);
}

/**
 * @brief Clears the transmitDone interrupt flag.
 */
void SX1276_clear_transmit_done_flag(void) {
    SX1276_write_register(SX1276_INTERRUPT_FLAGS, SX1276_TRANSMIT_DONE_FLAG);
}
