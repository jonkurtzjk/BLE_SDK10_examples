################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/api/src/ble_attribdb.c \
/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/api/src/ble_common.c \
/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/api/src/ble_gap.c \
/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/api/src/ble_gattc.c \
/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/api/src/ble_gattc_util.c \
/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/api/src/ble_gatts.c \
/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/api/src/ble_l2cap.c \
/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/api/src/ble_storage.c \
/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/api/src/ble_uuid.c 

OBJS += \
./sdk/ble/api/src/ble_attribdb.o \
./sdk/ble/api/src/ble_common.o \
./sdk/ble/api/src/ble_gap.o \
./sdk/ble/api/src/ble_gattc.o \
./sdk/ble/api/src/ble_gattc_util.o \
./sdk/ble/api/src/ble_gatts.o \
./sdk/ble/api/src/ble_l2cap.o \
./sdk/ble/api/src/ble_storage.o \
./sdk/ble/api/src/ble_uuid.o 

C_DEPS += \
./sdk/ble/api/src/ble_attribdb.d \
./sdk/ble/api/src/ble_common.d \
./sdk/ble/api/src/ble_gap.d \
./sdk/ble/api/src/ble_gattc.d \
./sdk/ble/api/src/ble_gattc_util.d \
./sdk/ble/api/src/ble_gatts.d \
./sdk/ble/api/src/ble_l2cap.d \
./sdk/ble/api/src/ble_storage.d \
./sdk/ble/api/src/ble_uuid.d 


