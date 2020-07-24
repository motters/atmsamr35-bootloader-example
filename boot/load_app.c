#include "load_app.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

static uint32_t* app_check_address_ptr = NULL;

/* Simple public domain implementation of the standard CRC32 checksum.
 * Outputs the checksum for each file given as a command line argument.
 * Invalid file names and files that cause errors are silently skipped.
 * The program reads from stdin if it is called with no arguments.
 *
 * From http://home.thep.lu.se/~bjorn/crc/ */

static uint32_t crc32_for_byte(uint32_t r)
{
    for(int j = 0; j < 8; ++j)
        r = (r & 1? 0: (uint32_t)0xEDB88320L) ^ r >> 1;
    return r ^ (uint32_t)0xFF000000L;
}

static uint32_t crc32(const void *data, uint32_t n_bytes)
{
    uint32_t crc = 0;
    static uint32_t table[0x100];
    if(!*table)
        for(size_t i = 0; i < 0x100; ++i)
            table[i] = crc32_for_byte(i);
    for(size_t i = 0; i < n_bytes; ++i)
        crc = table[(uint8_t)crc ^ ((uint8_t*)data)[i]] ^ crc >> 8;

    return crc;
}

/**
 * Check the slot / app flash CRC matches what is expected
 *
 * @param slot
 * @param hdr
 * @return
 */
bool app_verify(image_slot_t slot,  const image_hdr_t *hdr)
{
    // Find address
    void *addr = NULL;
    switch (slot)
    {
        case IMAGE_SLOT_1:
            addr = &__approm_start__;
            break;
        default:
            return false;
    }

    // Find the size of the header
    addr += sizeof(image_hdr_t);

    // Get the size of the firmware bin
    uint32_t len = hdr->data_size;

    // Calculate the CRC of the app firmware
    uint32_t a = crc32(addr, len);

    // Obtains the CRC from flash
    uint32_t b = hdr->crc;

    // Does the calc crc match the one in flash
    if (a == b)
        return true;

    // CRC Failed
    return false;

}

void app_start(const image_hdr_t *hdr)
{
    // Get app address
    uint32_t app_check_address = hdr->vector_addr;
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
    if(((app_check_address - sizeof(image_hdr_t)) & ~SCB_VTOR_TBLOFF_Msk) != 0x00)
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

