#include <GlobalStartup.h>
#include <cstdio>


#include <lora/micro-driver-sx1276.h>
#include <print_array.h>

/**
 * Default values for LoRa radio parameters. These are used when performing a reset,
 * as well as when the value in storage is invalid.
 */

#define V6_ANTENNA_MATCHING_SELECTION_PIN  PIN_PA13
#define LORA_MAX_TRANSMIT_HANG_MS 1000
#define LORA_DEFAULT_FREQUENCY         868500000
#define LORA_DEFAULT_TRANSMIT_POWER    14
#define LORA_DEFAULT_BANDWIDTH         SX1276_BW_125K
#define LORA_DEFAULT_CODING_RATE       SX1276_CR_4_5
#define LORA_DEFAULT_SPREADING_FACTOR  10
#define LORA_DEFAULT_AMP_USES_RFO      true    // true=RFO LF/HF   false=PA_BOOST


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

    // Welcome message
    PRINT("Edge Updater\r\n");

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
                // Abstract params
                uint8_t id;

                // Find the fields in the array
                uint8_t fields_converted = sscanf(serial_buffer, "%[A-Z2] %*c %d %*c %d", cmd, &edgeId, &id);

                // If we got all the params
                if(fields_converted == 3)
                {
                    // Send the lora command

                    // Wait for a reply

                    // Print the reply
                    printf("Registering update id %d to edge %lu\r\n", id, edgeId);
                }

            }
            else if(strstr(serial_buffer, "UPDATE") != NULL)
            {
                // Abstract params
                uint8_t id;

                // Find the fields in the array
                uint8_t fields_converted = sscanf(serial_buffer, "%[A-Z2] %*c %d", cmd, &id);

                // If we got all the params
                if(fields_converted == 2)
                {
                    // Send the lora command

                    // Wait for a reply

                    // Print the reply
                    printf("Updating edge %lu with update %d\r\n", edgeId, id);
                }

            }
            else if(strstr(serial_buffer, "REBOOT") != NULL)
            {
                // Abstract params
                uint8_t id;

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
            }
            else if(strstr(serial_buffer, "VERIFY") != NULL)
            {
                // Abstract params
                uint8_t id;

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
            }
            else
            {
                PRINT("Unknown command\r\n");
            }

            // Clear array
            serial_clear();
        }
    }
}
