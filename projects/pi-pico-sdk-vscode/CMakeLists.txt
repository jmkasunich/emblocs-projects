# Generated Cmake Pico project file

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
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(pi-pico-sdk-vscode C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(pi-pico-sdk-vscode main.c )

# ---- EMBLOCS configuration ----
set(BLOCS_FILE blink)
set(TARGET ${CMAKE_PROJECT_NAME})
set(EMBLOCS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../submodules/emblocs)
include(${EMBLOCS_DIR}/cmake/emblocs.cmake)
# ---- end EMBLOCS configuration ----

pico_set_program_name(pi-pico-sdk-vscode "pi-pico-sdk-vscode")
pico_set_program_version(pi-pico-sdk-vscode "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(pi-pico-sdk-vscode 1)
pico_enable_stdio_usb(pi-pico-sdk-vscode 0)

# Add the standard library to the build
target_link_libraries(pi-pico-sdk-vscode
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(pi-pico-sdk-vscode PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

pico_add_extra_outputs(pi-pico-sdk-vscode)

