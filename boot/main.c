#include <stdio.h>
#include <string.h>
#include <asf.h>

#include <flash.h>
#include <image.h>
#include <shared_memory.h>
#include <print_array.h>

#include "load_app.h"
#include "verify.h"
#include "update.h"
#include "rtc_api.h"

// Proved via bootloader linker script
extern uint32_t _sfixed;
extern uint32_t _efixed;

#define DISABLE_APP 0
#define MAX_BOOT_ATTEMPTS 3

/**
 * Bootloader
 */
int main()
{
    // Enable serial
    //sio2host_init();
    //rtc_api_init();

    system_init();
    sio2host_init();
    delay_init();
    rtc_api_init();

    // Print bootloader variables
    PRINT("\r\n\r\n\r\nBootloader V0.0.1\r\n");

    // Setup shared memory
    shared_memory_init();

    // Should we start app or enter DFU
    if (!shared_memory_is_dfu_requested() && shared_memory_get_boot_counter() <= MAX_BOOT_ATTEMPTS)
    {
        // Get header information
        const image_hdr_t *hdr = image_get_header(IMAGE_SLOT_1);

        // Print waiting message
        PRINT("Verifying app...\r\n");

        // Validate app in flash
        bool crc = crc_verification(IMAGE_SLOT_1, hdr);
        bool ecdsa = security_verification(IMAGE_SLOT_1, hdr);

        // Message we passed all checks
        if(crc && ecdsa)
        {
            PRINT("CRC & ECDSA passed booting app...\r\n\r\n");

            // Switch to application
            if(DISABLE_APP == 0 && !app_start(hdr))
                PRINT("App failed to start\r\n")
        }

        // App should have booted by the point, we're only here due to a failure
        // Try to boot again but inc the boot counter
        shared_memory_increment_boot_counter();
        system_reset();
    }

    // Clear DFT flag
    shared_memory_set_dfu_requested(false);

    // Init the updater
    init_update();

    // Listen for lora
    bool contiue_listening = true;
    PRINT("Updater Loaded: ")
    while (contiue_listening)
        contiue_listening = listen_update();

    // We'll never get here
    system_reset();
}
