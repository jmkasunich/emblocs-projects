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

# Pull in local machine configuration (paths to tools, SDK, etc.)
# See local.cmake.template for host-specific configuration.
include(local.cmake OPTIONAL RESULT_VARIABLE LOCAL_CMAKE_FOUND)
if(NOT LOCAL_CMAKE_FOUND)
    message(WARNING "local.cmake not found - relying on environment variables and PATH")
endif()

# Minimum CMake version required by Pico SDK
cmake_minimum_required(VERSION 3.13)

# Generate compile_commands.json for IntelliSense
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Pull in Pico SDK bootstrapping - must be before project()
include(pico_sdk_import.cmake)

# Target platform and board - must be set before project() and pico_sdk_init()
set(PICO_PLATFORM rp2040)
set(PICO_BOARD pico)

# Define project name and languages
project(pi-pico-bare-refactor C CXX ASM)

# Set C and C++ standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the Pico SDK - must be after project()
pico_sdk_init()

# Project target name - change this to rename all output files
set(TARGET blink)

# Path to emblocs - override in local.cmake if not using standard layout
if(NOT DEFINED EMBLOCS_DIR)
    set(EMBLOCS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../submodules/emblocs)
endif()

# Path to emblocs headers
set(EMBLOCS_INC ${EMBLOCS_DIR}/src/emblocs)

# Define the executable first - ${TARGET}.cmake will add sources to it
add_executable(${TARGET}
    main.c
)

# Run blocs_compiler.py at configure time to generate source files
# CMake will auto-reconfigure if ${TARGET}.blocs or emblocs.json change
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}.blocs
    ${CMAKE_CURRENT_SOURCE_DIR}/emblocs.json
)

# Run blocs_compiler.py at configure time
execute_process(
    COMMAND python
        ${EMBLOCS_DIR}/python/blocs_compiler.py
        ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}.blocs
        ${CMAKE_BINARY_DIR}
    RESULT_VARIABLE BLOCS_COMP_RESULT
    OUTPUT_VARIABLE BLOCS_COMP_OUTPUT
    ERROR_VARIABLE BLOCS_COMP_OUTPUT
)
if(NOT BLOCS_COMP_RESULT EQUAL 0)
    message(FATAL_ERROR "blocs_compiler.py failed:\n${BLOCS_COMP_OUTPUT}")
else()
    message(STATUS "${BLOCS_COMP_OUTPUT}")
endif()

# Include the generated cmake rules:
# This adds generated source files to ${TARGET}
include(${CMAKE_BINARY_DIR}/${TARGET}.cmake)

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
    ${CMAKE_BINARY_DIR}
    ${EMBLOCS_INC}
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
