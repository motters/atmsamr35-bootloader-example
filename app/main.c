#include <GlobalStartup.h>
#include <shared_memory.h>

/**
 * Application
 */
int main()
{
    configASF();

    // Welcome message
    printf("Application loaded V0.0.1 \r\n");

    // Inc boot count in shared memory
    printf("Bootcount is now: %d\r\n", shared_memory_get_boot_counter());

    // Load into firmware flasher
    shared_memory_set_dfu_requested(true);
    printf("Setting OTA mode to: %d\r\n", shared_memory_is_dfu_requested());

    // Try breaking
    printf("Trigger using a break point on me! \r\n");

    // Loop forever
    while(true);
}

