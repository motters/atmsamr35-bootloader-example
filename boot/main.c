#include <stdio.h>

#include <GlobalStartup.h>

#include <flash.h>
#include <image.h>
#include <shared_memory.h>

#include "load_app.h"

// Proved via bootloader linker script
extern uint32_t _sfixed;
extern uint32_t _efixed;


/**
 * Bootloader
 */
int main()
{
    // Enable serial
    sio2host_init();

    // Print bootloader variables
    printf("\r\n\r\n\r\nWelcome to the bootloader V0.0.1\r\n");

    // Setup shared memory
    shared_memory_init();

#if 0
    printf("Bootloader says heey, opening app at %lu\r\n", &__approm_start__);
    printf("start boot %lu\r\n", &__bootrom_start__);
    printf("length boot %lu\r\n", &__bootrom_size__);
    printf("start app %lu\r\n", app_check_address);
    printf("length app %lu\r\n", &__approm_size__);
    printf("_sfixed %lu\r\n", &_sfixed);
    printf("_efixed %lu\r\n", &_efixed);
#endif

    // Should we start app or enter DFU
    if (!shared_memory_is_dfu_requested())
    {
        // Get header information
        const image_hdr_t *hdr = image_get_header(IMAGE_SLOT_1);

        // Validate app in flash
        if(!app_verify(IMAGE_SLOT_1, hdr))
        {
            printf("Application firmware failed verification");
            ENDLESS_LOOP
        }

        // Switch to application
        app_start(hdr);
    }

    // Update flash
    // Demo modifying the application firmware
    // uses the same principle as OTA
    write_firmware_demo();

    // Clear DFT
    shared_memory_set_dfu_requested(false);

    // We'll never get here
    ENDLESS_LOOP
}
