/**
 * Memory Spaces Definitions
 *
 * See for more details: http://ww1.microchip.com/downloads/en/DeviceDoc/SAM_L21_Family_DataSheet_DS60001477C.pdf#_OPENTOPIC_TOC_PROCESSING_d138e24594
 */
MEMORY
{
  /* Flash start address 0x00000000 with a total size of in hex of 0x00020000 (131.072kB) */
  bootrom      (rx)  : ORIGIN = 0x00000000, LENGTH = 0x00004000 /* See section 28.6.5 for bootloader sizes */
  approm       (rx)  : ORIGIN = 0x00004000, LENGTH = 0x0001C000

  /* SRAM start address 0x20000000 with a total size of in hex of 0x00004000 (16.384kB)  */
  sharedram (rwx): ORIGIN = 0x20000000, LENGTH = 0x100        /* Shared between bootloader and application aka not reset on rest_handler */
  ram      (rwx) : ORIGIN = 0x20000100, LENGTH = 0x3F00       /* Used by the running firmware [boot or app] aka reset on reset_handler */

  /* Low power SRAM start address 0x30000000 with a total size of in hex of 0x00002000 (8.192kB)  */
  lpram    (rwx) : ORIGIN = 0x30000000, LENGTH = 0x00002000
}


/* Define address for the firmware to use */
__bootrom_start__ = ORIGIN(bootrom);
__bootrom_size__ = LENGTH(bootrom);

__approm_start__ = ORIGIN(approm);
__approm_size__ = LENGTH(approm);

__sharedram_start__ = ORIGIN(sharedram);
__sharedram_size__ = LENGTH(sharedram);


/* Define the shared memory section */
SECTIONS
{
    .shared_memory (NOLOAD) : {
        KEEP(*(.shared_memory))
    } >sharedram
}
