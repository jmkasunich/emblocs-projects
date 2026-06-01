# FindSTM32G4.cmake
# Finds the STM32CubeG4 firmware package (CMSIS headers and LL drivers)
#
# This module searches for the STM32CubeG4 package in the following locations,
# in order of priority:
#
#   1. STM32_G4_PATH environment variable
#   2. STM32_G4_PATH CMake variable (set on command line or in local config)
#   3. Common installation locations
#
# If not found, a helpful error message is printed explaining how to obtain
# and install the package.
#
# Result variables:
#   STM32G4_FOUND       - True if the package was found
#   STM32G4_INCLUDE_DIRS - List of include directories needed to use the package
#
# Usage in CMakeLists.txt:
#   find_package(STM32G4 REQUIRED)
#   target_include_directories(my_target PRIVATE ${STM32G4_INCLUDE_DIRS})

# Collect candidate search paths
set(_STM32G4_SEARCH_PATHS)

# 1. CMake variable (set via -DSTM32_G4_PATH=... or in local config)
if(DEFINED STM32_G4_PATH)
    list(APPEND _STM32G4_SEARCH_PATHS "${STM32_G4_PATH}")
endif()

# 2. Environment variable
if(DEFINED ENV{STM32_G4_PATH})
    list(APPEND _STM32G4_SEARCH_PATHS "$ENV{STM32_G4_PATH}")
endif()

# 3. Common installation locations (Windows)
if(CMAKE_HOST_WIN32)
    file(GLOB _WIN_CANDIDATES
        "C:/STM32/STM32Cube_FW_G4*"
        "C:/STM32CubeG4/STM32Cube_FW_G4*"
        "$ENV{USERPROFILE}/STM32/STM32Cube_FW_G4*"
        "C:/Program Files/STMicroelectronics/STM32Cube/Repository/STM32Cube_FW_G4*"
    )
    list(APPEND _STM32G4_SEARCH_PATHS ${_WIN_CANDIDATES})
endif()

# 3. Common installation locations (Linux/Mac)
if(CMAKE_HOST_UNIX)
    file(GLOB _UNIX_CANDIDATES
        "/opt/stm32/STM32Cube_FW_G4*"
        "$ENV{HOME}/STM32/STM32Cube_FW_G4*"
        "$ENV{HOME}/stm32cube/STM32Cube_FW_G4*"
    )
    list(APPEND _STM32G4_SEARCH_PATHS ${_UNIX_CANDIDATES})
endif()

# Search each candidate path for the key header
set(STM32G4_FOUND FALSE)
foreach(_path ${_STM32G4_SEARCH_PATHS})
    if(EXISTS "${_path}/Drivers/CMSIS/Device/ST/STM32G4xx/Include/stm32g4xx.h")
        set(STM32G4_ROOT "${_path}")
        set(STM32G4_FOUND TRUE)
        break()
    endif()
endforeach()

if(STM32G4_FOUND)
    set(STM32G4_INCLUDE_DIRS
        "${STM32G4_ROOT}/Drivers/CMSIS/Include"
        "${STM32G4_ROOT}/Drivers/CMSIS/Device/ST/STM32G4xx/Include"
        "${STM32G4_ROOT}/Drivers/STM32G4xx_HAL_Driver/Inc"
    )
    message(STATUS "Found STM32CubeG4 at ${STM32G4_ROOT}")
else()
    if(STM32G4_FIND_REQUIRED)
        message(FATAL_ERROR
            "STM32CubeG4 firmware package not found.\n"
            "Please download it from one of the following sources:\n"
            "  GitHub: https://github.com/STMicroelectronics/STM32CubeG4\n"
            "  STMicro: https://www.st.com/en/embedded-software/stm32cubeg4.html\n"
            "\n"
            "Then either:\n"
            "  a) Install it in one of the standard locations listed below, or\n"
            "  b) Set the STM32_G4_PATH environment variable to the package root, or\n"
            "  c) Pass -DSTM32_G4_PATH=/path/to/STM32Cube_FW_G4_Vx.x.x to cmake\n"
            "\n"
            "The package root should contain a 'Drivers' subdirectory.\n"
            "Searched in:\n"
            "  ${_STM32G4_SEARCH_PATHS}\n"
        )
    else()
        message(WARNING "STM32CubeG4 firmware package not found.")
    endif()
endif()
