/**
 ****************************************************************************************
 *
 * @file ble_central_config.h
 *
 * @brief Application configuration
 *
 * Copyright (C) 2015-2017 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef BLE_CENTRAL_CONFIG_H_
#define BLE_CENTRAL_CONFIG_H_

#define CLI_FW_VERSION                          "0.0.1"
#define CLI_FW_VERSION_SIZE                     (sizeof(CLI_FW_VERSION))

#define UART_BAUD_RATE                          (1000000)
#define UART_RX_BUFFER_LEN                      (1500)          //1500 minimum suggested
#define UART_EOL                                ("\r\n")
#define EOL_SIZE                                (sizeof(UART_EOL)-1)

#define BLE_DEFAULT_SCAN_INTERVAL_MS            (50)
#define BLE_DEFUALT_SCAN_WINDOW_MS              (50)

#define BLE_DEFAULT_MIN_INTERVAL_CP_MS          (10)
#define BLE_DEFAULT_MAX_INTERVAL_CP_MS          (10)
#define BLE_DEFAULT_CONN_TO_MS                  (500)

#endif /* BLE_CENTRAL_CONFIG_H_ */
