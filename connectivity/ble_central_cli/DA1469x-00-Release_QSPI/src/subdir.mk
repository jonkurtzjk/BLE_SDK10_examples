################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/command_parser.c \
../src/gap_commands.c \
../src/gatt_commands.c \
../src/peripheral_commands.c \
../src/suota_client.c \
../src/uart_driver_custom.c 

OBJS += \
./src/command_parser.o \
./src/gap_commands.o \
./src/gatt_commands.o \
./src/peripheral_commands.o \
./src/suota_client.o \
./src/uart_driver_custom.o 

C_DEPS += \
./src/command_parser.d \
./src/gap_commands.d \
./src/gatt_commands.d \
./src/peripheral_commands.d \
./src/suota_client.d \
./src/uart_driver_custom.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -Ddg_configDEVICE=DEVICE_DA1469x -Ddg_configBLACK_ORCA_IC_REV=BLACK_ORCA_IC_REV_A -Ddg_configBLACK_ORCA_IC_STEP=BLACK_ORCA_IC_STEP_A -DNDEBUG -DRELEASE_BUILD -I"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/ble_central_cli/config" -I"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/ble_central_cli/src" -I"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/ble_central_cli/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/middleware/adapters/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/adapter/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/api/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/services/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/services/src" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/manager/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/stack/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/stack/da14690/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/clients/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/clients/src" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/free_rtos/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/memory/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/middleware/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/middleware/osal" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/peripherals/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/snc/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/snc/src" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/system/sys_man" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/system/sys_man/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/util/include" -include"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/ble_central_cli/config/custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/peripheral_commands.o: ../src/peripheral_commands.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -Ddg_configDEVICE=DEVICE_DA1469x -Ddg_configBLACK_ORCA_IC_REV=BLACK_ORCA_IC_REV_A -Ddg_configBLACK_ORCA_IC_STEP=BLACK_ORCA_IC_STEP_A -DNDEBUG -DRELEASE_BUILD -I"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/ble_central_cli/config" -I"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/ble_central_cli/src" -I"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/ble_central_cli/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/middleware/adapters/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/adapter/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/api/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/services/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/services/src" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/manager/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/stack/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/stack/da14690/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/clients/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/clients/src" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/free_rtos/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/memory/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/middleware/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/middleware/osal" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/peripherals/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/snc/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/snc/src" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/system/sys_man" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/system/sys_man/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/util/include" -include"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/ble_central_cli/config/custom_config_qspi.h" -std=gnu11 -O0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"src/peripheral_commands.d" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/uart_driver_custom.o: ../src/uart_driver_custom.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -Ddg_configDEVICE=DEVICE_DA1469x -Ddg_configBLACK_ORCA_IC_REV=BLACK_ORCA_IC_REV_A -Ddg_configBLACK_ORCA_IC_STEP=BLACK_ORCA_IC_STEP_A -DNDEBUG -DRELEASE_BUILD -I"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/ble_central_cli/config" -I"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/ble_central_cli/src" -I"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/ble_central_cli/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/middleware/adapters/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/adapter/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/api/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/services/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/services/src" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/manager/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/stack/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/stack/da14690/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/clients/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/clients/src" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/free_rtos/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/memory/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/middleware/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/middleware/osal" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/peripherals/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/snc/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/snc/src" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/system/sys_man" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/system/sys_man/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/util/include" -include"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/ble_central_cli/config/custom_config_qspi.h" -std=gnu11 -O0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"src/uart_driver_custom.d" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


