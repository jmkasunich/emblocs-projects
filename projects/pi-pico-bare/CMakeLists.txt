# Pull in local machine configuration (paths to tools, SDK, etc.)
# See local.cmake.template for host-specific configuration.
include(local.cmake OPTIONAL RESULT_VARIABLE LOCAL_CMAKE_FOUND)
if(NOT LOCAL_CMAKE_FOUND)
    message(WARNING "local.cmake not found - relying on environment variables and PATH")
endif()

# Minimum CMake version required by Pico SDK
cmake_minimum_required(VERSION 3.13)

# info for IntelliSense and other tools - not required for build
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Pull in Pico SDK bootstrapping - must be before project()
include(pico_sdk_import.cmake)

# Target platform and board - must be set before project() and pico_sdk_init()
# Change to "pico_w" and include appropriate libraries if using Pico W
set(PICO_PLATFORM rp2040)
set(PICO_BOARD pico)

# Define project name and languages
project(pi-pico-bare C CXX ASM)

# Set C and C++ standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the Pico SDK - must be after project()
pico_sdk_init()

# Define the executable
add_executable(main
    main.c
)

# Header file locations
target_include_directories(main PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link against Pico SDK
target_link_libraries(main
    pico_stdlib
)

# Generate .uf2 and other output formats
# Requires picotool - see README for installation instructions
#pico_add_extra_outputs(main)