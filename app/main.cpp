#include <GlobalStartup.h>

#include "samr35j17b.h"

#include <shared_memory.h>
#include <image.h>

// Exported by linker
extern DeviceVectors exception_table;

// Fill in the header information
image_hdr_t image_hdr __attribute__((section(".image_hdr"))) = {
    .image_magic = IMAGE_MAGIC,
    .version_major = 1,
    .version_minor = 0,
    .version_patch = 2,
    .version_rc = 10,
    .vector_addr = (uint32_t)&exception_table,
    .git_sha = GIT_SHA,
};

/**
 * Application
 */
int main()
{
    configASF();

    // Welcome message
    printf("Application -  v%d.%d.%d-rc.%d (%s) - CRC 0x%lx\n",
           image_hdr.version_major,
           image_hdr.version_minor,
           image_hdr.version_patch,
           image_hdr.version_rc,
           image_hdr.git_sha,
           image_hdr.crc);

    // Inc boot count in shared memory
    printf("Resetting bootcount of %d as app is stable\r\n", shared_memory_get_boot_counter());
    shared_memory_clear_boot_counter();

    // Load into firmware flasher
    shared_memory_set_dfu_requested(true);
    shared_memory_set_update_id(123);
    printf("Setting OTA mode: %d\r\n", shared_memory_is_dfu_requested());

    // Reset to enter updater
    printf("Resetting to entering bootloader's updater\r\n");
    system_reset();

    // Loop forever
    while(true);
}

