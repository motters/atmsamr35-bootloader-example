#
# SAMR35 Boot Loaded Application Project Example
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

# Add libs
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
        LINKER_SCRIPT application.ld
        LINK_FLAGS "${LINK_FLAGS} -Wl,--start-group -larm_cortexM0l_math -lm -Wl,-Map=${PROJECT_NAME}-app.map -Wl,--end-group -L${CMAKE_SOURCE_DIR}/ASF/thirdparty/CMSIS/Lib/GCC -L${CMAKE_SOURCE_DIR}/linker -Wl,--gc-sections -mcpu=cortex-m0plus -Tapplication.ld"
)

# Setup linker script for application
set_target_properties(${PROJECT_NAME}-boot PROPERTIES
        LINKER_SCRIPT application.ld
        LINK_FLAGS "${LINK_FLAGS} -Wl,--start-group -larm_cortexM0l_math -lm -Wl,-Map=${PROJECT_NAME}-boot.map -Wl,--end-group -L${CMAKE_SOURCE_DIR}/ASF/thirdparty/CMSIS/Lib/GCC -L${CMAKE_SOURCE_DIR}/linker -Wl,--gc-sections -mcpu=cortex-m0plus -Tbootloader.ld"
)

# Print stats on the elf file
print_section_sizes(${PROJECT_NAME}-boot)
print_section_sizes(${PROJECT_NAME}-app)

# Generate the openocd config files to be used by CLion or command line
set(BOOTLOADER_OFFSET 0x4000)
generate_run_gdb_openocd("${PROJECT_NAME}-boot" "0x0000")
generate_run_gdb_openocd("${PROJECT_NAME}-app" "${BOOTLOADER_OFFSET}")

# Delete previous bin files
# Important to ensure we program a fresh bin (some windows bug)
add_custom_command(TARGET ${PROJECT_NAME}-app
        PRE_BUILD
        COMMAND DEL ${PROJECT_NAME}-app.bin ${PROJECT_NAME}-app.hex
        WORKING_DIRECTORY build
        COMMENT "Deleting auto generated files from application elf")
add_custom_command(TARGET ${PROJECT_NAME}-boot
        PRE_BUILD
        COMMAND DEL ${PROJECT_NAME}-boot.bin ${PROJECT_NAME}-boot.hex
        WORKING_DIRECTORY build
        COMMENT "Deleting auto generated files from boot elf")
add_custom_command(TARGET ${PROJECT_NAME}-boot ${PROJECT_NAME}-app
        PRE_BUILD
        COMMAND DEL ${PROJECT_NAME}-boot-app.bin
        WORKING_DIRECTORY build
        COMMENT "Deleting combined boot and application binary")

# Build binary files straight after the application has been built.
add_custom_command(TARGET ${PROJECT_NAME}-app
        POST_BUILD
        DEPENDS ${PROJECT_NAME}-app
        COMMAND ${CMAKE_OBJCOPY} -Obinary ${PROJECT_NAME}-app ${PROJECT_NAME}-app.bin
        WORKING_DIRECTORY build
        COMMENT "Generating bin file from application elf")
add_custom_command(TARGET ${PROJECT_NAME}-boot
        POST_BUILD
        DEPENDS ${PROJECT_NAME}-boot
        COMMAND ${CMAKE_OBJCOPY} --pad-to=${BOOTLOADER_OFFSET} --gap-fill=0xFF -Obinary ${PROJECT_NAME}-boot ${PROJECT_NAME}-boot.bin
        WORKING_DIRECTORY build
        COMMENT "Generating bin file from boot elf")

# Create a single binary file containing bootloader and application
add_custom_command(TARGET ${PROJECT_NAME}-app
        POST_BUILD
        DEPENDS ${PROJECT_NAME}-boot.bin ${PROJECT_NAME}-app.bin
        COMMAND copy /b /Y \"${PROJECT_NAME}-boot.bin\"+\"${PROJECT_NAME}-app.bin\" ${PROJECT_NAME}-boot-app.bin
        WORKING_DIRECTORY build
        COMMENT "Generating combined bootloader and application binary file with correct offsets")
add_custom_command(TARGET ${PROJECT_NAME}-boot
        POST_BUILD
        DEPENDS ${PROJECT_NAME}-boot.bin ${PROJECT_NAME}-app.bin
        COMMAND copy /b /Y \"${PROJECT_NAME}-boot.bin\"+\"${PROJECT_NAME}-app.bin\" ${PROJECT_NAME}-boot-app.bin
        WORKING_DIRECTORY build
        COMMENT "Generating combined bootloader and application binary file with correct offsets")