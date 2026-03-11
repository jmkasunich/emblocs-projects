# Build Systems

Each sample project has its own build system and sample code.  There is a fair amount of duplication in both the sample code and the build systems, but this is a tradeoff that I've chosen on purpose.  The goal is that any individual sample can be copied and used without dependencies such as makefiles that are included from some other shared directory.

All build systems are designed to place generated files in a {project}/build folder or folder tree, rather than intermixing generated files with source code.  The build systems build both project specific code and the emblocs library from source, so that all files are built for the same specific architecture and with the same compile flags.  This allows some target-specific functions to be either implemented as macros or inlined, even inside library code.  There is no binary library; each individual library source file becomes an object file, and all of the individual object files are linked; the linker discards any unused code.

I'm developing this project mostly on Windows 11, but I hope it will be relatively portable.  The Makefiles are written using GNU/Linux conventions, and rely on PowerShell to provide Linux-like commands such as rm and ls.

On Linux systems, things tend to be installed in standard locations; that's not always true on Windows.  I'm using environment variables to pass paths to things that might be in different places on different machines.  These variables are used by both the build system (Makefiles, etc) and by VS Code configuration files.  The current list of such variables is:
* ARM_GNU_TOOLCHAIN_PATH - installed [arm-none-eabi gcc compiler](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads) and related tools (including GDB)
* OPENOCD_PATH - installed OPENOCD debug server
* STM32G4_PATH - STM32G4xx support package (downloaded from [ST Micro](https://www.st.com/en/embedded-software/stm32cubeg4.html))

There is a VS Code configuration for each sample project to support Intellisense and build/debug, however VS Code is entirely optional. The intent is that you can use whatever editor you want and invoke the build system from a command line.  Note that VS Code configurations are not located in the individual project directories; that is a VS Code limitation.  All VS Code config data is in {toplevel}/.vscode/c_cpp_properties.json and {toplevel}/.vscode/launch.json.

