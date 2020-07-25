##
## Configure the ASF framework
##

PROJECT(ASF LANGUAGES CXX)

# We'll request a rather new version of cmake becuase we can
cmake_minimum_required(VERSION 3.13)


# Configs
ADD_DEFINITIONS(
        -D__SAMR35J17B__
)

# Configs ASF defines
#ADD_DEFINITIONS(
        #-DDEBUG
#        -DBOARD=USER_BOARD
 #       -DARM_MATH_CM0PLUS=false
 #       -DSYSTICK_MODE
 #       -DEXTINT_CALLBACK_MODE=false
 #       -DRTC_COUNT_ASYNC=true
 #       -DSPI_CALLBACK_MODE=false
        #-DUSART_CALLBACK_MODE=true
 #       -DTC_ASYNC=true
  #      -DWDT_CALLBACK_MODE=false
  #      -DI2C_MASTER_CALLBACK_MODE=false
#)

if (CMAKE_HOST_SYSTEM_NAME STREQUAL Windows)
    ADD_DEFINITIONS(
            -DWIN32
    )
endif()


# Includes folders for library
INCLUDE_DIRECTORIES(
        "config"

        "atmel/CMSIS-Core/Include"
        "atmel/SAMR35_DFP/include"

        "common/boards"

        "sam0/utils"
        "sam0/utils/header_files"
        "sam0/utils/preprocessor"

        "thirdparty/CMSIS/Include"
        "thirdparty/CMSIS/Lib/GCC"

        "common/utils"
        "sam0/utils/cmsis/samr35/include"
        "sam0/utils/cmsis/samr35/source"

        "sam0/drivers/system"

        "sam0/drivers/system/clock/clock_samr35"
        "sam0/drivers/system/clock"

        "sam0/drivers/system/interrupt"
        "sam0/drivers/system/interrupt/system_interrupt_samr35"

        "sam0/drivers/system/pinmux"

        "sam0/drivers/system/power/power_sam_l"
        "sam0/drivers/system/power"

        "sam0/drivers/system/reset/reset_sam_l"
        "sam0/drivers/system/reset"

        "common2/boards/user_board"

        "sam0/drivers/nvm"

        "sam0/drivers/port"

        "sam0/services/eeprom/emulator/main_array"

        "sam0/utils/stdio/stdio_serial"

        "common/services/serial"

        "common2/services/delay"
        "common2/services/delay/sam0"

        "sam0/drivers/extint"
        "sam0/drivers/extint/extint_sam_l_c"

        "sam0/drivers/rtc"
        "sam0/drivers/rtc/rtc_sam_l_c"

        "sam0/drivers/sercom"
        "sam0/drivers/sercom/spi"
        "sam0/drivers/sercom/usart"

        "thirdparty/wireless/addons/sio2host/uart"

        "sam0/drivers/tc"
        "sam0/drivers/tc/tc_sam_l_c"

        "sam0/drivers/sercom/i2c"
        "sam0/drivers/sercom/i2c/i2c_sam0"

        "sam0/drivers/wdt"

        "sam0/drivers/bod"
        "sam0/drivers/bod/bod_sam_l"

        "."
)

# setup library files
file(GLOB_RECURSE folder_header *.h)
file(GLOB_RECURSE folder_source *.c)

add_library(${PROJECT_NAME} OBJECT ${folder_header} ${folder_source})
add_library(${PROJECT_NAME}-mini OBJECT
        asf-mini.h
        common/utils/interrupt/interrupt_sam_nvic.c
        sam0/drivers/port/port.c
        sam0/drivers/system/clock/clock_samr35/clock.c
        sam0/drivers/system/clock/clock_samr35/gclk.c
        sam0/drivers/system/pinmux/pinmux.c
        sam0/utils/cmsis/samr35/source/system_samr35.c
        sam0/utils/cmsis/samr35/source/gcc/startup_samr35.c
        sam0/drivers/nvm/nvm.c
        sam0/drivers/sercom/usart/usart.c
        sam0/drivers/sercom/usart/usart_interrupt.c
        sam0/drivers/sercom/sercom.c
        sam0/drivers/sercom/sercom_interrupt.c
        common/services/serial/serial.h
        common/services/serial/sam0_usart/usart_serial.h
        sam0/utils/stdio/write.c
        sam0/utils/stdio/read.c
        sam0/utils/stdio/stdio_serial/stdio_serial.h
        thirdparty/wireless/addons/sio2host/uart/sio2host.c
        atmel/SAMR35_DFP/include/component/nvmctrl.h
        sam0/utils/syscalls/gcc/syscalls.c
        )