# Each subdirectory must supply rules for building sources it contributes
sdk/ble/api/src/ble_attribdb.o: /home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/api/src/ble_attribdb.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -Ddg_configDEVICE=DEVICE_DA1469x -Ddg_configBLACK_ORCA_IC_REV=BLACK_ORCA_IC_REV_A -Ddg_configBLACK_ORCA_IC_STEP=BLACK_ORCA_IC_STEP_A -DNDEBUG -DRELEASE_BUILD -I"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/ble_central_cli/config" -I"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/ble_central_cli/src" -I"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/ble_central_cli/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/middleware/adapters/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/adapter/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/api/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/services/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/services/src" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/manager/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/stack/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/stack/da14690/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/clients/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/clients/src" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/free_rtos/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/memory/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/middleware/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/middleware/osal" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/peripherals/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/snc/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/snc/src" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/system/sys_man" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/system/sys_man/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/util/include" -include"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/ble_central_cli/config/custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sdk/ble/api/src/ble_common.o: /home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/api/src/ble_common.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -Ddg_configDEVICE=DEVICE_DA1469x -Ddg_configBLACK_ORCA_IC_REV=BLACK_ORCA_IC_REV_A -Ddg_configBLACK_ORCA_IC_STEP=BLACK_ORCA_IC_STEP_A -DNDEBUG -DRELEASE_BUILD -I"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/ble_central_cli/config" -I"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/ble_central_cli/src" -I"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/ble_central_cli/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/middleware/adapters/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/adapter/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/api/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/services/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/services/src" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/manager/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/stack/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/stack/da14690/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/clients/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/clients/src" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/free_rtos/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/memory/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/middleware/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/middleware/osal" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/peripherals/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/snc/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/snc/src" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/system/sys_man" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/system/sys_man/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/util/include" -include"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/ble_central_cli/config/custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sdk/ble/api/src/ble_gap.o: /home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/api/src/ble_gap.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -Ddg_configDEVICE=DEVICE_DA1469x -Ddg_configBLACK_ORCA_IC_REV=BLACK_ORCA_IC_REV_A -Ddg_configBLACK_ORCA_IC_STEP=BLACK_ORCA_IC_STEP_A -DNDEBUG -DRELEASE_BUILD -I"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/ble_central_cli/config" -I"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/ble_central_cli/src" -I"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/ble_central_cli/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/middleware/adapters/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/adapter/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/api/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/services/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/services/src" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/manager/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/stack/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/stack/da14690/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/clients/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/clients/src" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/free_rtos/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/memory/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/middleware/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/middleware/osal" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/peripherals/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/snc/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/snc/src" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/system/sys_man" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/system/sys_man/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/util/include" -include"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/ble_central_cli/config/custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sdk/ble/api/src/ble_gattc.o: /home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/api/src/ble_gattc.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -Ddg_configDEVICE=DEVICE_DA1469x -Ddg_configBLACK_ORCA_IC_REV=BLACK_ORCA_IC_REV_A -Ddg_configBLACK_ORCA_IC_STEP=BLACK_ORCA_IC_STEP_A -DNDEBUG -DRELEASE_BUILD -I"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/ble_central_cli/config" -I"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/ble_central_cli/src" -I"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/ble_central_cli/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/middleware/adapters/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/adapter/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/api/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/services/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/services/src" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/manager/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/stack/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/stack/da14690/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/clients/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/clients/src" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/free_rtos/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/memory/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/middleware/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/middleware/osal" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/peripherals/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/snc/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/snc/src" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/system/sys_man" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/system/sys_man/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/util/include" -include"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/ble_central_cli/config/custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sdk/ble/api/src/ble_gattc_util.o: /home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/api/src/ble_gattc_util.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -Ddg_configDEVICE=DEVICE_DA1469x -Ddg_configBLACK_ORCA_IC_REV=BLACK_ORCA_IC_REV_A -Ddg_configBLACK_ORCA_IC_STEP=BLACK_ORCA_IC_STEP_A -DNDEBUG -DRELEASE_BUILD -I"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/ble_central_cli/config" -I"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/ble_central_cli/src" -I"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/ble_central_cli/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/middleware/adapters/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/adapter/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/api/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/services/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/services/src" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/manager/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/stack/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/stack/da14690/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/clients/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/clients/src" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/free_rtos/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/memory/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/middleware/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/middleware/osal" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/peripherals/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/snc/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/snc/src" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/system/sys_man" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/system/sys_man/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/util/include" -include"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/ble_central_cli/config/custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sdk/ble/api/src/ble_gatts.o: /home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/api/src/ble_gatts.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -Ddg_configDEVICE=DEVICE_DA1469x -Ddg_configBLACK_ORCA_IC_REV=BLACK_ORCA_IC_REV_A -Ddg_configBLACK_ORCA_IC_STEP=BLACK_ORCA_IC_STEP_A -DNDEBUG -DRELEASE_BUILD -I"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/ble_central_cli/config" -I"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/ble_central_cli/src" -I"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/ble_central_cli/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/middleware/adapters/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/adapter/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/api/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/services/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/services/src" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/manager/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/stack/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/stack/da14690/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/clients/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/clients/src" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/free_rtos/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/memory/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/middleware/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/middleware/osal" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/peripherals/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/snc/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/snc/src" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/system/sys_man" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/system/sys_man/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/util/include" -include"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/ble_central_cli/config/custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sdk/ble/api/src/ble_l2cap.o: /home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/api/src/ble_l2cap.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -Ddg_configDEVICE=DEVICE_DA1469x -Ddg_configBLACK_ORCA_IC_REV=BLACK_ORCA_IC_REV_A -Ddg_configBLACK_ORCA_IC_STEP=BLACK_ORCA_IC_STEP_A -DNDEBUG -DRELEASE_BUILD -I"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/ble_central_cli/config" -I"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/ble_central_cli/src" -I"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/ble_central_cli/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/middleware/adapters/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/adapter/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/api/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/services/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/services/src" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/manager/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/stack/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/stack/da14690/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/clients/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/clients/src" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/free_rtos/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/memory/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/middleware/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/middleware/osal" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/peripherals/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/snc/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/snc/src" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/system/sys_man" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/system/sys_man/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/util/include" -include"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/ble_central_cli/config/custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sdk/ble/api/src/ble_storage.o: /home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/api/src/ble_storage.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -Ddg_configDEVICE=DEVICE_DA1469x -Ddg_configBLACK_ORCA_IC_REV=BLACK_ORCA_IC_REV_A -Ddg_configBLACK_ORCA_IC_STEP=BLACK_ORCA_IC_STEP_A -DNDEBUG -DRELEASE_BUILD -I"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/ble_central_cli/config" -I"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/ble_central_cli/src" -I"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/ble_central_cli/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/middleware/adapters/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/adapter/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/api/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/services/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/services/src" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/manager/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/stack/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/stack/da14690/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/clients/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/clients/src" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/free_rtos/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/memory/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/middleware/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/middleware/osal" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/peripherals/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/snc/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/snc/src" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/system/sys_man" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/system/sys_man/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/util/include" -include"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/ble_central_cli/config/custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sdk/ble/api/src/ble_uuid.o: /home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/api/src/ble_uuid.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -Ddg_configDEVICE=DEVICE_DA1469x -Ddg_configBLACK_ORCA_IC_REV=BLACK_ORCA_IC_REV_A -Ddg_configBLACK_ORCA_IC_STEP=BLACK_ORCA_IC_STEP_A -DNDEBUG -DRELEASE_BUILD -I"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/ble_central_cli/config" -I"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/ble_central_cli/src" -I"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/ble_central_cli/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/middleware/adapters/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/adapter/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/api/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/services/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/services/src" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/manager/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/stack/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/stack/da14690/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/clients/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/interfaces/ble/clients/src" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/free_rtos/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/memory/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/middleware/config" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/middleware/osal" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/peripherals/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/snc/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/snc/src" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/system/sys_man" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/system/sys_man/include" -I"/home/jon/DA1469x/SDKs/SDK_10.0.10.118/sdk/bsp/util/include" -include"/home/jon/DA1469x/repos/BLE_SDK10_examples/connectivity/ble_central_cli/config/custom_config_qspi.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

