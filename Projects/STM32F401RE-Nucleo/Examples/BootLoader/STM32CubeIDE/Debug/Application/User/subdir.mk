################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/MyFolder/01_GitRepoProject/FP-SNS-FLIGHT1/Firmware/Projects/STM32F401RE-Nucleo/Examples/BootLoader/Src/app_bootloader.c \
C:/MyFolder/01_GitRepoProject/FP-SNS-FLIGHT1/Firmware/Projects/STM32F401RE-Nucleo/Examples/BootLoader/Src/main.c \
C:/MyFolder/01_GitRepoProject/FP-SNS-FLIGHT1/Firmware/Projects/STM32F401RE-Nucleo/Examples/BootLoader/Src/stm32f4xx_hal_msp.c \
C:/MyFolder/01_GitRepoProject/FP-SNS-FLIGHT1/Firmware/Projects/STM32F401RE-Nucleo/Examples/BootLoader/Src/stm32f4xx_it.c \
C:/MyFolder/01_GitRepoProject/FP-SNS-FLIGHT1/Firmware/Projects/STM32F401RE-Nucleo/Examples/BootLoader/Src/stm32f4xx_nucleo.c \
../Application/User/syscalls.c \
../Application/User/sysmem.c 

OBJS += \
./Application/User/app_bootloader.o \
./Application/User/main.o \
./Application/User/stm32f4xx_hal_msp.o \
./Application/User/stm32f4xx_it.o \
./Application/User/stm32f4xx_nucleo.o \
./Application/User/syscalls.o \
./Application/User/sysmem.o 

C_DEPS += \
./Application/User/app_bootloader.d \
./Application/User/main.d \
./Application/User/stm32f4xx_hal_msp.d \
./Application/User/stm32f4xx_it.d \
./Application/User/stm32f4xx_nucleo.d \
./Application/User/syscalls.d \
./Application/User/sysmem.d 


# Each subdirectory must supply rules for building sources it contributes
Application/User/app_bootloader.o: C:/MyFolder/01_GitRepoProject/FP-SNS-FLIGHT1/Firmware/Projects/STM32F401RE-Nucleo/Examples/BootLoader/Src/app_bootloader.c Application/User/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xE -c -I../../Inc -I../../../../../../Drivers/STM32F4xx_HAL_Driver/Inc -I../../../../../../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../../../../../../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../../../../../../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Application/User/main.o: C:/MyFolder/01_GitRepoProject/FP-SNS-FLIGHT1/Firmware/Projects/STM32F401RE-Nucleo/Examples/BootLoader/Src/main.c Application/User/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xE -c -I../../Inc -I../../../../../../Drivers/STM32F4xx_HAL_Driver/Inc -I../../../../../../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../../../../../../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../../../../../../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Application/User/stm32f4xx_hal_msp.o: C:/MyFolder/01_GitRepoProject/FP-SNS-FLIGHT1/Firmware/Projects/STM32F401RE-Nucleo/Examples/BootLoader/Src/stm32f4xx_hal_msp.c Application/User/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xE -c -I../../Inc -I../../../../../../Drivers/STM32F4xx_HAL_Driver/Inc -I../../../../../../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../../../../../../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../../../../../../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Application/User/stm32f4xx_it.o: C:/MyFolder/01_GitRepoProject/FP-SNS-FLIGHT1/Firmware/Projects/STM32F401RE-Nucleo/Examples/BootLoader/Src/stm32f4xx_it.c Application/User/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xE -c -I../../Inc -I../../../../../../Drivers/STM32F4xx_HAL_Driver/Inc -I../../../../../../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../../../../../../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../../../../../../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Application/User/stm32f4xx_nucleo.o: C:/MyFolder/01_GitRepoProject/FP-SNS-FLIGHT1/Firmware/Projects/STM32F401RE-Nucleo/Examples/BootLoader/Src/stm32f4xx_nucleo.c Application/User/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xE -c -I../../Inc -I../../../../../../Drivers/STM32F4xx_HAL_Driver/Inc -I../../../../../../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../../../../../../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../../../../../../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Application/User/%.o Application/User/%.su: ../Application/User/%.c Application/User/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xE -c -I../../Inc -I../../../../../../Drivers/STM32F4xx_HAL_Driver/Inc -I../../../../../../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../../../../../../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../../../../../../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Application-2f-User

clean-Application-2f-User:
	-$(RM) ./Application/User/app_bootloader.d ./Application/User/app_bootloader.o ./Application/User/app_bootloader.su ./Application/User/main.d ./Application/User/main.o ./Application/User/main.su ./Application/User/stm32f4xx_hal_msp.d ./Application/User/stm32f4xx_hal_msp.o ./Application/User/stm32f4xx_hal_msp.su ./Application/User/stm32f4xx_it.d ./Application/User/stm32f4xx_it.o ./Application/User/stm32f4xx_it.su ./Application/User/stm32f4xx_nucleo.d ./Application/User/stm32f4xx_nucleo.o ./Application/User/stm32f4xx_nucleo.su ./Application/User/syscalls.d ./Application/User/syscalls.o ./Application/User/syscalls.su ./Application/User/sysmem.d ./Application/User/sysmem.o ./Application/User/sysmem.su

.PHONY: clean-Application-2f-User

