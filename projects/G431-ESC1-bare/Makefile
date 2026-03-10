# project directory is where the Makefile lives
PROJ_DIR := .

# source search always starts in the project directory
SRC_PATH := .

# Disable built-in rules and variables
MAKEFLAGS += --no-builtin-rules
MAKEFLAGS += --no-builtin-variables
.SUFFIXES :

# Avoid excess printing
.SILENT :

# local.mk contains host specific paths, etc
# it should set the following variables:
# BINUTILS_ROOT - path to toolchain
# OPENOCD_ROOT - path to OpenOCD
# PACKAGE_ROOT - path to chip-specific packages

include ../local.mk

# append some subdirs to the source search path
EMBLOCS_DIR := ../../submodules/emblocs/src/
#SRC_PATH += $(EMBLOCS_DIR)emblocs
#SRC_PATH += $(EMBLOCS_DIR)components
#SRC_PATH += $(EMBLOCS_DIR)misc

# tell make to use the path only for .c and .s files
SET_VPATH := $(eval vpath %.c $(SRC_PATH))
SET_VPATH := $(eval vpath %.s $(SRC_PATH))



# ---------------------------------------------------------------------
# Toolchain Configuration
# ---------------------------------------------------------------------
TOOLCHAIN		:= arm-none-eabi
CC				:= $(BINUTILS_ROOT)/bin/$(TOOLCHAIN)-gcc
AS				:= $(BINUTILS_ROOT)/bin/$(TOOLCHAIN)-as
OBJCOPY			:= $(BINUTILS_ROOT)/bin/$(TOOLCHAIN)-objcopy
SIZE			:= $(BINUTILS_ROOT)/bin/$(TOOLCHAIN)-size
GDB				:= $(BINUTILS_ROOT)/bin/$(TOOLCHAIN)-gdb
C_STANDARD		:= -std=gnu11

# ---------------------------------------------------------------------
# Directories - when make is invoked in a project directory, all build
# products are stored in a subdirectory of that project, even if the 
# source files come from elsewhere in the tree.  This allows each
# project to re-build common code with project-specific configuration
# and compiler flags.
# ---------------------------------------------------------------------

BUILD_DIR		:= ./build
OBJECT_DIR		:= $(BUILD_DIR)/obj
DEPS_DIR		:= $(BUILD_DIR)/dep
TEMP_DIR		:= $(BUILD_DIR)/tmp
DIRS			:= $(BUILD_DIR) $(OBJECT_DIR) $(DEPS_DIR) $(TEMP_DIR)

# define object file groups that projects might want to use

EMBLOCS_OBJS := emblocs_core.o emblocs_parse.o emblocs_show.o \
			    linked_list.o str_to_xx.o serial.o

COMP_OBJS := mux2.o sum2.o perftimer.o tmp_gpio.o watch.o scope.o


######################################################

# info for this project

OBJECTS := main.o \
           startup_stm32g431kbtx.o \
		   system_stm32g4xx.o \
		   platform.o
#		   printing.o \
		   $(EMBLOCS_OBJS) \
		   $(COMP_OBJS)

# objects aren't built here; prepend the object directory
OBJECTS := $(addprefix $(OBJECT_DIR)/,$(OBJECTS))

TARGET := main.hex

MCU_PKG := $(PACKAGE_ROOT)/STM32Cube_FW_G4_V1.6.0

INC_DIRS 				:= $(SRC_PATH:%/=%)
INC_DIRS				+= $(MCU_PKG)/Drivers/CMSIS/Include
INC_DIRS				+= $(MCU_PKG)/Drivers/CMSIS/Device/ST/STM32G4xx/Include
INC_DIRS				+= $(MCU_PKG)/Drivers/STM32G4xx_HAL_Driver/Inc

# -----------------------------------------------------------------------------------------------------------------
# Defined Symbols
# -----------------------------------------------------------------------------------------------------------------
#DEFS 					:= -DSTM32F43xx -DUSE_HAL_DRIVER -DARM_MATH_CM4 -D__FPU_PRESENT=1U -DHSE_VALUE=25000000
DEFS 					:= -DSTM32G431xx -DARM_MATH_CM4 -D__FPU_PRESENT=1U -DHSE_VALUE=8000000

# ---------------------------------------------------------------------------------------------------------------------------------------
# Compiler & Linker Flags
# ---------------------------------------------------------------------------------------------------------------------------------------
# Flags sent to all tools in the Toolchain
TOOLCHAIN_SETTINGS 		:= -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16
TOOLCHAIN_SETTINGS 		+= -fmessage-length=0 -ffunction-sections -fdata-sections

