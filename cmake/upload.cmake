
set(GENERATE_GDBINIT TRUE)

set(OpenOCD "C:\\msys64\\mingw32\\bin\\openocd.bin")
set(GDB_PORT 23331)
set(OPENOCD_CHIP_CONFIG_COMMANDS
        -f ${OpenOCD_SCRIPT_DIR}/interface/cmsis-dap.cfg
        -f ${OpenOCD_SCRIPT_DIR}/target/at91samdXX.cfg
        -c "gdb_memory_map disable")

function(gen_upload_target TARGET_NAME EXECUTABLE BIN_FILE)

    add_custom_target(flash-${TARGET_NAME}
            COMMENT "Flashing ${TARGET_NAME} with OpenOCD..."
            COMMAND ${OpenOCD}
            ${OPENOCD_CHIP_CONFIG_COMMANDS}
            -c init
            -c "reset init"
            -c "program ${BIN_FILE} reset exit")


    add_dependencies(flash-${TARGET_NAME} ${TARGET_NAME})

    # create debug target
    add_custom_target(debug-${TARGET_NAME}
            COMMENT "starting GDB to debug ${TARGET_NAME}..."
            COMMAND arm-none-eabi-gdb
            --command=${GDBINIT_PATH}
            ${EXECUTABLE})


    add_dependencies(debug-${TARGET_NAME} ${TARGET_NAME})

endfunction(gen_upload_target)

# also create a target to run GDB server
add_custom_target(start-gdbserver
        COMMENT "Starting OpenOCD GDB server"
        COMMAND
        ${OpenOCD}
        ${OPENOCD_CHIP_CONFIG_COMMANDS}
        -c "gdb_port ${GDB_PORT}"
        -c init
        -c "reset init"
        )