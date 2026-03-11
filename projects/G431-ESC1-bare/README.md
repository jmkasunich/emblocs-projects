# G431-ESC1-Bare

This is a sample project for the STMicro [B-G431B-ESC1 Discovery Kit](https://www.st.com/en/evaluation-tools/b-g431b-esc1.html), which is a three-phase brushless motor driver development kit containing an [STM32F431CBU6](https://www.st.com/en/microcontrollers-microprocessors/stm32g431cb.html) microcontroller, a gate driver, and a three-phase MOSFET bridge.  The board also contains a detachable section with a USB connector and an embedded STLink V2 debugging interface, which has SWD and UART interfaces to the main MCU.

The microcontroller has a 170MHz Cortex M4 core, 128K of flash, and 32K of RAM, in a 48-pin UFQFPN package.

Data from STMicro includes:
* [Datasheet](https://www.st.com/resource/en/datasheet/stm32g431cb.pdf)
* [Reference Manual](https://www.st.com/resource/en/reference_manual/rm0440-stm32g4-series-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)
* [Programming Manual](https://www.st.com/resource/en/programming_manual/pm0214-stm32-cortexm4-mcus-and-mpus-programming-manual-stmicroelectronics.pdf)

STMicro does not publish a schematic for the board.  Digikey has some links on their [product page](https://www.digikey.at/reference-designs/en/motor-control/motor-control/2840) for the board, but the schematic link to STMicro is dead.  Fortunately the same Digikey page has PNG files for the individual schematic pages.  It's not clear what version of the board the schematics are for; it looks like STMicro has made three versions.

This project is for a bare-metal implementation, using a Makefile for the build system.

The makefile provides two extra targets that take advantage of the ESC1 on-board STLink:
* 'make flash' uses OpenOCD to connect to the board and flash the .hex file to the device, then resets the device to run the code.
* 'make debug' uses OpenOCD as a debug server and invokes GDB.