LDSCRIPTS				:= -TSTM32G431KBTX_FLASH.ld

# -------------------------------------------------------------
# Debug info
# -------------------------------------------------------------

DEFS 		+= -DDEBUG
CFLAGS		+= -ggdb -g1 -Og
LDFLAGS		+= --specs=rdimon.specs -Og
#LDFLAGS		+= --specs=nosys.specs -Og
#LDFLAGS		+= -nolibc -nostartfiles -Og

#########################################################################
# common make recipes, etc

# C Compiler -- main args 
CFLAGS 			+= $(TOOLCHAIN_SETTINGS) $(DEFS)
# C Compiler -- include paths
CFLAGS			+= $(addprefix -I, $(INC_DIRS))
# C compiler -- save preprocessor and assembler intermediate files
CFLAGS			+= -save-temps -dumpdir $(TEMP_DIR)/
# C Compiler -- warnings
CFLAGS			+= -Wall
CFLAGS			+= -Wextra
CFLAGS			+= -Wfatal-errors
CFLAGS			+= -Wpacked
CFLAGS			+= -Winline
CFLAGS			+= -Wfloat-equal
CFLAGS			+= -Wconversion
CFLAGS			+= -Wlogical-op
CFLAGS			+= -Wpointer-arith
CFLAGS			+= -Wdisabled-optimization
CFLAGS			+= -Wunused-parameter
CFLAGS			+= -Wno-sign-conversion
CFLAGS			+= -Wstrict-aliasing=1
# Assembler settings
CFLAGS			+= -Wa,-alh=$(@:.o=.lst)

# Linker
LDFLAGS			+= $(TOOLCHAIN_SETTINGS) $(DEFS) -Xlinker --gc-sections
LDFLAGS			+= --specs=nano.specs

# rules and recipes for common targets

# master rule
all : $(BUILD_DIR)/$(TARGET)

flash : $(BUILD_DIR)/$(TARGET)
	@echo 'Flashing: $<'
	$(OPENOCD_ROOT)/bin/openocd -d0 -f ./openocd.cfg -c "program $(BUILD_DIR)/$(TARGET) preverify verify reset exit" 
	@echo ' '
	@echo 'Finished flashing: $<'
	@echo ' '


.PHONY : all flash

# generate hex file
$(BUILD_DIR)/%.hex : $(BUILD_DIR)/%.elf
	@echo ' '
	@echo 'Generating: $@'
	@echo 'Invoking: Cross ARM GNU Create Flash Image'
	$(OBJCOPY) -O ihex $< $(@) 
	@echo 'Invoking: Cross ARM GNU Print Size'
	$(SIZE) --format=berkeley $<
	@echo 'Finished building: $@'
	@echo ' '


# link the target
$(BUILD_DIR)/$(TARGET:.hex=.elf) : $(OBJECTS)
	@echo ' '
	@echo 'Linking $(@)'
	@echo 'Invoking: Cross ARM C Linker'
	$(CC) \
		-Xlinker -Map=$(patsubst %.elf,%.map,$(@)) \
		$(LDFLAGS) \
		$(LDSCRIPTS) \
		-o $(@) $(OBJECTS)
	@echo 'Finished linking: $@'


# compile .c files (with dependency tracking)
$(OBJECT_DIR)/%.o : %.c | $(DIRS)
	@echo Compiling $(<)
	$(CC) $(C_STANDARD) $(CFLAGS) -c -MMD -MP -MT $(@) -MF $(DEPS_DIR)/$(*F).d $< -o $(@)
	$(SIZE) --format=berkeley $@


# assemble .s files
$(OBJECT_DIR)/%.o : %.s | $(DIRS)
	@echo Assembling $(<)
	$(AS) $(ASFLAGS) $< -o $(@)
	$(SIZE) --format=berkeley $@


# create build directories
$(DIRS) : 
	@echo Creating $(@)
	mkdir -p $(@)

# clean up build directories
clean:
	@echo "Cleaning"
	rm -rf $(BUILD_DIR)/*.elf
	rm -rf $(BUILD_DIR)/*.hex
	rm -rf $(BUILD_DIR)/*.map
	rm -rf $(OBJECT_DIR)/*.o
	rm -rf $(OBJECT_DIR)/*.lst
	rm -rf $(DEPS_DIR)/*.d
	rm -rf $(TEMP_DIR)/*.i
	rm -rf $(TEMP_DIR)/*.s

.PHONY : clean

# include auto dependencies
-include $(DEPS_DIR)/*.d
