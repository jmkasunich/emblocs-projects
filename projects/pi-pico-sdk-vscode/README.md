# Pi-Pico-SDK-VSCode

A sample project for the [Raspberry Pi Pico](https://www.raspberrypi.com/documentation/microcontrollers/pico-series.html#pico1)
 board, using the [RP2040](https://www.raspberrypi.com/documentation/microcontrollers/microcontroller-chips.html#rp2040)
 MCU.
The RP2040 provides two 125MHz Cortex-M0+ cores, 264KB of RAM,
and no on-chip flash. The Pico board integrates a 2MB QSPI flash chip.

This project demonstrates EMBLOCS integration using the Raspberry Pi
Pico VS Code extension's standard workflow. It is intended for users
who are already familiar with the extension and prefer its
build/flash/debug buttons over the command line.

## Requirements

Install [Visual Studio Code](https://code.visualstudio.com/) and the
[Raspberry Pi Pico extension](https://marketplace.visualstudio.com/items?itemName=raspberry-pi.raspberry-pi-pico).
The extension will automatically download and install the Pico SDK, compiler,
cmake, ninja, and other required tools when you first open the project.

No additional environment variables or PATH modifications are required for the
VS Code workflow. The extension manages all tool paths automatically.

## Opening the Project

Open this project folder directly in VS Code:
**File → Open Folder** → navigate to `projects/pi-pico-sdk-vscode`

When prompted "Do you want to import this project as a Raspberry Pi Pico
project?", click **No** — the project is already configured.

## Building

Use the Raspberry Pi Pico extension's Quick Access panel in the Activity Bar:

1. **Configure CMake** — run once after cloning or when `CMakeLists.txt` changes
2. **Compile Project** — builds the project

EMBLOCS's `blocs_compiler.py` runs automatically during the configure step,
so there is no separate tool invocation needed.

## Flashing

Connect a [Raspberry Pi Debug Probe](https://www.raspberrypi.com/products/debug-probe/)
via SWD to the Pico, then use:

- **Flash Project (SWD)** in the Quick Access panel, or
- **F5** to flash and start a debug session

## VS Code Files

Files in the `.vscode` directory:

- `c_cpp_properties.json` — include paths, compiler, and C standard. When
  creating a new project, change the `name` field to match your project.
- `cmake-kits.json` — compiler kit configuration for CMake Tools. Generated
  by the Pico extension; do not edit manually.
- `extensions.json` — recommended VS Code extensions. Can be copied unmodified
  when starting a new project.
- `launch.json` — debug configurations for F5 debugging. When creating a new
  project, update the `name` fields and the `executable` path to match the
  `TARGET` variable in `CMakeLists.txt`.
- `settings.json` — project-level settings enabling CMake Tools integration
  with the Pico extension. Can be copied unmodified when starting a new project.
- `tasks.json` — compile, flash, and reset tasks. When creating a new project,
  update references to the `.elf` file to match the `TARGET` variable in
  `CMakeLists.txt`.

## Adding EMBLOCS to a New Project

To add EMBLOCS to a project created by the Pico extension wizard:

1. Add emblocs as a git submodule (or copy the emblocs source tree)
2. Create a `.blocs` system definition file and `emblocs.json` config file
3. Add the following to `CMakeLists.txt` after `add_executable()`:

```cmake
# ---- EMBLOCS configuration ----
set(TARGET ${CMAKE_PROJECT_NAME})
set(BLOCS_FILE your_blocs_file_stem)
set(EMBLOCS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../submodules/emblocs)
include(${EMBLOCS_DIR}/cmake/emblocs.cmake)
# ---- end EMBLOCS configuration ----
```

4. In `main.c`, include the generated system header and call the thread functions:

```c
#include "your_blocs_file_stem.h"

int main() {
    stdio_init_all();
    your_blocs_file_stem_init(1);
    while (true) {
        your_blocs_file_stem_threadname(period_ns);
        sleep_ms(period_ms);
    }
}
```

## Hardware

Target board: Raspberry Pi Pico (standard, not Pico W)
Onboard LED: GP25