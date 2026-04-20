# Pi-Pico-Bare

A bare-metal sample project for the [Raspberry Pi Pico](https://www.raspberrypi.com/documentation/microcontrollers/pico-series.html#pico1) board, using the [RP2040](https://www.raspberrypi.com/documentation/microcontrollers/microcontroller-chips.html#rp2040) MCU. The RP2040 provides two 125MHz Cortex-M0+ cores, 264KB of RAM, and no on-chip flash. The Pico board integrates a 2MB QSPI flash chip.

This project uses the Raspberry Pi Pico SDK and CMake as its build system.

## Environment Setup

See the [emblocs-projects README](../../README.md) for full environment setup
instructions. The following Windows environment variables must be set before
building this project:

| Variable | Purpose | Example |
|----------|---------|---------|
| `PICO_SDK_PATH` | Root of the Pico SDK | `C:\pi_pico\pico-sdk` |
| `ARM_GNU_TOOLCHAIN_PATH` | Root of arm-none-eabi-gcc toolchain | `C:\ArmGCC\Arm-GNU-Toolchain-arm-none-eabi\14.2-rel1` |
| `OPENOCD_PATH` | Root of OpenOCD (needed for debugging) | `C:\openocd` |

The following must also be on your user PATH:
- `%ARM_GNU_TOOLCHAIN_PATH%\bin`
- cmake executable
- ninja executable

## Pico SDK Installation

Choose a location for the Pico SDK and clone it with submodules:
```cmd
git clone https://github.com/raspberrypi/pico-sdk.git --branch master
cd pico-sdk
git submodule update --init
```
Set `PICO_SDK_PATH` to point to this directory.

## Building

Copy `local.cmake.template` to `local.cmake`. Edit if your environment
variables differ from the defaults documented in that file.

From the project directory:
```cmd
configure.bat
cmake --build build
```

The configure step only needs to be re-run when `CMakeLists.txt` changes.
For subsequent builds after editing source files, just run:
```cmd
cmake --build build
```

## VS Code

Open `pi-pico-bare.code-workspace` from the `emblocs-projects` root folder
using **File → Open Workspace from File**. This gives you a focused view of
this project and the emblocs core library, with IntelliSense correctly
configured via the build system.

## Hardware

Target board: Raspberry Pi Pico (standard, not Pico W)
Onboard LED: GP25

