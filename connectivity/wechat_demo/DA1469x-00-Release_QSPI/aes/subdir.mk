################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../aes/crypto_aes.c \
../aes/crypto_hash_op.c 

OBJS += \
./aes/crypto_aes.o \
./aes/crypto_hash_op.o 

C_DEPS += \
./aes/crypto_aes.d \
./aes/crypto_hash_op.d 


# Each subdirectory must supply rules for building sources it contributes
aes/%.o: ../aes/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -Wno-cpp  -g3 -Ddg_configDEVICE=DEVICE_DA1469x -Ddg_configBLACK_ORCA_IC_REV=BLACK_ORCA_IC_REV_A -Ddg_configBLACK_ORCA_IC_STEP=BLACK_ORCA_IC_STEP_A -I"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/wechat_demo/config" -I"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/wechat_demo/aes" -I"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/wechat_demo/products" -I"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/wechat_demo/common" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/middleware/adapters/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/adapter/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/api/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/manager/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/services/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/stack/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/stack/da14690/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/middleware/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/free_rtos/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/free_rtos/portable/GCC/DA1469x" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/memory/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/middleware/osal" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/peripherals/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/snc/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/snc/src" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/system/sys_man" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/system/sys_man/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/util/include" -include"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/wechat_demo/config/custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


