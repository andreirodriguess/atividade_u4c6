

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(atividade_u4c6 C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(atividade_u4c6 atividade_u4c6.c inc/ssd1306.c)

pico_set_program_name(atividade_u4c6 "atividade_u4c6")
pico_set_program_version(atividade_u4c6 "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(atividade_u4c6 0)
pico_enable_stdio_uart(atividade_u4c6 1)
pico_enable_stdio_usb(atividade_u4c6 0)
pico_enable_stdio_usb(atividade_u4c6 1)

pico_generate_pio_header(atividade_u4c6 ${CMAKE_CURRENT_LIST_DIR}/atividade_u4c6.pio)

target_sources(atividade_u4c6 PRIVATE atividade_u4c6.c)

# Add the standard library to the build
target_link_libraries(atividade_u4c6
        pico_stdlib
        hardware_pio
        hardware_adc
        pico_bootrom)

# Add the standard include files to the build
target_include_directories(atividade_u4c6 PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
target_link_libraries(atividade_u4c6 
        hardware_i2c
        )

pico_add_extra_outputs(atividade_u4c6)