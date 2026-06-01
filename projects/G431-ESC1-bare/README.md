# G431-ESC1-Bare

A sample project for the STMicro [B-G431B-ESC1 Discovery Kit](https://www.st.com/en/evaluation-tools/b-g431b-esc1.html),
which is a three-phase brushless motor driver development kit containing an
[STM32G431CBU6](https://www.st.com/en/microcontrollers-microprocessors/stm32g431cb.html)
microcontroller, a gate driver, and a three-phase MOSFET bridge. The board also
contains a detachable section with a USB connector and an embedded STLink V2
debugging interface, which has SWD and UART interfaces to the main MCU.

The microcontroller has a 170MHz Cortex-M4 core with FPU, 128K of flash, and
32K of RAM, in a 48-pin UFQFPN package.

This project demonstrates EMBLOCS integration using a command-line CMake build
workflow, without relying on any STM32 HAL or IDE automation.

## Board Documentation

STMicro data for the STM32G431CB microcontroller:
- [Datasheet](https://www.st.com/resource/en/datasheet/stm32g431cb.pdf)
- [Reference Manual](https://www.st.com/resource/en/reference_manual/rm0440-stm32g4-series-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)
- [Programming Manual](https://www.st.com/resource/en/programming_manual/pm0214-stm32-cortexm4-mcus-and-mpus-programming-manual-stmicroelectronics.pdf)

STMicro does not publish a schematic for the board. Digikey has some links on
their [product page](https://www.digikey.at/reference-designs/en/motor-control/motor-control/2840)
for the board, but the schematic link to STMicro is dead. Fortunately the same
Digikey page has PNG files for the individual schematic pages. It's not clear
what version of the board the schematics are for; it looks like STMicro has
made three versions.

## Required Tools

| Tool | Purpose | Notes |
|------|---------|-------|
| `cmake` | Build system generator | Version 3.13 or higher |
| `ninja` | Build tool | |
| `arm-none-eabi-gcc` | Cross-compiler | ARM GNU Toolchain |
| `python` | Runs EMBLOCS tooling | Version 3.9 or higher |
| `openocd` | Flashing and debugging | xPack OpenOCD recommended |

All tools must be on your PATH.

On Linux: `sudo apt install cmake ninja-build gcc-arm-none-eabi python3 openocd`

## STM32CubeG4 Package

This project requires the STM32CubeG4 firmware package for CMSIS device headers
and LL driver headers. The build system searches for it automatically in standard
locations. If not found, a helpful error message explains where to download it
and how to make it findable.

Download from:
- [GitHub](https://github.com/STMicroelectronics/STM32CubeG4)
- [STMicro](https://www.st.com/en/embedded-software/stm32cubeg4.html)

The standard locations searched by the build system are listed below.
Ideally the package is installed in one of these places. If your
installation must be elsewhere, set the `STM32_G4_PATH` environment
variable to the package root, or pass `-DSTM32_G4_PATH=C:/path/to/package`
to cmake.

Windows:
- `C:\STM32\STM32Cube_FW_G4*`
- `C:\STM32CubeG4\STM32Cube_FW_G4*`
- `%USERPROFILE%\STM32\STM32Cube_FW_G4*`
Linux/Mac:
- `/opt/stm32/STM32Cube_FW_G4*`
- `~/STM32/STM32Cube_FW_G4*`
- `~/stm32cube/STM32Cube_FW_G4*`

See `FindSTM32G4.cmake` for the complete list of searched locations.

## Environment Setup - Windows

The following environment variables must be set:

| Variable | Purpose |
|----------|---------|
| `ARM_GNU_TOOLCHAIN_PATH` | Root of arm-none-eabi-gcc toolchain |
| `OPENOCD_PATH` | Root of OpenOCD installation |

The following must be on your PATH:
- `%ARM_GNU_TOOLCHAIN_PATH%\bin` — compiler, GDB, and related tools
- `cmake` executable
- `ninja` executable
- `python` executable
- OpenOCD `bin/` directory

## Environment Setup - Linux

On Linux, tools live in well-defined directories that are already
on PATH.  Just install anything that is missing:

`sudo apt install cmake ninja-build gcc-arm-none-eabi python3 openocd`

You still need to install the STM32CubeG4 package, and if not in a
standard location, add  `export STM32_G4_PATH=/path/to/package` to
your ~/.bashrc or equivalent.

Note: the `openocd` package in some distributions may be outdated.
If you have connection issues, consider installing
[xPack OpenOCD](https://xpack.github.io/openocd/) instead.


## Building

All commands run from the project directory.

**First time, or after adding/removing blocks:**
```
cmake -B build -G Ninja
```

**Normal build:**
```
cmake --build build
```

**Partial clean:**
```
cmake --build build --target clean
```

**Full clean:**
```
cmake -E rm -rf build
```

## Flashing

Connect the board via USB — the onboard STLink appears as a debugger and serial
port. Then from the project directory:

```
openocd -f flash.cfg
```

## Debugging

Start an OpenOCD GDB server:

```
openocd -f debug.cfg
```

Then connect GDB in another terminal:

```
arm-none-eabi-gdb build/main.elf
(gdb) target remote localhost:3333
(gdb) monitor reset halt
(gdb) continue
```

Stop OpenOCD with Ctrl+C when done.

VS Code F5 debugging is configured in `.vscode/launch.json` and manages
OpenOCD automatically.

## VS Code Integration

Open this project folder directly in VS Code:
**File → Open Folder** → navigate to `projects/G431-ESC1-bare`

IntelliSense is driven by `build/compile_commands.json`. Run the configure
step at least once before opening source files in VS Code.

Files in the `.vscode` directory:

- `c_cpp_properties.json` — include paths and compiler for IntelliSense.
  When creating a new project, change the `name` field.
- `extensions.json` — recommended VS Code extensions. Can be copied unmodified.
- `launch.json` — debug configuration for F5 debugging via the onboard STLink.
  When creating a new project, update the `name` field and `executable` path
  to match the `TARGET` variable in `CMakeLists.txt`.
- `settings.json` — project-level settings. Can be copied unmodified.
- `tasks.json` — configure, compile, flash, and reset tasks. When creating a
  new project, no changes needed since flash and reset use `flash.cfg` and
  `debug.cfg` which contain the project-specific settings.

## Creating a New Project from This Template

1. Copy this project directory to a new location
2. Edit `CMakeLists.txt` — the configuration block at the top describes
   exactly what to change:
   - `PROJECT_NAME` — your project's name
   - `BLOCS_FILE` — stem name of your `.blocs` system definition file
   - `TARGET` — your executable name
   - CPU flags if targeting a different STM32 variant
3. Replace `blink.blocs` with your own system definition file
4. Update `emblocs.json` if your block source files are in a different location
5. Update `main.c` to call your thread functions
6. Update `flash.cfg` — change `HEX_FILE` to match your `TARGET` name
7. Run `cmake -B build -G Ninja` to configure

## Hardware

Target board: B-G431B-ESC1
Onboard LED: PC6
Debugger: Onboard STLink V2 (SWD interface)
Console UART: Connected to STLink virtual COM port and external pins