target_compile_definitions(${PROJECT_NAME}
        PUBLIC
        __SAMR35J17B__
        BOARD=USER_BOARD
        ARM_MATH_CM0PLUS=false
        SYSTICK_MODE
        EXTINT_CALLBACK_MODE=false
        RTC_COUNT_ASYNC=true
        SPI_CALLBACK_MODE=false
        USART_CALLBACK_MODE=true
        TC_ASYNC=true
        WDT_CALLBACK_MODE=false
        I2C_MASTER_CALLBACK_MODE=false
)

target_compile_definitions(${PROJECT_NAME}-mini
        PUBLIC
        __SAMR35J17B__
        BOARD=USER_BOARD
        ARM_MATH_CM0PLUS=false
        SYSTICK_MODE
        EXTINT_CALLBACK_MODE=false
        RTC_COUNT_ASYNC=true
        SPI_CALLBACK_MODE=false
        USART_CALLBACK_MODE=true
        TC_ASYNC=true
        WDT_CALLBACK_MODE=false
        I2C_MASTER_CALLBACK_MODE=false
        )

# Export the include paths
target_include_directories(${PROJECT_NAME}
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/atmel/SAMR35_DFP/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common/boards>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/utils>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/utils/header_files>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/utils/preprocessor>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/CMSIS/Include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/CMSIS/Lib/GCC>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common/utils>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/utils/cmsis/samr35/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/utils/cmsis/samr35/source>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/system>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/system/clock/clock_samr35>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/system/clock>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/system/interrupt>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/system/interrupt/system_interrupt_samr35>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/system/pinmux>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/system/power/power_sam_l>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/system/power>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/system/reset/reset_sam_l>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/system/reset>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common2/boards/user_board>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/nvm>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/port>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/services/eeprom/emulator/main_array>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/utils/stdio/stdio_serial>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common/services/serial>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common2/services/delay>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common2/services/delay/sam0>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/extint>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/extint/extint_sam_l_c>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/rtc>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/rtc/rtc_sam_l_c>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/sercom>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/sercom/spi>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/sercom/usart>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/wireless/addons/sio2host/uart>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/tc>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/tc/tc_sam_l_c>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/sercom/i2c>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/sercom/i2c/i2c_sam0>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/wdt>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/bod>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/bod/bod_sam_l>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/config>
        $<INSTALL_INTERFACE:include>)

target_include_directories(${PROJECT_NAME}-mini
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/atmel/SAMR35_DFP/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common/boards>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/utils>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/utils/header_files>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/utils/preprocessor>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/CMSIS/Include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/CMSIS/Lib/GCC>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common/utils>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/utils/cmsis/samr35/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/utils/cmsis/samr35/source>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/system>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/system/clock/clock_samr35>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/system/clock>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/system/interrupt>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/system/interrupt/system_interrupt_samr35>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/system/pinmux>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/system/power/power_sam_l>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/system/power>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/system/reset/reset_sam_l>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/system/reset>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common2/boards/user_board>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/nvm>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/port>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/services/eeprom/emulator/main_array>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/utils/stdio/stdio_serial>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common/services/serial>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common2/services/delay>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/common2/services/delay/sam0>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/extint>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/extint/extint_sam_l_c>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/rtc>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/rtc/rtc_sam_l_c>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/sercom>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/sercom/spi>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/sercom/usart>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/wireless/addons/sio2host/uart>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/tc>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/tc/tc_sam_l_c>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/sercom/i2c>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/sercom/i2c/i2c_sam0>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/wdt>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/bod>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sam0/drivers/bod/bod_sam_l>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/config>
        $<INSTALL_INTERFACE:include>)

