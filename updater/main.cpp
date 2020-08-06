#include <GlobalStartup.h>
#include <cstdio>

#include <lora/micro-driver-sx1276.h>
#include <print_array.h>
#include <rtc_api.h>

#include "app_firmware_auto_gen.h"
#include "app_firmware_1_auto_gen.h"

/**
 * Default values for LoRa radio parameters. These are used when performing a reset,
 * as well as when the value in storage is invalid.
 */
#define V6_ANTENNA_MATCHING_SELECTION_PIN  PIN_PA13
#define LORA_MAX_TRANSMIT_HANG_MS 1000
#define LORA_DEFAULT_AMP_USES_RFO      true    // true=RFO LF/HF   false=PA_BOOST

#if 0
// LONG range
#define LORA_DEFAULT_FREQUENCY         868500000
#define LORA_DEFAULT_TRANSMIT_POWER    14
#define LORA_DEFAULT_BANDWIDTH         SX1276_BW_125K
#define LORA_DEFAULT_CODING_RATE       SX1276_CR_4_5
#define LORA_DEFAULT_SPREADING_FACTOR  10
#else
// Short range
#define LORA_DEFAULT_FREQUENCY         869500000
#define LORA_DEFAULT_TRANSMIT_POWER    14
#define LORA_DEFAULT_BANDWIDTH         SX1276_BW_500K
#define LORA_DEFAULT_CODING_RATE       SX1276_CR_4_6
#define LORA_DEFAULT_SPREADING_FACTOR  7
#endif

/**
 * Serial buffer
 */
char serial_buffer[256] = { 0 };
int serial_position = 0;


bool init_lora(void)
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


void reset_lora(void)
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

void serial_clear(void)
{
    serial_position = 0;
    memset(serial_buffer, 0, sizeof(serial_buffer) +1);
}

bool serial_update(void)
{
    // Get characters
    int serialRxInt = sio2host_getchar_nowait();

    // Check length
    if(serialRxInt == -1)
        return false;

    // Convert the input to a character and echo it
    char serialRxChar = (char) serialRxInt;

    // Echo char
    print(&serialRxChar, 1);

    // Check position
    if(serial_position == 255)
    {
        PRINT("\r\nToo long please try again\r\n")
        serial_clear();
    }
    else
    {
        serial_buffer[serial_position] = serialRxChar;
        serial_position++;
    }

    return true;
}


/**
 * Updater
 */
