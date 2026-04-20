# EMBLOCS Architecture

## Overview

EMBLOCS is a real-time control system framework targeted at microcontrollers
(MCUs). It is inspired by block diagram tools used in control system design,
and uses a hardware analogy throughout: blocks are like integrated circuits,
pins are like IC pins, and signals are like the nets or wires on a schematic.

EMBLOCS is intended to be a code-only library without an embedded build system.
It runs on bare-metal MCUs and MCUs running an RTOS. It is not intended for
larger systems such as PCs or embedded Linux.

Currently supported targets:
- STM32G431 (ARM Cortex-M4), built with GNU make
- Raspberry Pi Pico / RP2040 (ARM Cortex-M0+), built with CMake and the Pico SDK

Planned targets:
- STM32F103 ("blue pill")
- FreeRTOS variants of the above

## Core Abstractions

### Blocks
A block is a reusable functional unit, analogous to an integrated circuit.
Examples include PID controllers, summers, integrators, filters, ADC inputs,
and PWM outputs. Blocks are the unit of reuse in EMBLOCS.

Each block:
- Exports one or more **pins** for inputs and outputs
- Exports one or more **functions** that implement its behavior
- Depends only on the core library, not on other blocks

Users may write custom blocks for specific projects. A custom block can often
replace a complex arrangement of simpler generic blocks with less overhead.

### Pins
Pins are typed input/output ports on a block, analogous to pins on an
integrated circuit. Pin types include: bool, u32, s32, and float.

### Signals
Signals are named values that connect pins, analogous to nets or wires on
a schematic. Signals form a directed graph connecting block outputs to block
inputs. A signal may be connected to multiple pins.

### Threads
Threads are periodic execution contexts. Each thread has a period (in
nanoseconds) and an ordered list of block functions to call. On bare-metal
systems, threads run in ISRs. On RTOS systems, threads run as OS threads.

## Source Structure

### emblocs/src/emblocs/
The core library. Key files:

| File | Role | Visibility |
|------|------|------------|
| emblocs_config.h | Project-specific configuration | Public - must be copied to project and edited |
| emblocs_common.h | Shared data structures | Public - included by api and comp headers |
| emblocs_api.h | Core API declarations | Public - used by applications and blocks |
| emblocs_comp.h | Block-authoring API | Semi-public - used when writing blocks |
| emblocs_priv.h | Internal data structures | Private - only included by .c files |
| emblocs_core.c | Engine implementation | Compiled |
| emblocs_show.c | Display/inspection functions | Compiled, optionally stripped in production |
| emblocs_parse.c | Language parser/interpreter | Compiled, optionally stripped in production |

Include hierarchy:

    emblocs_config.h
        ^
    emblocs_common.h
        ^           ^
    emblocs_api.h   emblocs_comp.h
             ^      ^
              emblocs_priv.h
                   ^
        emblocs_core.c  emblocs_show.c  emblocs_parse.c

emblocs_config.h is project-supplied and controls:
- Memory pool sizes
- Whether disconnect/remove operations are enabled
- Whether null pointer checks are compiled in
- Whether the parser and show modules are included

### emblocs/src/components/
Generic block implementations. Each block depends only on the core library.
Blocks are designed to be independently testable via CI test harnesses.

### emblocs/src/misc/
Utility code used by the core library:
- Linked list management
- Lightweight printf replacement
- Lightweight string parsing (string to float, string to int, etc.)
- Serial port library implementing a combined human-readable stream and
  binary packet protocol

### emblocs/python/
Build-time and runtime Python tools (see Python Tooling section below).

## The System Definition Language

EMBLOCS defines a scripting language for describing systems. Files use the
extensions .bloc (single block definition) and .blocs (system definition).

### Compile-time commands (the "use" section)
These commands drive code generation and linking. All blocks needed by the
system must appear here.

    use <basename>

Use a block defined by basename.bloc. Generate basename.c, compile it,
and link it into the executable.

    use <basename> variant <variantname> <param>=<value> ...

Use a parameterized variant of a block. Parameters such as number of channels
or inputs are used to generate variantname.c from basename.bloc.

Example:

    use mux variant mux_3ch_2to1 nchannels=3 inputs=2

