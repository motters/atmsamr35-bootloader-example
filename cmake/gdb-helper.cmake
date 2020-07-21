##
## Collection of functions to generate different GDB debugging configurations
##

# Get the path of this module
set(CURRENT_MODULE_DIR ..)

#---------------------------------------------------------------------------------------
# Set tools
#---------------------------------------------------------------------------------------
set(GDB_BIN ${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN}-gdb${TOOLCHAIN_EXT})
if(NOT OPENOCD_BIN)
        if(CMAKE_HOST_SYSTEM_NAME STREQUAL Linux)
            set(OPENOCD_BIN "/usr/bin/openocd" CACHE STRING "OpenOCD executable")
        elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL Darwin)
            set(OPENOCD_BIN "/usr/local/bin/openocd" CACHE STRING "OpenOCD executable")
        elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL Windows)
            set(OPENOCD_BIN "C:/openocd/bin/openocd.exe" CACHE STRING "OpenOCD executable")
        endif()
endif()

#---------------------------------------------------------------------------------------
# Generates a GDB run script for debugging with any supported programmer and openOCD.
#---------------------------------------------------------------------------------------
function(generate_run_gdb_openocd OCD_BIN OCD_OFFSET)
    # Hex to decimal for bootloader lock
    math(EXPR ODB_BOOT_LOCK "${BOOTLOADER_OFFSET}" OUTPUT_FORMAT DECIMAL)

    # Output config file
    configure_file(cmake/openocd-script.cfg ${PROJECT_BINARY_DIR}/openocd-script-${OCD_BIN}.cfg @ONLY)
endfunction()
