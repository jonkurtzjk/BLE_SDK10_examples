#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "osal.h"
#include "sys_watchdog.h"
#include "ble_att.h"
#include "ble_common.h"
#include "ble_config.h"
#include "ble_gap.h"
#include "common.h"
#include "msg_queues.h"
#include "ble_gattc.h"
#include "gatt_commands.h"
#include "uart_driver_custom.h"

uint8_t test_buff[20];
void handle_gatt_read_cmd(gatt_read_cmd *evt)
{
        ble_error_t ret;

        ret = ble_gattc_read(evt->conn_idx, evt->handle, evt->offset);


        if(ret == BLE_STATUS_OK)
        {
              uart_printf("OK\r\n");
        }
        else
        {
              uart_printf("ERROR\r\n");
        }

}

void handle_gatt_write_no_rsp_cmd(gatt_write_cmd *evt)
{
       ble_error_t ret;

       ret = ble_gattc_write_no_resp(evt->conn_idx, evt->handle, 0,
               evt->length , (uint8_t *)&evt->value);


       if(ret == BLE_STATUS_OK)
       {
              uart_printf("OK\r\n");
       }
       else
       {
              uart_printf("ERROR\r\n");
       }

}
void handle_gatt_write_cmd(gatt_write_cmd *evt)
{
       ble_error_t ret;

       ret = ble_gattc_write(evt->conn_idx, evt->handle, 0,
               evt->length , (uint8_t *)&evt->value);


       if(ret == BLE_STATUS_OK)
       {
              uart_printf("OK\r\n");
       }
       else
       {
              uart_printf("ERROR\r\n");
       }

}
void handle_gatt_browse_cmd(gatt_browse_cmd *evt)
{
        ble_error_t ret;

        ret = ble_gattc_browse(evt->conn_idx, evt->at_uuid);

        if(ret == BLE_STATUS_OK)
        {
                uart_printf("OK\r\n");
                uart_printf("BROWSE STARTED\r\n");
        }
        else
        {
                uart_printf("ERROR\r\n");
        }

}
