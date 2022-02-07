################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/Driver_SLCD.c" \
"../Sources/Fonts_LCD.c" \
"../Sources/main.c" \

C_SRCS += \
../Sources/Driver_SLCD.c \
../Sources/Fonts_LCD.c \
../Sources/main.c \

OBJS += \
./Sources/Driver_SLCD.o \
./Sources/Fonts_LCD.o \
./Sources/main.o \

C_DEPS += \
./Sources/Driver_SLCD.d \
./Sources/Fonts_LCD.d \
./Sources/main.d \

OBJS_QUOTED += \
"./Sources/Driver_SLCD.o" \
"./Sources/Fonts_LCD.o" \
"./Sources/main.o" \

C_DEPS_QUOTED += \
"./Sources/Driver_SLCD.d" \
"./Sources/Fonts_LCD.d" \
"./Sources/main.d" \

OBJS_OS_FORMAT += \
./Sources/Driver_SLCD.o \
./Sources/Fonts_LCD.o \
./Sources/main.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/Driver_SLCD.o: ../Sources/Driver_SLCD.c
	@echo 'Building file: $<'
	@echo 'Executing target #1 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/Driver_SLCD.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/Driver_SLCD.o"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Fonts_LCD.o: ../Sources/Fonts_LCD.c
	@echo 'Building file: $<'
	@echo 'Executing target #2 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/Fonts_LCD.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/Fonts_LCD.o"
	@echo 'Finished building: $<'
	@echo ' '

Sources/main.o: ../Sources/main.c
	@echo 'Building file: $<'
	@echo 'Executing target #3 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/main.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/main.o"
	@echo 'Finished building: $<'
	@echo ' '


