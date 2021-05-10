################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../common/ble_wechat_util.c \
../common/crc32.c \
../common/epb.c \
../common/epb_MmBp.c \
../common/wechats.c 

OBJS += \
./common/ble_wechat_util.o \
./common/crc32.o \
./common/epb.o \
./common/epb_MmBp.o \
./common/wechats.o 

C_DEPS += \
./common/ble_wechat_util.d \
./common/crc32.d \
./common/epb.d \
./common/epb_MmBp.d \
./common/wechats.d 


# Each subdirectory must supply rules for building sources it contributes
common/%.o: ../common/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall -Wno-cpp  -g3 -Ddg_configDEVICE=DEVICE_DA1469x -Ddg_configBLACK_ORCA_IC_REV=BLACK_ORCA_IC_REV_A -Ddg_configBLACK_ORCA_IC_STEP=BLACK_ORCA_IC_STEP_A -I"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/wechat_demo/config" -I"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/wechat_demo/aes" -I"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/wechat_demo/products" -I"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/wechat_demo/common" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/middleware/adapters/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/adapter/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/api/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/manager/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/services/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/stack/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/stack/da14690/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/middleware/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/free_rtos/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/free_rtos/portable/GCC/DA1469x" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/memory/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/middleware/osal" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/peripherals/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/snc/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/snc/src" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/system/sys_man" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/system/sys_man/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/util/include" -include"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/wechat_demo/config/custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


