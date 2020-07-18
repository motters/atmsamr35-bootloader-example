#
# Edge 6 firmware projects
#

# We'll request a rather new version of cmake becuase we can
cmake_minimum_required(VERSION 3.12)

# Project name
project(SAMR35)

set(TARGET "atsamr35j17b")

# What version of c++ should be used?
set(CMAKE_CXX_STANDARD 17)

# Add the cmake folder so the FindSphinx module is found
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

# Debug or release mode?
set(CMAKE_BUILD_TYPE DEBUG)

# File we wish to export
set(ELF_FILE ${TARGET})
set(HEX_FILE ${TARGET}.hex)
set(BIN_FILE ${TARGET}.bin)

# Put the bins in build directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY build)

# Output compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# We will force the use of ARMs 9-2019-q4-major GNU toolchain
if (NOT CMAKE_COMPILER_IS_GNUCC OR NOT CMAKE_CXX_COMPILER_VERSION MATCHES "9.2.1")
    message(FATAL_ERROR "Compile must be gcc-arm-none-eabi-9-2019-q4-major, see documentation.")
endif ()

# Toolchain helper functions
include(cmake/binutils-arm-none-eabi.cmake)

# GDB helper functions
include(cmake/gdb-helper.cmake)

# Object build options
set(OBJECT_GEN_FLAGS "-mcpu=cortex-m0plus ")

# Setup target speicific compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OBJECT_GEN_FLAGS}" CACHE INTERNAL "C Compiler options")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OBJECT_GEN_FLAGS}" CACHE INTERNAL "C++ Compiler options")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} ${OBJECT_GEN_FLAGS}" CACHE INTERNAL "ASM Compiler options")

add_subdirectory(ASF ASF)
add_subdirectory(boot)
add_subdirectory(app)

# Create executable
add_executable(${PROJECT_NAME}-boot $<TARGET_OBJECTS:boot>)
add_executable(${PROJECT_NAME}-app $<TARGET_OBJECTS:app>)

# Create executable
target_link_libraries(${PROJECT_NAME}-boot ASF)
target_link_libraries(${PROJECT_NAME}-app ASF)

# Setup linker script for boot loader
set_target_properties(${PROJECT_NAME}-app ASF PROPERTIES
        LINKER_SCRIPT bootloader.ld
        LINK_FLAGS "${LINK_FLAGS} -Wl,--start-group -larm_cortexM0l_math -lm  -Wl,--end-group -L${CMAKE_SOURCE_DIR}/ASF/thirdparty/CMSIS/Lib/GCC -L${CMAKE_SOURCE_DIR}/linker -Wl,--gc-sections -mcpu=cortex-m0plus -Tapplication.ld"
)

# Setup linker script for application
set_target_properties(${PROJECT_NAME}-boot PROPERTIES
        LINKER_SCRIPT application.ld
        LINK_FLAGS "${LINK_FLAGS} -Wl,--start-group -larm_cortexM0l_math -lm  -Wl,--end-group -L${CMAKE_SOURCE_DIR}/ASF/thirdparty/CMSIS/Lib/GCC -L${CMAKE_SOURCE_DIR}/linker -Wl,--gc-sections -mcpu=cortex-m0plus -Tbootloader.ld"
)

# Print stats on the elf file
print_section_sizes(${PROJECT_NAME}-boot)
print_section_sizes(${PROJECT_NAME}-app)

# Generate hex and bin outputs from elf file
create_hex_output(${PROJECT_NAME}-boot)
create_hex_output(${PROJECT_NAME}-app)

create_bin_output(${PROJECT_NAME}-boot)
create_bin_output(${PROJECT_NAME}-app)

# Setup gdb
generate_run_gdb_openocd(${TARGET})

# Create boot loaded application binary
add_custom_target(${TARGET}-boot-padded.bin ALL DEPENDS ${PROJECT_NAME}-boot.bin ${PROJECT_NAME}-app.bin
        COMMAND ${CMAKE_OBJCOPY} --pad-to=0x4000 --gap-fill=0xFF -Obinary ${PROJECT_NAME}-boot ${PROJECT_NAME}-boot-padded.bin
        COMMAND copy /b /Y \"${PROJECT_NAME}-boot-padded.bin\"+\"${PROJECT_NAME}-app.bin\" ${TARGET}-boot-app.bin
        COMMAND ${CMAKE_OBJCOPY} -I binary -O elf32-little -B arm -S --change-section-address .data=0x00000000 ${TARGET}-boot-app.bin ${TARGET}-boot-app.elf
        WORKING_DIRECTORY build
        COMMENT "Objcopying ${TARGET} to boot loaded application firmware: ${TARGET}-boot-app.bin")


