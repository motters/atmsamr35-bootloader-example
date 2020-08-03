#include "update.h"

#include <rtc_api.h>
#include <shared_memory.h>
#include <flash.h>

// Indicates that last time the LoRa module was known to be in RECEIVE_CONTINUOUS mode.
static uint32_t last_receive_mode_time = 0;
static uint32_t serial_heart_beat = 0;
static uint32_t lora_heart_beat = 0;

static void reset_lora(void)
{
    // Reset LoRa device to clear flag issue
    SX1276_reset_device();
    SX1276_setup_device(
        LORA_DEFAULT_FREQUENCY,
        LORA_DEFAULT_TRANSMIT_POWER,
        LORA_DEFAULT_BANDWIDTH,
        LORA_DEFAULT_CODING_RATE,
        LORA_DEFAULT_SPREADING_FACTOR,
        LORA_DEFAULT_AMP_USES_RFO
    );
    SX1276_set_device_mode(SX1276_RECEIVE_CONTINUOUS);
}

static void handle_lora_message(uint8_t* message, uint8_t message_length)
{
    // too short to parse a header
    if (message_length < 2)
        return;

    // Check update id set by application
    if(message[0] != shared_memory_get_update_id())
        return;

    // Update slots 1 bit for each slot (2048/8=256)
    // bootloader: 16384 / 64 = 256 pages
    // application: 2048 - 256 = 1792
    // tracking size: 1792 / 8 = 224
    static uint8_t slot_status[224] = {0};

    // Get message command
    switch(message[1])
    {
        // Update slot
        case 1:
        {
            // Update flash
            update_page( message[2], &message[3]);
            uint8_t reply[] = {message[0], 1, 1};
            SX1276_send_message(reply, sizeof(reply));

            // Update tracking
            uint8_t key = message[1] / 8;
            uint8_t bit = message[1] % 8;
            slot_status[key] |= 1UL << bit;

            // Update heart beat to show update still in progress
            lora_heart_beat = rtc_api_count();

            break;
        }


        // Missing slots
        case 2:
        {
            // Return app update tracking
            uint8_t slot_status_reply[3 + sizeof(slot_status)] = { 0 };
            slot_status_reply[0] = message[0];
            slot_status_reply[1] = 2;
            slot_status_reply[2] = 1;
            for (int i = 0; i < sizeof(slot_status); ++i)
                slot_status_reply[i+2] = slot_status[i];
            SX1276_send_message(slot_status_reply, sizeof(slot_status_reply));

            // Update heart beat to show update still in progress
            lora_heart_beat = rtc_api_count();
            break;
        }

        // Erase app
        case 3:
        {
            // Erase application in flash
            erase_application();

            // Reset slot update tracker
            for (int i = 0; i < sizeof(slot_status); ++i)
                slot_status[i] = 0;

            // Show success
            uint8_t reply[] = {message[0], 3, 1};
            SX1276_send_message(reply, sizeof(reply));

            // Update heart beat to show update still in progress
            lora_heart_beat = rtc_api_count();
            break;
        }

        // Reboot
        case 4:
        {
            // Show success
            uint8_t reply[] = {message[0], 4, 1};
            SX1276_send_message(reply, sizeof(reply));
            delay_ms(1000);
            system_reset();
            break;
        }
    }
}

static void receive_lora_message(void)
{
    uint8_t lora_receive_buf[257];

    if (SX1276_read_interrupt_flags() & SX1276_PAYLOAD_CRC_ERROR_FLAG)
    {
        SX1276_clear_payload_crc_error_flag();
    }
    else
    {
        uint8_t lora_message_len = SX1276_receive_message(lora_receive_buf);
        handle_lora_message(lora_receive_buf, lora_message_len);
    }

    SX1276_clear_recieve_done_flag();
}

bool init_update(void)
{
    // Configure the LoRa antenna matching network selection. Should match the selected
    // LoRa amplifier.
    // pin low / false = PA_BOOST
    // pin high / true = RFO_HF
    struct port_config config_antenna_matching_port_pin;
    port_get_config_defaults(&config_antenna_matching_port_pin);
    config_antenna_matching_port_pin.direction = PORT_PIN_DIR_OUTPUT;
    port_pin_set_config(V6_ANTENNA_MATCHING_SELECTION_PIN, &config_antenna_matching_port_pin);
    port_pin_set_output_level(V6_ANTENNA_MATCHING_SELECTION_PIN, LORA_DEFAULT_AMP_USES_RFO);

    // Configure LoRa radio
    SX1276_configure_hardware();
    bool status = SX1276_setup_device(
        LORA_DEFAULT_FREQUENCY,
        LORA_DEFAULT_TRANSMIT_POWER,
        LORA_DEFAULT_BANDWIDTH,
        LORA_DEFAULT_CODING_RATE,
        LORA_DEFAULT_SPREADING_FACTOR,
        LORA_DEFAULT_AMP_USES_RFO
    );
    SX1276_set_device_mode(SX1276_RECEIVE_CONTINUOUS);

    return status;
}

bool listen_update(void)
{
    // Handle LoRa receiveDone and transmitDone events
    uint8_t lora_flags = SX1276_read_interrupt_flags();
    if (lora_flags & SX1276_RECEIVE_DONE_FLAG)
    {
        receive_lora_message();
    }

    if (lora_flags & SX1276_TRANSMIT_DONE_FLAG)
    {
        // Set device back to continuous receive
        SX1276_set_device_mode(SX1276_RECEIVE_CONTINUOUS);

        SX1276_clear_transmit_done_flag();
    }

    // Check for LoRa module stuck in non-receive mode
    if (SX1276_get_device_mode() == SX1276_RECEIVE_CONTINUOUS)
    {
        last_receive_mode_time = rtc_api_count();
    }
    else if((rtc_api_count() - last_receive_mode_time) > LORA_MAX_TRANSMIT_HANG_MS)
    {
        reset_lora();
    }

    // Handle all lora errors
    if ((lora_flags & SX1276_PAYLOAD_CRC_ERROR_FLAG) ||
        (SX1276_read_interrupt_flags() & SX1276_TRANSMIT_DONE_FLAG) ||
        (SX1276_read_interrupt_flags() & SX1276_RECEIVE_DONE_FLAG))
    {
        reset_lora();
    }

    // Print to serial to show we're still alive
    if((rtc_api_count() - serial_heart_beat) > 2000)
    {
        serial_heart_beat = rtc_api_count();
        PRINT(".")
    }

    // Reset if no lora coms for 20 seconds
    if((rtc_api_count() - lora_heart_beat) > 20000)
    {
        PRINT("\r\nLora silent rebooting...\r\n")
        return false;
    }

    return true;
}


