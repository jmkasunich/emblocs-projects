# Pi-Pico-SDK-CmdLine

A sample project for the [Raspberry Pi Pico](https://www.raspberrypi.com/documentation/microcontrollers/pico-series.html#pico1)
board, using the [RP2040](https://www.raspberrypi.com/documentation/microcontrollers/microcontroller-chips.html#rp2040)
MCU. The RP2040 provides two 125MHz Cortex-M0+ cores, 264KB of RAM,
and no on-chip flash. The Pico board integrates a 2MB QSPI flash chip.

This project demonstrates EMBLOCS integration using a command-line
workflow.  It uses the Raspberry Pi Pico SDK, but performs all build
steps without relying on IDE automation.  It is intended for users
who prefer to use the editor of their choice and invoke the build
process separately.

Although this project supports any editor, it does include vscode
configuration files so that features like Intellisense work correctly
in VSCode.

Users who wish to use the Raspberry Pi Pico VS-Code extension with its
IDE-Like `build`, `flash`, `debug`, buttons can refer to sample
project `pi-pico-sdk-vscode`; this sample makes the under-the-hood
workings more visible.

## Required Tools

The following tools must be installed and available:

| Tool | Purpose | Notes |
|------|---------|-------|
| `cmake` | Build system generator | Version 3.13 or higher |
| `ninja` | Build tool | Preferred over make for Pico projects |
| `arm-none-eabi-gcc` | Cross-compiler | ARM GNU Toolchain |
| `python` | Runs EMBLOCS tooling | Version 3.9+ |
| `openocd` | Flashing and debugging | xPack OpenOCD recommended |

## Pico SDK Installation

If you do not have the Pico SDK installed, clone it with submodules:

```
git clone https://github.com/raspberrypi/pico-sdk.git --branch master
cd pico-sdk
git submodule update --init
```

Set `PICO_SDK_PATH` to point to this directory.

## Environment Setup - Windows

The following environment variables must be set:

| Variable | Purpose | Example |
|----------|---------|---------|
| `PICO_SDK_PATH` | Root of the Pico SDK | `C:\pi_pico\pico-sdk` |
| `ARM_GNU_TOOLCHAIN_PATH` | Root of arm-none-eabi-gcc toolchain | `C:\ArmGCC\Arm-GNU-Toolchain-arm-none-eabi\14.2-rel1` |

The following must also be on your PATH:
- `%ARM_GNU_TOOLCHAIN_PATH%\bin` — compiler, GDB, and related tools
- `cmake` executable
- `ninja` executable
- `python` executable
- `openocd` bin/ directory; adds executable to path

If you installed the Raspberry Pi Pico VS Code extension, cmake and ninja
are already installed at `%USERPROFILE%\.pico-sdk\cmake\` and
`%USERPROFILE%\.pico-sdk\ninja\` respectively. Add those directories to
your PATH to use them from the command line.

## Environment Setup - Linux

On Linux, tools live in well-defined directories that are already
on PATH.  Just install anything that is missing:

`sudo apt install cmake ninja-build gcc-arm-none-eabi python3 openocd`

You still need to clone the Pico SDK and add `export PICO_SDK_PATH=/path/to/pico-sdk`
to your ~/.bashrc or equivalent.

Note: the `openocd` package in some distributions may be outdated.
If you have connection issues, consider installing
[xPack OpenOCD](https://xpack.github.io/openocd/) instead.

## Building

All commands are run from the project directory (where `CMakeLists.txt` lives).

**First time, or after adding/removing blocks from the `.blocs` file:**
```
cmake -B build -G Ninja
```
This configures the build system. CMake automatically runs `blocs_compiler.py`
to generate the EMBLOCS source files. The `build/` directory is created if
it does not exist.

**Normal build (after editing source files):**
```
cmake --build build
```
CMake detects changes to `.blocs` files, block source files, and
`emblocs.json` and automatically reconfigures if needed before building.

**Partial clean (remove compiled artifacts, keep configuration):**
```
cmake --build build --target clean
```

**Full clean (remove everything and start fresh):**
```
cmake -E rm -rf build
```
After a full clean, run the configure step again before building.

## Flashing

Connect a [Raspberry Pi Debug Probe](https://www.raspberrypi.com/products/debug-probe/)
via SWD, then from the project directory:

```
openocd -f flash.cfg
```

## Debugging

For occasional debugging needs, a GDB server can be started using the
provided OpenOCD configuration file:

```
openocd -f debug.cfg
```
This starts OpenOCD and leaves it running as a GDB server on port 3333.
The Pico is halted and waiting for a GDB connection. Connect with:

```
arm-none-eabi-gdb build/blink.elf
(gdb) target remote localhost:3333
(gdb) monitor reset halt
(gdb) continue
```
Stop OpenOCD with Ctrl+C when done.

Note: `debug.cfg` does not flash the Pico. Run `openocd -f flash.cfg`
first to ensure the binary on the chip matches your current build.

**VS Code users:** F5 debugging via the Debug Probe is configured in
`.vscode/launch.json` and manages OpenOCD automatically. The `debug.cfg`
file is only needed for command-line GDB sessions.

**A note on debugging real-time code:** Breakpoints halt the processor
while the outside world keeps moving. For EMBLOCS systems with periodic
threads, this means timers continue, hardware state changes, and resuming
from a breakpoint may produce unexpected behavior. Print statements via
UART and the EMBLOCS runtime monitor are often more useful than
single-stepping through thread functions.


## VS Code Integration

VS Code may be used as an editor alongside the command-line build. Open this
project folder directly:
**File → Open Folder** → navigate to `projects/pi-pico-sdk-cmdline`

IntelliSense is driven by `build/compile_commands.json` generated during the
configure step. Run the configure step at least once before opening files in
VS Code to avoid red squiggles on include statements.

Files in the `.vscode` directory:

- `c_cpp_properties.json` — include paths and compiler for IntelliSense.
  When creating a new project, change the `name` field to match your project.
- `extensions.json` — recommended VS Code extensions.
  Can be copied unmodified when starting a new project.
- `launch.json` — debug configurations for F5 debugging via the Debug Probe.
  When creating a new project, update the `name` fields and `executable`
  path to match the `TARGET` variable in `CMakeLists.txt`.
- `settings.json` — suppresses the Pico extension and CMake Tools from
  interfering with the command-line build. Can be copied unmodified.
- `tasks.json` — compile, flash, and reset tasks available via
  Terminal → Run Task. When creating a new project, update `.elf` filename
  references to match the `TARGET` variable in `CMakeLists.txt`.

Linux only:  Some vscode configuration .json files may use
`${env:ARM_GNU_TOOLCHAIN_PATH}` and refer to `.exe` files for
certain tools; on Linux delete the variable reference and the
`.exe` extension.

## Creating a New Project from This Template

1. Copy this project directory to a new location
2. Edit `CMakeLists.txt` — the configuration block at roughly line 24
   describes exactly what to change:
   - `PROJECT_NAME` — your project's name
   - `BLOCS_FILE` — stem name of your `.blocs` system definition file
   - `TARGET` — your executable name; output files will be named accordingly
     (e.g. `blink.elf`, `blink.map`)
   - `EMBLOCS_DIR` — path to the emblocs repository (change if not using
     the standard submodule layout)
   - `PICO_BOARD` — change to `pico_w` if using a Pico W
3. Replace `blink.blocs` with your own system definition file
4. Update `emblocs.json` if your block source files are in a different location
5. Update `main.c` to call your thread functions
6. Update `flash.cfg` — change `ELF_FILE` to match your `TARGET` name
7. If using VS Code, update `.vscode` files as described in the VS Code section
8. Run `cmake -B build -G Ninja` to configure

## Hardware

Target board: Raspberry Pi Pico (standard, not Pico W)
Onboard LED: GP25
