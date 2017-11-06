#This file is generated by VisualGDB.
#It contains GCC settings automatically derived from the board support package (BSP).
#DO NOT EDIT MANUALLY. THE FILE WILL BE OVERWRITTEN. 
#Use VisualGDB Project Properties dialog or modify Makefile or per-configuration .mak files instead.

#VisualGDB provides BSP_ROOT and TOOLCHAIN_ROOT via environment when running Make. The line below will only be active if GNU Make is started manually.
BSP_ROOT ?= C:/SysGCC/arm-eabi
EFP_BASE ?= $(LOCALAPPDATA)/VisualGDB/EmbeddedEFPs
TESTFW_BASE ?= $(LOCALAPPDATA)/VisualGDB/TestFrameworks
TOOLCHAIN_ROOT ?= C:/SysGCC/arm-eabi
#Embedded toolchain
CC := $(TOOLCHAIN_ROOT)/bin/arm-eabi-gcc.exe
CXX := $(TOOLCHAIN_ROOT)/bin/arm-eabi-g++.exe
LD := $(CXX)
AR := $(TOOLCHAIN_ROOT)/bin/arm-eabi-ar.exe
OBJCOPY := $(TOOLCHAIN_ROOT)/bin/arm-eabi-objcopy.exe

#Additional flags
PREPROCESSOR_MACROS += ARM_MATH_CM4 STM32F429IG flash_layout STM32F429xx OS_SUPPORT USE_HAL_DRIVER USE_FreeRTOS=1
INCLUDE_DIRS += . ../../../src/app ../../../src/cli ../../../src/drivers/hal/CMSIS_RTOS ../../../src/drivers/hal/CMSIS/Include ../../../src/drivers/hal/CMSIS/Device/ST/STM32F4xx/Include ../../../src/drivers/hal/STM32F4xx_HAL_Driver/Inc ../../../src/drivers/bsp/inc ../../../src/drivers/device/inc ../../../src/os ../../../src/os/FreeRTOS/Source/include ../../../src/os/FreeRTOS/Source/portable/GCC/ARM_CM4F ../../../src/middlewares/tinysh ../../../src/middlewares/STemWin/inc ../../../src/middlewares/STemWin/Config ../../../src/middlewares/STemWin/Demo ../../../src/middlewares/ff13a/source ../../../src/middlewares/userlib/inc ../../../src/middlewares/cJSON ../../../src/middlewares/gdsl-1.8/src ../../../src/middlewares/gdsl-1.8/src/examples ../../../src/middlewares/STemWin/BMP ../../../src/middlewares/STemWin/XBF ../../../src/middlewares/tiny-AES128-C ../../../src/hmi ../../../src/hmi/lcd ../../../src/net/mbedTLS/include ../../../src/hmi/qrencode/inc ../../../src/middlewares/STemWin/SIF
LIBRARY_DIRS += 
LIBRARY_NAMES += compactcpp
ADDITIONAL_LINKER_INPUTS += 
MACOS_FRAMEWORKS += 
LINUX_PACKAGES += 

CFLAGS += 
CXXFLAGS += 
ASFLAGS += -mfpu=fpv4-sp-d16
LDFLAGS += --specs=nano.specs --specs=nosys.specs
COMMONFLAGS += -mcpu=cortex-m4 -mthumb -mfloat-abi=hard
LINKER_SCRIPT := STM32F429IG_flash.lds

