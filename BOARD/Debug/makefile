################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include USB_DEVICE/Target/subdir.mk
-include USB_DEVICE/App/subdir.mk
-include Middlewares/ST/STM32_USB_Device_Library/Core/Src/subdir.mk
-include Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/subdir.mk
-include Drivers/STM32F4xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
Fascin8_v3.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
Fascin8_v3.list \

OBJCOPY_BIN += \
Fascin8_v3.bin \


# All Target
all: Fascin8_v3.elf secondary-outputs

# Tool invocations
Fascin8_v3.elf: $(OBJS) $(USER_OBJS) C:\Users\i8-tech\Desktop\Hardware\Code\Fascin8_v3.8\Fascin8_v3.8\STM32F407VGTX_FLASH.ld
	arm-none-eabi-gcc -o "Fascin8_v3.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"C:\Users\i8-tech\Desktop\Hardware\Code\Fascin8_v3.8\Fascin8_v3.8\STM32F407VGTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="Fascin8_v3.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

Fascin8_v3.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "Fascin8_v3.list"
	@echo 'Finished building: $@'
	@echo ' '

Fascin8_v3.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "Fascin8_v3.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
