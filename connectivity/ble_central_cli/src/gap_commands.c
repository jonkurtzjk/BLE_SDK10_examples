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
#include "gap_commands.h"
#include "uart_driver_custom.h"

void gap_get_cmd(msg *message)
{
        msg_queue_get(&at_cmd_queue, message, OS_QUEUE_FOREVER);
}

const char *format_bd_address(const bd_address_t *addr)
{
        static char buf[19];
        int i;

        for (i = 0; i < sizeof(addr->addr); i++) {
                int idx;

                // for printout, address should be reversed
                idx = sizeof(addr->addr) - i - 1;
                sprintf(&buf[i * 3], "%02x:", addr->addr[idx]);
        }

        buf[sizeof(buf) - 2] = '\0';

        return buf;
}
void handle_gap_pair(gap_pair_cmd *cmd){
        ble_error_t ret;
        ret = ble_gap_pair(cmd->conn_idx, cmd->bond);
        if(ret == BLE_STATUS_OK)
        {
                uart_printf("OK\r\n");
        }
        else
        {
                uart_printf("BLE_ERROR\r\n");
        }
}
void handle_gap_pk_reply(gap_pk_reply_cmd_t *cmd)
{
        ble_error_t ret;
        ret = ble_gap_passkey_reply(cmd->conn_idx, cmd->accept, cmd->passkey);
        if(ret == BLE_STATUS_OK)
        {
                uart_printf("OK\r\n");
        }
        else
        {
                uart_printf("BLE_ERROR\r\n");
        }
}
void handle_gap_yes_no_reply(gap_yes_no_reply_cmd_t *cmd)
{

        ble_error_t ret;
        ret = ble_gap_numeric_reply(cmd->conn_idx, cmd->accept);

        if(ret == BLE_STATUS_OK)
        {
                uart_printf("OK\r\n");
        }
        else
        {
                uart_printf("BLE_ERROR\r\n");
        }

}
void handle_gap_set_io_cmd(gap_set_io_cap_cmd *cmd)
{
        ble_error_t ret;
        ret = ble_gap_set_io_cap(cmd->io_cap);
        if(ret == BLE_STATUS_OK)
        {
                uart_printf("OK\r\n");
        }
        else
        {
                uart_printf("BLE_ERROR\r\n");
        }
}
void handle_gap_cancel_conn_cmd(void)
{
        ble_error_t ret;
        ret = ble_gap_connect_cancel();

        if(ret == BLE_STATUS_OK)
        {
                uart_printf("OK\r\n");
        }
        else
        {
                uart_printf("BLE_ERROR\r\n");
        }
}
void handle_gap_set_cp(gap_set_cp_cmd *cmd)
{
        ble_error_t ret;
        ret = ble_gap_conn_param_update(cmd->conn_idx, &cmd->gap_cp);
        if(ret == BLE_STATUS_OK)
        {
                uart_printf("OK\r\n");
        }
        else
        {
                uart_printf("BLE_ERROR\r\n");
        }
}
void handle_gap_disc(gap_disc_cmd *cmd)
{
        ble_error_t ret;
        ret = ble_gap_disconnect(cmd->conn_idx, cmd->reason);

        if(ret == BLE_STATUS_OK)
        {
                uart_printf("OK\r\n");
        }
        else
        {
                uart_printf("BLE_ERROR\r\n");
        }
}
void handle_gap_connect(gap_connect_cmd *cmd)
{
        ble_error_t ret;

        ret = ble_gap_connect( &(cmd->addr), &(cmd->cp));
        if(ret == BLE_STATUS_OK)
        {
                uart_printf("OK\r\n");
        }
        else
        {
                uart_printf("BLE_ERROR\r\n");
        }
}
void handle_gap_scan_request( gap_scan_cmd *cmd)
{

        ble_error_t ret;

        if(cmd->scan_start == true){
                ret = ble_gap_scan_start(cmd->scan_type, cmd->scan_mode, cmd->scan_interval,
                                        cmd->scan_window, cmd->scan_filt_wlist, cmd->scan_filt_dupl);
                if(ret == BLE_STATUS_OK)
                {
                        uart_printf("OK\r\n");
                }

        }
        else
        {
                ret = ble_gap_scan_stop();
                if(ret == BLE_STATUS_OK)
                {
                        uart_printf("OK\r\n");
                }
        }


        if(ret != BLE_STATUS_OK)
        {
                uart_printf("BLE_ERROR\r\n");
        }



}

void handle_clear_bond(clear_bond_cmd_t *cmd)
{
        ble_error_t err;
        err = ble_gap_unpair(&cmd->address);

        if(err != BLE_STATUS_OK)
        {
                uart_printf("ERROR: %02x\r\n", err);
        }
        else
        {
                uart_printf("OK\r\n");
        }

}

void handle_get_bonds(void)
{
        ble_error_t err;
        size_t length = defaultBLE_MAX_BONDED;
        gap_device_t *devices = OS_MALLOC(length*sizeof(gap_device_t));


        err = ble_gap_get_devices(GAP_DEVICE_FILTER_ALL, NULL, &length, devices);

        if(err != BLE_STATUS_OK)
        {
               uart_printf("ERROR: %d\r\n", err);
        }else
        {
               uart_printf("EVT_GET_BONDS: %d devices\r\n", length);

               uint8_t i = 0;
               for(i = 0; i<length; i++)
               {
                       uart_printf("EVT_BONDED_DEVICE: slot: %d: addr: %s addr_type: %d bonded: %d connected: %d\r\n",
                                       i, format_bd_address(&devices->address), devices->address.addr_type,
                                       devices->bonded, devices->connected);
               }

               uart_printf("EVT_GET_BONDS_COMPLETE\r\n");
        }

        OS_FREE(devices);
}

