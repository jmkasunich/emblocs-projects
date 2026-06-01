# Minimum CMake version
cmake_minimum_required(VERSION 3.13)

# Generate compile_commands.json for IntelliSense
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ============================================================
# Project configuration - edit these lines
# ============================================================

# Project name
set(PROJECT_NAME G431-ESC1-bare)

# Name of blocs file (without extension) that defines emblocs content
set(BLOCS_FILE blink)

# Target executable name
set(TARGET main)

# Path to emblocs - change this if not using standard layout
set(EMBLOCS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../submodules/emblocs)

# CPU and FPU flags - specific to Cortex-M4 with FPU
set(CPU_FLAGS
    -mcpu=cortex-m4
    -march=armv7e-m
    -mthumb
    -mfloat-abi=hard
    -mfpu=fpv4-sp-d16
)

# Flags common to compiler and linker
set(COMMON_FLAGS
    ${CPU_FLAGS}
    -fmessage-length=0
    -ffunction-sections
    -fdata-sections
)

# Debug flags
set(DEBUG_FLAGS
    -ggdb
    -g1
    -Og
)

# Warning flags
set(WARNING_FLAGS
    -Wall
    -Wextra
    -Wfatal-errors
    -Wpacked
    -Winline
    -Wfloat-equal
    -Wconversion
    -Wlogical-op
    -Wpointer-arith
    -Wdisabled-optimization
    -Wunused-parameter
    -Wno-sign-conversion
    -Wstrict-aliasing=1
)

# ============================================================

# Toolchain must be set before project()
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_SIZE arm-none-eabi-size)

# Define project
project(${PROJECT_NAME} C CXX ASM)

# Find STM32G4 firmware package (CMSIS headers and LL drivers)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
find_package(STM32G4 REQUIRED)

# Define the executable
add_executable(${TARGET}
    main.c
    platform.c
    system_stm32g4xx.c
    startup_stm32g431kbtx.s
)

# include emblocs.cmake to run blocs_compiler.py at configure time
include(${EMBLOCS_DIR}/cmake/emblocs.cmake)

# Preprocessor defines
target_compile_definitions(${TARGET} PRIVATE
    STM32G431xx
    ARM_MATH_CM4
    __FPU_PRESENT=1U
    HSE_VALUE=8000000
    DEBUG
)

# Compiler flags
target_compile_options(${TARGET} PRIVATE
    ${COMMON_FLAGS}
    ${DEBUG_FLAGS}
    -std=gnu11
    ${WARNING_FLAGS}
    -save-temps=obj
)

# Include paths
target_include_directories(${TARGET} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${STM32G4_INCLUDE_DIRS}
)

# Linker flags and script
target_link_options(${TARGET} PRIVATE
    ${COMMON_FLAGS}
    ${DEBUG_FLAGS}
    -T${CMAKE_CURRENT_SOURCE_DIR}/STM32G431KBTX_FLASH.ld
    -Xlinker --gc-sections
    --specs=rdimon.specs
    --specs=nano.specs
    -Wl,-Map=${CMAKE_BINARY_DIR}/${TARGET}.map
)

# Generate .hex file after build
add_custom_command(
    TARGET ${TARGET}
    POST_BUILD
    COMMAND arm-none-eabi-objcopy -O ihex
        $<TARGET_FILE:${TARGET}>
        ${CMAKE_BINARY_DIR}/${TARGET}.hex
    COMMAND arm-none-eabi-size $<TARGET_FILE:${TARGET}>
    COMMENT "Generating hex file and size report"
)
