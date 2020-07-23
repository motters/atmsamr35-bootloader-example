#include "load_app.h"

static uint32_t* app_check_address_ptr = NULL;

bool app_verify()
{

    return true;
}

void app_start()
{
    // Get app address
    uint32_t app_check_address = (uint32_t) &__approm_start__;
    app_check_address_ptr = (uint32_t*) app_check_address;

    // Check if there is an application present at the app address
    if((uint32_t) app_check_address_ptr == 0xFFFFFFFF)
    {
        printf("!!!!!! No application found !!!!!!\r\n");
        // Start an endless loop for now
        ENDLESS_LOOP
    }
    else
    {
        printf("Found application at: %lu; Main stack point at: %lu\r\n", (uint32_t) app_check_address_ptr, __get_MSP());
    }

    // Check that the vector table is aligned
    if((app_check_address & ~SCB_VTOR_TBLOFF_Msk) != 0x00)
    {
        printf("!!!!!! Test vector table address failed !!!!!! %lu %lu %lu\r\n", app_check_address,
               ~SCB_VTOR_TBLOFF_Msk, (app_check_address & ~SCB_VTOR_TBLOFF_Msk));

        // Start an endless loop for now
        ENDLESS_LOOP
    }
    else
    {
        printf("Test vector table address passed: %lu %lu %lu\r\n", app_check_address, SCB_VTOR_TBLOFF_Msk,
               (app_check_address & ~SCB_VTOR_TBLOFF_Msk));
    }

    // Rebase the vector table base address
    SCB->VTOR = ((uint32_t) app_check_address_ptr & SCB_VTOR_TBLOFF_Msk);
    printf("VTOR set to application at: %lu\r\n", ((uint32_t) app_check_address_ptr & SCB_VTOR_TBLOFF_Msk));

    // Rebase the stack pointer
    __set_MSP((uint32_t) &app_check_address);
    __set_PSP((uint32_t) &app_check_address);
    printf("Main stack point set to: %lu; Process stack pointer set to: %lu\r\n", __get_MSP(), __get_PSP());


    // Inc boot count
    shared_memory_increment_boot_counter();

    // De-init serial
    printf("Serial will now be de-init & application loaded\r\n"
           "--------------------------------------------------\r\n\r\n");
    sio2host_deinit();

    // Jump to user Reset Handler of application
    __asm("bx %0" ::"r"(*(unsigned*) ((uint32_t) app_check_address_ptr + 4)));
}

