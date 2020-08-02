#include <stdio.h>
#include <string.h>

#include "shared_memory.h"
#include "memory_map_api.h"


// UnInitialised Marker number
const uint32_t INITIALISED_FLAG = 0xed9e6;


// Information we want to share between boot and app
// We pack this to ensure it's aligned across app & boot
typedef struct __attribute__((packed))
{
    uint32_t initialised_flag;
    uint32_t flags;
    uint8_t boot_counter;
    uint8_t update_id;
} shared_memory_t;


// Assigned the shared memory to the shared memory linker section
shared_memory_t shared_memory __attribute__((section(".shared_memory")));


// Flags that we are tracking int the flags int
enum
{
    DFU_REQUESTED = 1 << 0,
};


/**
 * Set flag in the share int
 *
 * @param flag Flag and hence bit position to set
 * @param value boolean logic to set 0 or 1
 */
static void shared_memory_set_flag(uint32_t flag, bool value)
{
    if (value)
    {
        shared_memory.flags |= flag;
    }
    else
    {
        shared_memory.flags &= ~flag;
    }
}


/**
 * Get flag in the share int
 *
 * @param flag Flag and hence bit to obtain
 * @return Bool logic for 0 & 1
 */
static bool shared_memory_get_flag(uint32_t flag)
{
    return shared_memory.flags & flag;
}


/**
 * When the power resets we need to re-initialise our share memory / struct.
 * We can do this by checking if a int value is set when the bootloader runs
 */
void shared_memory_init(void)
{
    // Does the flag equal what we'd expect if the memory is initialised
    if (shared_memory.initialised_flag != INITIALISED_FLAG)
    {
        // Tell the world what we're doing
        // printf("Shared memory uinitialized; Initialing and setting flag\r\n");

        // Re-initialise the struct
        memset(&shared_memory, 0, sizeof(shared_memory_t));

        // Set the expected flag to ensure we dont re-initialise again, untill next power cycle
        shared_memory.initialised_flag = INITIALISED_FLAG;
    }
}


/**
 * Check if the dfu flag is set
 *
 * @return the dfu flag
 */
bool shared_memory_is_dfu_requested(void)
{
    return shared_memory_get_flag(DFU_REQUESTED);
}


/**
 * Ensure the bootloader enters device firmware update mode on next reset
 *
 * @param state should we enter dfu mode on next reset?
 */
void shared_memory_set_dfu_requested(bool state)
{
    shared_memory_set_flag(DFU_REQUESTED, state);
}


/**
 * Increment the boot count
 *   - will be done by the boot loader each time it tries to load the app
 */
void shared_memory_increment_boot_counter(void)
{
    shared_memory.boot_counter++;
}


/**
 * Clear the boot count
 *   - will be done once the app is detected as stable
 */
void shared_memory_clear_boot_counter(void)
{
    shared_memory.boot_counter = 0;
}


/**
 * Get the application boot count
 *
 * @return int of each time the app booted
 */
uint8_t shared_memory_get_boot_counter(void)
{
    return shared_memory.boot_counter;
}



void shared_memory_set_update_id(uint8_t id)
{
    shared_memory.update_id = id;
}

uint8_t shared_memory_get_update_id()
{
    return shared_memory.update_id;
}
