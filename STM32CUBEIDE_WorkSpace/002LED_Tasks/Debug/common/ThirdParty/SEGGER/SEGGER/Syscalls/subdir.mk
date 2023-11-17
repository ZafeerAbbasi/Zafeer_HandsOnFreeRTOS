################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../common/ThirdParty/SEGGER/SEGGER/Syscalls/SEGGER_RTT_Syscalls_GCC.c 

OBJS += \
./common/ThirdParty/SEGGER/SEGGER/Syscalls/SEGGER_RTT_Syscalls_GCC.o 

C_DEPS += \
./common/ThirdParty/SEGGER/SEGGER/Syscalls/SEGGER_RTT_Syscalls_GCC.d 


# Each subdirectory must supply rules for building sources it contributes
common/ThirdParty/SEGGER/SEGGER/Syscalls/%.o common/ThirdParty/SEGGER/SEGGER/Syscalls/%.su common/ThirdParty/SEGGER/SEGGER/Syscalls/%.cyclo: ../common/ThirdParty/SEGGER/SEGGER/Syscalls/%.c common/ThirdParty/SEGGER/SEGGER/Syscalls/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/zafee/Documents/GitHub/Zafeer_HandsOnFreeRTOS/STM32CUBEIDE_WorkSpace/common/ThirdParty/FreeRTOS/include" -I"C:/Users/zafee/Documents/GitHub/Zafeer_HandsOnFreeRTOS/STM32CUBEIDE_WorkSpace/common/ThirdParty/FreeRTOS/portable/GCC/ARM_CM4F" -I"C:/Users/zafee/Documents/GitHub/Zafeer_HandsOnFreeRTOS/STM32CUBEIDE_WorkSpace/common/ThirdParty/SEGGER/Config" -I"C:/Users/zafee/Documents/GitHub/Zafeer_HandsOnFreeRTOS/STM32CUBEIDE_WorkSpace/common/ThirdParty/SEGGER/OS" -I"C:/Users/zafee/Documents/GitHub/Zafeer_HandsOnFreeRTOS/STM32CUBEIDE_WorkSpace/common/ThirdParty/SEGGER/SEGGER" -I"C:/Users/zafee/Documents/GitHub/Zafeer_HandsOnFreeRTOS/STM32CUBEIDE_WorkSpace/002LED_Tasks/common/ThirdParty/FreeRTOS" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-common-2f-ThirdParty-2f-SEGGER-2f-SEGGER-2f-Syscalls

clean-common-2f-ThirdParty-2f-SEGGER-2f-SEGGER-2f-Syscalls:
	-$(RM) ./common/ThirdParty/SEGGER/SEGGER/Syscalls/SEGGER_RTT_Syscalls_GCC.cyclo ./common/ThirdParty/SEGGER/SEGGER/Syscalls/SEGGER_RTT_Syscalls_GCC.d ./common/ThirdParty/SEGGER/SEGGER/Syscalls/SEGGER_RTT_Syscalls_GCC.o ./common/ThirdParty/SEGGER/SEGGER/Syscalls/SEGGER_RTT_Syscalls_GCC.su

.PHONY: clean-common-2f-ThirdParty-2f-SEGGER-2f-SEGGER-2f-Syscalls

