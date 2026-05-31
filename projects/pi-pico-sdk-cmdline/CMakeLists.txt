# The following block supports the Raspberry Pi Pico VS Code extension.
# It is safe to remove if you are not using that extension.
# If kept, update version numbers when upgrading the SDK or toolchain.
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

# Minimum CMake version required by Pico SDK
cmake_minimum_required(VERSION 3.13)

# Generate compile_commands.json for IntelliSense
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ============================================================
# Project configuration - edit these lines
# ============================================================

# Project name - used by CMake to identify this project.
# Note: this is not the same as TARGET (the executable name).
# For this project they happen to be different: the project is named
# after the build approach (cmdline) while the executable is named
# after what it does (blink).
set(PROJECT_NAME pi-pico-sdk-cmdline)

# Name of blocs file (without extension) that defines emblocs content
set(BLOCS_FILE blink)

# Target name - change to rename all output files
set(TARGET blink)

# Path to emblocs - override in local.cmake if not using standard layout
set(EMBLOCS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../submodules/emblocs)

# Target platform and board
set(PICO_PLATFORM rp2040)
set(PICO_BOARD pico)

# ============================================================

# Pull in Pico SDK bootstrapping - must be before project()
include(pico_sdk_import.cmake)

# Define project name and languages
project(${PROJECT_NAME} C CXX ASM)

# Set C and C++ standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the Pico SDK - must be after project()
pico_sdk_init()

# Define the executable - add non-emblocs source files here
add_executable(${TARGET}
    main.c
)

# include emblocs.cmake to run blocs_compiler.py at configure time
include(${EMBLOCS_DIR}/cmake/emblocs.cmake)

# Compiler flags - set explicitly rather than relying on CMAKE_BUILD_TYPE
# -g    include debug symbols
# -Og   optimize for debugging (minimal optimization, preserves structure)
target_compile_options(${TARGET} PRIVATE
    -g
    -Og
    -Wall
    -Wextra
)

# Header file locations
target_include_directories(${TARGET} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link against Pico SDK - linked once for all sources
target_link_libraries(${TARGET}
    pico_stdlib
)

# Generate map file for symbol and memory layout inspection
target_link_options(${TARGET} PRIVATE
    -Wl,-Map=${CMAKE_BINARY_DIR}/${TARGET}.map
)

# Report size of generated code
add_custom_command(
    TARGET ${TARGET}
    POST_BUILD
    COMMAND arm-none-eabi-size $<TARGET_FILE:${TARGET}>
    COMMENT "Calculating target size:"
)


# Generate .uf2 and other output formats
# Requires picotool - see README for installation instructions
# pico_add_extra_outputs(${TARGET})
