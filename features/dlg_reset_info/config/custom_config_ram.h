/**
 ****************************************************************************************
 *
 * @file custom_config_ram.h
 *
 * @brief Board Support Package. User Configuration file for execution from RAM.
 *
 * Copyright (C) 2015-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef CUSTOM_CONFIG_RAM_H_
#define CUSTOM_CONFIG_RAM_H_

#include "bsp_definitions.h"

#define CONFIG_RETARGET

/*****************************************************************************
 * ADF Related defines
 *****************************************************************************/

#define dg_configENABLE_ADF                             (1)
#define dg_configIMAGE_SETUP                            (PRODUCTION_MODE)
#define dg_configSKIP_MAGIC_CHECK_AT_START              (1)
#define dg_configFREERTOS_ENABLE_THREAD_AWARENESS       (1)

#ifdef dg_configENABLE_ADF
#define ADDTL_UNINIT            (1024)          //Could be optimized if needed
#else
#define ADDTL_UNINIT            (256)
#endif



/******************************************************************************/

#define dg_configRETAINED_UNINIT_SECTION_SIZE    (ADDTL_UNINIT)

#define dg_configUSE_LP_CLK                     ( LP_CLK_32768 )
#define dg_configCODE_LOCATION                  NON_VOLATILE_IS_NONE

#define dg_configUSE_WDOG                       (0)

#define dg_configFLASH_CONNECTED_TO             (FLASH_IS_NOT_CONNECTED)


#define dg_configUSE_SW_CURSOR                  (1)

/*************************************************************************************************\
 * FreeRTOS specific config
 */
#define OS_FREERTOS                              /* Define this to use FreeRTOS */
#define configTOTAL_HEAP_SIZE                    14000   /* This is the FreeRTOS Total Heap Size */

/*************************************************************************************************\
 * Peripheral specific config
 */

#define dg_configRF_ENABLE_RECALIBRATION        (0)

#define dg_configFLASH_ADAPTER                  (0)
#define dg_configNVMS_ADAPTER                   (0)
#define dg_configNVMS_VES                       (0)

/* Include bsp default values */
#include "bsp_defaults.h"
/* Include middleware default values */
#include "middleware_defaults.h"

#endif /* CUSTOM_CONFIG_RAM_H_ */