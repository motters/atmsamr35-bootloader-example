#include <stdio.h>

#include <GlobalStartup.h>
#include "memory_map_api.h"

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

    // Get app address
    uint32_t app_check_address = &__approm_start__;
    uint32_t* app_check_address_ptr = (uint32_t*) app_check_address;

    // Print bootloader variables
    printf("\r\n\r\n\r\nWelcome to the bootloader!\r\n");
#if 0
    printf("Bootloader says heey, opening app at %lu\r\n", &__approm_start__);
    printf("start boot %lu\r\n", &__bootrom_start__);
    printf("length boot %lu\r\n", &__bootrom_size__);
    printf("start app %lu\r\n", app_check_address);
    printf("length app %lu\r\n", &__approm_size__);
    printf("_sfixed %lu\r\n", &_sfixed);
    printf("_efixed %lu\r\n", &_efixed);
#endif

    // Check if there is an application present at the app address
    if(app_check_address_ptr == 0xFFFFFFFF)
    {
        printf("!!!!!! No application found !!!!!!\r\n");
        while(true)
        {
        };
    }
    else
    {
        printf("Found application at: %lu; Main stack point at: %lu\r\n", app_check_address_ptr, __get_MSP());
    }

    // Check that the vector table is aligned
    if((app_check_address & ~SCB_VTOR_TBLOFF_Msk) != 0x00)
    {
        printf("!!!!!! Test vector table address failed !!!!!! %d %d %d\r\n", app_check_address, ~SCB_VTOR_TBLOFF_Msk,
               (app_check_address & ~SCB_VTOR_TBLOFF_Msk));
        while(true)
        {
        };
    }
    else
    {
        printf("Test vector table address passed: %d %d %d\r\n", app_check_address, SCB_VTOR_TBLOFF_Msk,
               (app_check_address & ~SCB_VTOR_TBLOFF_Msk));
    }

    // Rebase the vector table base address
    SCB->VTOR = ((uint32_t) app_check_address_ptr & SCB_VTOR_TBLOFF_Msk);
    printf("VTOR set to application: %d\r\n", ((uint32_t) app_check_address_ptr & SCB_VTOR_TBLOFF_Msk));

    // Rebase the stack pointer
    __set_MSP(&app_check_address);
    __set_PSP(&app_check_address);
    printf("Main stack point set to: %d; Process stack pointer set to: %d\r\n", __get_MSP(), __get_PSP());

    // De-init serial
    printf("Serial will now be de-init & application loaded\r\n--------------------------------------------------\r\n");
    sio2host_deinit();

    // Jump to user Reset Handler of application
    asm("bx %0" ::"r"(*(unsigned*) ((uint32_t) app_check_address_ptr + 4)));

    // We'll never get here
    while(true)
    {
    };
}
