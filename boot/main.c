#include <stdio.h>

#include <nvm.h>

#include <GlobalStartup.h>
#include "memory_map_api.h"

// Proved via bootloader linker script
extern uint32_t _sfixed;
extern uint32_t _efixed;


void config_nvm(void)
{
    // Load the defaults setting the NVM
    struct nvm_config config_nvm;
    nvm_get_config_defaults(&config_nvm);

    // We want it to autowrite at the end of the page
    config_nvm.manual_page_write = false;

    // Set the nvm controller
    nvm_set_config(&config_nvm);
}


void write_page(uint32_t page, uint8_t *change_page_buffer)
{
    // Calculate the page address in flash
    uint32_t page_address = page * NVMCTRL_ROW_PAGES * NVMCTRL_PAGE_SIZE;

    // Record the error code
    enum status_code error_code;

    // Find root page in row
    uint32_t root_page = page - (page % NVMCTRL_ROW_PAGES);
    uint32_t root_address = (root_page * NVMCTRL_ROW_PAGES) * NVMCTRL_PAGE_SIZE;

    // Page buffer
    uint8_t page_buffers[4][NVMCTRL_PAGE_SIZE] = {{0}};

    // Loop through all pages in row and cache them in RAM
    for(int row = 0; row < NVMCTRL_ROW_PAGES; row++)
    {
        // Find address
        uint32_t address = root_address + (row * NVMCTRL_PAGE_SIZE);

        // Wait until we can read
        error_code = STATUS_BUSY;
        while (error_code == STATUS_BUSY)
        {
            // Read the page
            error_code = nvm_read_buffer(address, page_buffers[row], NVMCTRL_PAGE_SIZE);
        }
    }

    // Load the new page into the arround
    memcpy(&page_buffers[page % NVMCTRL_ROW_PAGES], change_page_buffer, NVMCTRL_PAGE_SIZE);

    // Before we can write to a page we need to erase the row (hence this includes the 4 pages in there)
    error_code = STATUS_BUSY;
    while (error_code == STATUS_BUSY)
    {
        error_code = nvm_erase_row(page_address);
    }

    // Loop through all pages in the row and write the cache
    for(int row = 0; row < NVMCTRL_ROW_PAGES; row++)
    {
        // Find address
        uint32_t address = root_address + (row * NVMCTRL_PAGE_SIZE);

        // Wait until we can read
        error_code = STATUS_BUSY;
        while (error_code == STATUS_BUSY)
        {
            // Write the page
            error_code = nvm_write_buffer(address, page_buffers[row], NVMCTRL_PAGE_SIZE);
        }
    }

}


void debug_read_app_flash(uint8_t eitherSide, uint8_t *page_buffer)
{
    // Show application firmware pages 2 either side of the start
    uint32_t app_page_location = (0x4000 / NVMCTRL_PAGE_SIZE) / NVMCTRL_ROW_PAGES;
    for(int page = app_page_location - eitherSide ; page < app_page_location + eitherSide + 1; page++)
    {
        // Page address
        uint32_t page_address = page * NVMCTRL_ROW_PAGES * NVMCTRL_PAGE_SIZE;

        // Create a page buffer
        uint8_t page_buffer_read[NVMCTRL_PAGE_SIZE] = {0};

        // Read the page
        enum status_code error_code = STATUS_BUSY;
        while (error_code == STATUS_BUSY)
        {
            error_code = nvm_read_buffer(page_address, page_buffer_read, NVMCTRL_PAGE_SIZE);
        }

        // Save packet if it's the page
        if(app_page_location == page)
            for(int i = 0; i < NVMCTRL_PAGE_SIZE; i++)
                page_buffer[i] = page_buffer_read[i];

        // Print read bytes
        printf("Page %d at %d: \r\n", page, page_address);
        for(int i = 0; i < NVMCTRL_PAGE_SIZE; i++)
        {
            printf(" %04x ", page_buffer_read[i]);
            if((i+1) % 16 == 0)
                printf("\r\n");
        }
        printf("\r\n");
    }
}


/**
 * Demo modifes the application fist byte, then puts it back to normal, then allows the boot
 *
 * Version   Flash Size [KB]    Number of Pages      Page Size [Bytes]
 *    J17         128                 2048                  64
 *   J18         256                 4096                  64
 *
 * The main address space is divided into rows and pages
 * There are 4 pages per row
 *
 * Get NVM memory page size and total pages
 * nvm_get_parameters(nvm_parameter);
 *
 * Calculate the page number & size
 * page_number = (row_number * 4) + page_pos_in_row
 * page_address = page_number * page_size
 *
 * Read memory
 * nvm_read_buffer(page_address, buffer, length);
 *
 * Erase row to write page; the smallest amount you can erase is a row;
 * As such we should really write 64 * 4 = 256 bytes at once. Other wise
 * the remaining page will just be buffered in RAM and then re-wrote.
 * nvm_erase_row(row_address)
 *
 * Write a number of bytes into a page
 * nvm_write_buffer(page_address, buffer, length);
 */
void write_firmware_demo(void)
{
    // Setup flash drivers
    config_nvm();

    printf("\r\nDemo'ing modified application firmware\r\n");
    printf("-------------------------------------------\r\n");

    // Show the start of the application firmware
    printf("Getting the first page for application firmware\r\n");
    uint8_t page_buffer[NVMCTRL_PAGE_SIZE] = {0};
    debug_read_app_flash(0, page_buffer);

    // Write a packet to the application firmware
    page_buffer[0] = 0x10;
    write_page(64, &page_buffer);

    // Show the updated packet
    printf("\r\nModified the first byte of the first page for application firmware to 0x10\r\n");
    debug_read_app_flash(0, page_buffer);

    // Fix the packet back to original so it'll boot
    page_buffer[0] = 0x38;
    write_page(64, &page_buffer);

    // Show the fixed packet
    printf("\r\nReturned the first byte of the first page for application firmware to normal\r\n");
    debug_read_app_flash(0, page_buffer);
}


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
    printf("\r\n\r\n\r\nWelcome to the bootloader V0.0.1\r\n");
#if 0
    printf("Bootloader says heey, opening app at %lu\r\n", &__approm_start__);
    printf("start boot %lu\r\n", &__bootrom_start__);
    printf("length boot %lu\r\n", &__bootrom_size__);
    printf("start app %lu\r\n", app_check_address);
    printf("length app %lu\r\n", &__approm_size__);
    printf("_sfixed %lu\r\n", &_sfixed);
    printf("_efixed %lu\r\n", &_efixed);
#endif

    // Demo modifying the application firmware
    // uses the same principle as OTA
    write_firmware_demo();

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
    printf("VTOR set to application at: %d\r\n", ((uint32_t) app_check_address_ptr & SCB_VTOR_TBLOFF_Msk));

    // Rebase the stack pointer
    __set_MSP(&app_check_address);
    __set_PSP(&app_check_address);
    printf("Main stack point set to: %d; Process stack pointer set to: %d\r\n", __get_MSP(), __get_PSP());

    // De-init serial
    printf("Serial will now be de-init & application loaded\r\n--------------------------------------------------\r\n\r\n");
    sio2host_deinit();

    // Jump to user Reset Handler of application
    asm("bx %0" ::"r"(*(unsigned*) ((uint32_t) app_check_address_ptr + 4)));

    // We'll never get here
    while(true)
    {
    };
}