int main()
{
    configASF();
    rtc_api_init();

    // http://patorjk.com/software/taag/#p=display&h=0&v=0&f=Ivrit&t=Edge%20Updater (Ivrit)  Larry 3D
    printf("  _____       _                    _   _               _           _                 \r\n"
          " | ____|   __| |   __ _    ___    | | | |  _ __     __| |   __ _  | |_    ___   _ __ \r\n"
          " |  _|    / _` |  / _` |  / _ \\   | | | | | '_ \\   / _` |  / _` | | __|  / _ \\ | '__|\r\n"
          " | |___  | (_| | | (_| | |  __/   | |_| | | |_) | | (_| | | (_| | | |_  |  __/ | |   \r\n"
          " |_____|  \\__,_|  \\__, |  \\___|    \\___/  | .__/   \\__,_|  \\__,_|  \\__|  \\___| |_|   \r\n"
          "                  |___/                   |_|                                        \r\n");

    // Welcome message
    PRINT("Version 0.0.1\r\n\r\n$ ");

    // Setup lora
    init_lora();

    // Loop forever
    bool loop = true;
    while(loop)
    {
        // Get serial content
        serial_update();

        // Has enter
        if(strstr(serial_buffer, "\r\n") != NULL)
        {
            // Get Edge Id
            uint32_t edgeId;
            char cmd[20];

            // Get function type
            if(strstr(serial_buffer, "REGISTER") != NULL)
            {
                // Abstract params (123)
                int id;

                // Find the fields in the array
                uint8_t fields_converted = sscanf(serial_buffer, "%[A-Z2] %*c %lu %*c %d", cmd, &edgeId, &id);

                // If we got all the params
                if(fields_converted == 3)
                {
                    // Send the lora command

                    // Wait for a reply

                    // Print the reply
                    printf("Registering update id %d to edge %lu\r\n", id, edgeId);
                }
                else
                {
                    printf("Incorrect parameters\r\n");
                }
            }
            else if(strstr(serial_buffer, "UPDATE") != NULL)
            {
                // Abstract params
                int id;

                // Find the fields in the array
                uint8_t fields_converted = sscanf(serial_buffer, "%[A-Z2] %*c %d", cmd, &id);

                uint32_t *actual_firmware_upgrade_size;
                const uint8_t *actual_firmware_upgrade;
                if(id == 1)
                {
                    printf("id: %d\n", id);
                    actual_firmware_upgrade_size = &firmware_upgrade_1_size;
                    actual_firmware_upgrade = firmware_upgrade_1;
                }
                else
                {
                    printf("id: %d\n", id);
                    actual_firmware_upgrade_size = &firmware_upgrade_size;
                    actual_firmware_upgrade = firmware_upgrade;
                }

                // If we got all the params
                if(fields_converted == 2)
                {
                    PRINT("Updating: ")
                    // 64 byte packets @todo packet needs to be uint32_t
                    for(uint8_t packet = 0; packet <= (*actual_firmware_upgrade_size / 64); packet++)
                    {
                        // Start byte
                        uint32_t start = packet * 64;

                        // Generate lora packet(update id, command id, page id, page data[64 bytes]) (uint8_t) id
                        uint8_t send[67] = {123, 1, packet};
                        for(size_t i = 0; i < 64; i++)
                        {
                            if(start+i < *actual_firmware_upgrade_size)
                            {
                                send[i + 3] = actual_firmware_upgrade[start + i];
                            }
                            else
                            {
                                send[i + 3] = 0xFF;
                            }
                        }

                        // Print lora packet
                        /*printf("Packet id: %lu: [ ", packet);
                        for(size_t a = 0; a < 64; a++)
                            printf("%d, ", send[a]);
                        printf("]\r\n ");*/
                        // Send the lora command and wait until sent
                        SX1276_send_message(send, 67);
                        while(!(SX1276_read_interrupt_flags() & (uint8_t) SX1276_TRANSMIT_DONE_FLAG))
                        {
                        }
                        SX1276_set_device_mode(SX1276_RECEIVE_CONTINUOUS);
                        while(SX1276_get_device_mode() != SX1276_RECEIVE_CONTINUOUS)
                        {
                        }
                        SX1276_clear_transmit_done_flag();
                        while((SX1276_read_interrupt_flags() & (uint8_t) SX1276_TRANSMIT_DONE_FLAG))
                        {
                        }

                        // Wait for reply
                        if(SX1276_get_device_mode() == SX1276_RECEIVE_CONTINUOUS)
                        {
                            uint32_t listen_started = rtc_api_count();
                            bool reply_status = false;

                            while (!reply_status)
                            {
                                // Check for reply
                                if(SX1276_read_interrupt_flags() & (uint8_t) SX1276_RECEIVE_DONE_FLAG)
                                {
                                    // Message
                                    //printf("Success packet: %d\n", packet);
                                    PRINT(".")

                                    // Clear flag dont check content for now mvp life
                                    SX1276_clear_recieve_done_flag();
                                    while(SX1276_read_interrupt_flags() & (uint8_t) SX1276_RECEIVE_DONE_FLAG)
                                    {
                                    }
                                    reply_status = true;
                                    delay_ms(50);
                                }
                                // Send again if we never get a reply
                                if((rtc_api_count() - listen_started) > 2000)
                                {
                                    // Send again
                                    printf("Failed packet: %d will re-try\n", packet);
                                    packet--;
                                    reply_status = true;
                                }
                            }
                        }
                    }

                    // Print the reply
                    printf("\r\nUpdated with firmware %d\r\n", id);
                }
                else
                {
                    printf("Incorrect parameters\r\n");
                }

            }
            else if(strstr(serial_buffer, "REBOOT") != NULL)
            {
                // Abstract params
                int id;

                // Find the fields in the array
                uint8_t fields_converted = sscanf(serial_buffer, "%[A-Z2] %*c %d", cmd, &id);

                // If we got all the params
                if(fields_converted == 2)
                {
                    // Send the lora command

                    // Wait for a reply

                    // Print the reply
                    printf("Rebooting edges registered to update %d\r\n", id);
                }
                else
                {
                    printf("Incorrect parameters\r\n");
                }
            }
            else if(strstr(serial_buffer, "VERIFY") != NULL)
            {
                // Abstract params
                int id;

                // Find the fields in the array
                uint8_t fields_converted = sscanf(serial_buffer, "%[A-Z] %*c %d", cmd, &id);

                // If we got all the params
                if(fields_converted == 2)
                {
                    // Send the lora command

                    // Wait for a reply

                    // Print the reply
                    printf("Verifying packet that have been wrote to edge registered at %d\r\n", id);
                }
                else
                {
                    printf("Incorrect parameters\r\n");
                }
            }
            else
            {
                PRINT("Unknown command\r\n");
            }

            // New command line
            PRINT("\r\n$ ");

            // Clear array
            serial_clear();
        }
    }
}
