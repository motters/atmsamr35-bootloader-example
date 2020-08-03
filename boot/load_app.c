#include "load_app.h"

#include <print_array.h>

#include "rtc_api.h"

static uint32_t* app_check_address_ptr = NULL;

/**
 * Attempt to boot the application
 *
 * @param hdr
 */
bool app_start(const image_hdr_t *hdr)
{
    // Get app address
    uint32_t app_check_address = hdr->vector_addr;
    app_check_address_ptr = (uint32_t*) app_check_address;

    // Check if there is an application present at the app address
    if((uint32_t) app_check_address_ptr == 0xFFFFFFFF)
    {
        return false;
        //PRINT("No application found.\r\n");
        // Start an endless loop for now
        //ENDLESS_LOOP
    }

    // Check that the vector table is aligned
    if(((app_check_address - sizeof(image_hdr_t)) & ~SCB_VTOR_TBLOFF_Msk) != 0x00)
    {
        return false;
        //printf("!!!!!! Test vector table address failed !!!!!! %lu %lu %lu\r\n", app_check_address,
        //       ~SCB_VTOR_TBLOFF_Msk, (app_check_address & ~SCB_VTOR_TBLOFF_Msk));
        //PRINT("Test vector table address failed")
        // Start an endless loop for now
        //ENDLESS_LOOP
    }

    // Rebase the vector table base address
    SCB->VTOR = ((uint32_t) app_check_address_ptr & SCB_VTOR_TBLOFF_Msk);

    // Rebase the stack pointer
    __set_MSP((uint32_t) &app_check_address);
    __set_PSP((uint32_t) &app_check_address);

    // Inc boot count
    shared_memory_increment_boot_counter();

    // De-init serial
    sio2host_deinit();
    rtc_api_deinit();

    // Jump to user Reset Handler of application
    __asm("bx %0" ::"r"(*(unsigned*) ((uint32_t) app_check_address_ptr + 4)));
}

