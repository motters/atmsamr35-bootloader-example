/**
 * Linker script for the samr35j17b
 *
 * The SAMR35 series contains a SAML21 micrcontroller, see section 10
 * http://ww1.microchip.com/downloads/en/DeviceDoc/SAM-R34-R35-Low-Power-LoRa-Sub-GHz-SiP-Data-Sheet-DS70005356C.pdf
 */


/**
 * Define the arch
 */
OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
SEARCH_DIR(.)


/**
 * Memory Spaces Definitions
 *
 * See for more details: http://ww1.microchip.com/downloads/en/DeviceDoc/SAM_L21_Family_DataSheet_DS60001477C.pdf#_OPENTOPIC_TOC_PROCESSING_d138e24594
 */
MEMORY
{
  /* Flash start address 0x00000000 with a total size of in hex of 0x00020000 (131.072kB) */
  rom      (rx)  : ORIGIN = 0x00000000, LENGTH = 0x00020000

  /* SRAM start address 0x20000000 with a total size of in hex of 0x00004000 (16.384kB)  */
  ram      (rwx) : ORIGIN = 0x20000000, LENGTH = 0x00004000

  /* Low power SRAM start address 0x30000000 with a total size of in hex of 0x00002000 (8.192kB)  */
  lpram    (rwx) : ORIGIN = 0x30000000, LENGTH = 0x00002000
}


/**
 * How large should the stack size be?
 * This depend how much memory is statically allocated by the firmware
 */
STACK_SIZE = DEFINED(STACK_SIZE) ? STACK_SIZE : DEFINED(__stack_size__) ? __stack_size__ : 0x1000;


/**
 * Section Definitions
 */
SECTIONS
{

    /**
     * ROM Definition: part 1
     */
    .text :
    {
        . = ALIGN(4);
        _sfixed = .;
        KEEP(*(.vectors .vectors.*))
        *(.text .text.*)
        *(.rodata .rodata*)
        . = ALIGN(4);


        /**
         * Support C constructors, and C destructors in both user code
         * and the C library. This also provides support for C++ code.
         */
        . = ALIGN(4);
        KEEP(*(.init))
        . = ALIGN(4);
        __preinit_array_start = .;
        KEEP (*(.preinit_array))
        __preinit_array_end = .;

        . = ALIGN(4);
        __init_array_start = .;
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array))
        __init_array_end = .;

        . = ALIGN(4);
        KEEP (*crtbegin.o(.ctors))
        KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*crtend.o(.ctors))

        . = ALIGN(4);
        KEEP(*(.fini))

        . = ALIGN(4);
        __fini_array_start = .;
        KEEP (*(.fini_array))
        KEEP (*(SORT(.fini_array.*)))
        __fini_array_end = .;

        KEEP (*crtbegin.o(.dtors))
        KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
        KEEP (*(SORT(.dtors.*)))
        KEEP (*crtend.o(.dtors))

        . = ALIGN(4);
        _efixed = .;            /* End of text section */
    } > rom


    /**
     * ROM Definition: part 2
     * .ARM.exidx is sorted, so has to go in its own output section.
     */
    PROVIDE_HIDDEN (__exidx_start = .);
    .ARM.exidx :
    {
      *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > rom
    PROVIDE_HIDDEN (__exidx_end = .);

    . = ALIGN(4);
    _etext = .;


    /**
     * RAM Definition: part 1
     * .ARM.exidx is sorted, so has to go in its own output section.
     */
    .relocate : AT (_etext)
    {
        . = ALIGN(4);
        _srelocate = .;
        *(.ramfunc .ramfunc.*);
        *(.data .data.*);
        . = ALIGN(4);
        _erelocate = .;
    } > ram


    /**
     * Low Power RAM Definition
     */
    .lpram (NOLOAD):
    {
        . = ALIGN(8);
        _slpram = .;
        *(.lpram .lpram.*);
        . = ALIGN(8);
        _elpram = .;
    } > lpram


    /**
     * RAM Definition: part 2
     * .bss section which is used for uninitialized data
     */
    .bss (NOLOAD) :
    {
        . = ALIGN(4);
        _sbss = . ;
        _szero = .;
        *(.bss .bss.*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = . ;
        _ezero = .;
    } > ram


    /**
     * RAM Definition: part 3
     * Stack section in RAM
     */
    .stack (NOLOAD):
    {
        . = ALIGN(8);
        _sstack = .;
        . = . + STACK_SIZE;
        . = ALIGN(8);
        _estack = .;
    } > ram

    . = ALIGN(4);
    _end = . ;
}