### Runtime-flexible commands
These commands can be processed at compile time, startup time, or runtime
depending on the deployment mode.

    block <blockname> <basename|variantname>

Create an instance of a block type and give it a name.

    signal <signame> <type> [=<value>] [+<blockname>.<pinname>] ...

Create a signal of the given type (bool, u32, s32, float), optionally assign
a value, and optionally connect it to one or more pins.

    thread <threadname> <periodns> [+<blockname>.<functname>] ...

Create a thread with the given period in nanoseconds, and optionally add
block functions to it in execution order.

### Modification commands
These commands modify existing objects and can be used at any phase:

    <signalname> [=<value>] [+<blockname>.<pinname>] [-<blockname>.<pinname>]

Set a signal value (=), connect to additional pins (+), or disconnect from
pins (-).

    <threadname> [+<blockname>.<functname>] [-<blockname>.<functname>]

Add (+) or remove (-) functions from a thread.

    <blockname>.<pinname> [=<value>] [+<signalname>] [-]

Set an unconnected pin value (=), connect a pin to a signal (+), or
disconnect a pin (-).

## Deployment Modes

EMBLOCS supports a spectrum of deployment configurations controlled by
compile-time options in emblocs_config.h:

### Dynamic (development)
- All modules compiled in, including parser and metadata (names)
- Executable contains a parser and full name tables
- User configures the system at runtime by sending .blocs commands over UART
- Most flexible, largest footprint
- Use case: early development, exploration

### Static with metadata (integration/debug)
- System pre-configured at startup via generated system.c
- Parser and name metadata still present
- Runtime monitor can inspect and modify the running system by name
- Use case: integration testing, debug builds

### Static, no metadata (production)
- System pre-configured at startup via generated system.c
- Name strings stripped out to minimize flash/RAM usage
- Functional behavior identical to debug build
- Runtime monitor has limited or no visibility
- Use case: production firmware

The same .blocs file produces all deployment modes; only the build
configuration changes.

## Block Variants

Some blocks have parameterized variants. For example, a multiplexor block
might exist as a 2-to-1, 3-to-1, or N-channel variant. Rather than
maintaining separate source files for each variant, a single .bloc file
defines the block template, and the build system generates the variant-specific
.c file using parameters supplied in the use command.

Example - a 3-channel 2-to-1 multiplexor:
- Pins: ch0_in0, ch0_in1, ch1_in0, ch1_in1, ch2_in0, ch2_in1,
  ch0_out, ch1_out, ch2_out, select
- When select=1: all chN_in1 are copied to chN_out

## Python Tooling

### Runtime Monitor
A GUI tool that connects to a running EMBLOCS system over UART. Features:
- Inspector: list signals, show signal connections, show thread contents
- Meters: display signal values numerically at update rates limited by
  UART and GUI performance
- Oscilloscope: display signal values vs. time, capturing one sample per
  thread execution

### Build-time Tools
- Block compiler: generates .c files from .bloc template files,
  applying variant parameters
- System compiler: processes .blocs system definition files, driving
  code generation, compilation, and linking

## Build System Philosophy

- All source is compiled per-project into a project-specific build directory
- No pre-compiled libraries; all files are compiled with project-specific flags
- This allows target-specific optimizations (inlining, macros) even in library code
- The linker discards unused code
- emblocs_config.h is project-supplied and never part of the library itself
- The build system is CMake for Pico targets, GNU make for STM32 targets
  (with possible future unification under CMake)
- Only open-source toolchains are supported (arm-none-eabi-gcc, etc.)
- Proprietary compilers are explicitly not supported

## Portability

EMBLOCS targets MCUs only - not PCs or embedded Linux. Supported and planned
compiler toolchains:

| Toolchain | Targets |
|-----------|---------|
| arm-none-eabi-gcc | STM32 (all variants), RP2040, Teensy 4.1 |
| xtensa-esp32-elf-gcc | ESP32 |
| riscv32-esp-elf-gcc | ESP32-C3/C6 |

Platform-specific code (GPIO, UART, timers) uses the platform's native SDK
directly. EMBLOCS does not define its own hardware abstraction layer - that
is the responsibility of the platform SDK. EMBLOCS abstracts the control
system layer (blocks, signals, threads), not the hardware layer.
