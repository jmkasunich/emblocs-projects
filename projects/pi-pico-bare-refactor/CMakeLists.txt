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

# Path to emblocs headers
set(EMBLOCS_INC ${CMAKE_CURRENT_SOURCE_DIR}/../../submodules/emblocs/src/emblocs)

# Define the executable first - blink.cmake will add sources to it
add_executable(main
    main.c
)

# Pull in the generated source files and compile rules from Tool 2
# Run variant_compiler.py manually to regenerate when blink.blocs changes
include(${CMAKE_BINARY_DIR}/blink.cmake)

# Compiler flags - set explicitly rather than relying on CMAKE_BUILD_TYPE
# -g    include debug symbols
# -Og   optimize for debugging (minimal optimization, preserves structure)
target_compile_options(main PRIVATE
    -g
    -Og
)

# Header file locations
target_include_directories(main PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${EMBLOCS_INC}
)

# Link against Pico SDK - linked once for all sources
target_link_libraries(main
    pico_stdlib
)

# Generate .uf2 and other output formats
# Requires picotool - see README for installation instructions
# pico_add_extra_outputs(main)
