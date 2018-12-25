################################################################################
# Automatically-generated file. Do not edit!
################################################################################

CG_TOOL_ROOT := /Applications/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.4.LTS

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./main.obj" \
"./startup_msp432p401r_ccs.obj" \
"./system_msp432p401r.obj" \
"./touch_P401R.obj" \
"./Hardware/CS_Driver.obj" \
"./Hardware/GPIO_Driver.obj" \
"./Hardware/SPI_Driver.obj" \
"./Hardware/TIMERA_Driver.obj" \
"./LcdDriver/HAL_MSP_EXP432P401R_KITRONIX320X240_SSD2119_SPI.obj" \
"./LcdDriver/kitronix320x240x16_ssd2119_spi.obj" \
"./fatfs/diskio.obj" \
"./fatfs/ff.obj" \
"./fatfs/ffsystem.obj" \
"./fatfs/ffunicode.obj" \
"./fatfs/mmc_MSP432P401r.obj" \
"./images/Images_Button.obj" \
"./images/Primitives_Button.obj" \
"./images/TI_logo_150x150.obj" \
"./images/TI_platform_bar_red.obj" \
"./images/games.obj" \
"./images/group.obj" \
"./images/lcd_color_320x240.obj" \
"./images/mario.obj" \
"./images/street_fighter.obj" \
"./images/zelda.obj" \
"../msp432p401r.cmd" \
$(GEN_CMDS__FLAG) \
-l"ti/display/lib/display.aem4f" \
-l"ti/grlib/lib/ccs/m4f/grlib.a" \
-l"third_party/spiffs/lib/ccs/m4f/spiffs.a" \
-l"ti/drivers/lib/drivers_msp432p401x.aem4f" \
-l"third_party/fatfs/lib/ccs/m4f/fatfs.a" \
-l"ti/devices/msp432p4xx/driverlib/ccs/msp432p4xx_driverlib.lib" \
-llibc.a \

-include ../makefile.init

RM := rm -rf
RMDIR := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include Hardware/subdir_vars.mk
-include LcdDriver/subdir_vars.mk
-include fatfs/subdir_vars.mk
-include images/subdir_vars.mk
-include subdir_rules.mk
-include Hardware/subdir_rules.mk
-include LcdDriver/subdir_rules.mk
-include fatfs/subdir_rules.mk
-include images/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
LCD_Display.out \

EXE_OUTPUTS__QUOTED += \
"LCD_Display.out" \

BIN_OUTPUTS += \
LCD_Display.hex \

BIN_OUTPUTS__QUOTED += \
"LCD_Display.hex" \


# All Target
all: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "LCD_Display.out"

# Tool invocations
LCD_Display.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: ARM Linker'
	"/Applications/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.4.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --advice:power=none --define=__MSP432P401R__ --define=DeviceFamily_MSP432P401x -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on -z -m"LCD_Display.map" --stack_size=512 -i"/Applications/ti/simplelink_msp432p4_sdk_2_30_00_14/source" -i"/Applications/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.4.LTS/lib" --diag_wrap=off --display_error_number --warn_sections --xml_link_info="LCD_Display_linkInfo.xml" --rom_model -o "LCD_Display.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '

LCD_Display.hex: $(EXE_OUTPUTS)
	@echo 'Building files: $(strip $(EXE_OUTPUTS__QUOTED))'
	@echo 'Invoking: ARM Hex Utility'
	"/Applications/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.4.LTS/bin/armhex"  -o "LCD_Display.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building: $(strip $(EXE_OUTPUTS__QUOTED))'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(BIN_OUTPUTS__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "main.obj" "startup_msp432p401r_ccs.obj" "system_msp432p401r.obj" "touch_P401R.obj" "Hardware/CS_Driver.obj" "Hardware/GPIO_Driver.obj" "Hardware/SPI_Driver.obj" "Hardware/TIMERA_Driver.obj" "LcdDriver/HAL_MSP_EXP432P401R_KITRONIX320X240_SSD2119_SPI.obj" "LcdDriver/kitronix320x240x16_ssd2119_spi.obj" "fatfs/diskio.obj" "fatfs/ff.obj" "fatfs/ffsystem.obj" "fatfs/ffunicode.obj" "fatfs/mmc_MSP432P401r.obj" "images/Images_Button.obj" "images/Primitives_Button.obj" "images/TI_logo_150x150.obj" "images/TI_platform_bar_red.obj" "images/games.obj" "images/group.obj" "images/lcd_color_320x240.obj" "images/mario.obj" "images/street_fighter.obj" "images/zelda.obj" 
	-$(RM) "main.d" "startup_msp432p401r_ccs.d" "system_msp432p401r.d" "touch_P401R.d" "Hardware/CS_Driver.d" "Hardware/GPIO_Driver.d" "Hardware/SPI_Driver.d" "Hardware/TIMERA_Driver.d" "LcdDriver/HAL_MSP_EXP432P401R_KITRONIX320X240_SSD2119_SPI.d" "LcdDriver/kitronix320x240x16_ssd2119_spi.d" "fatfs/diskio.d" "fatfs/ff.d" "fatfs/ffsystem.d" "fatfs/ffunicode.d" "fatfs/mmc_MSP432P401r.d" "images/Images_Button.d" "images/Primitives_Button.d" "images/TI_logo_150x150.d" "images/TI_platform_bar_red.d" "images/games.d" "images/group.d" "images/lcd_color_320x240.d" "images/mario.d" "images/street_fighter.d" "images/zelda.d" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

