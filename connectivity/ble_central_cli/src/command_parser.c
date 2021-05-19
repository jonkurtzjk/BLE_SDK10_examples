#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <resmgmt.h>
#include "osal.h"
#include "hw_gpio.h"
#include "uart_driver_custom.h"
#include "common.h"
#include "peripheral_commands.h"
#include "gap_commands.h"
#include "gatt_commands.h"
#include "ble_gap.h"
#include "ble_gatt.h"
#include "msg_queues.h"
#include "ad_nvms.h"
#include "hw_cpm_da1469x.h"
#include "hw_uart.h"
#include "ble_central_config.h"

#define UART_NOTIF_RX_CB        (1 << 10)
#define RX_BUFFER_LEN           (UART_RX_BUFFER_LEN)
#define MAX_ARG_LEN             (MAX_ENCODED_b64)
#define BAUD_RATE               (UART_BAUD_RATE)



PRIVILEGED_DATA static OS_TASK command_parser_handle = NULL;
PRIVILEGED_DATA static OS_TASK ble_handle = NULL;


char            uart_rx[RX_BUFFER_LEN];
uint16_t        rx_len;
msg_queue at_cmd_queue;


const gap_conn_params_t default_gap_cp = {
                .interval_min = BLE_CONN_INTERVAL_FROM_MS(BLE_DEFAULT_MIN_INTERVAL_CP_MS),   // 50.00ms
                .interval_max = BLE_CONN_INTERVAL_FROM_MS(BLE_DEFAULT_MAX_INTERVAL_CP_MS),   // 70.00ms
                .slave_latency = 0,
                .sup_timeout = BLE_SUPERVISION_TMO_FROM_MS(BLE_DEFAULT_CONN_TO_MS), // 420ms
        };
//#define AT_PARAM_LEN       (10)

static const char *AT_GAP_COMMAND_STRING[AT_NUM_GAP_COMMANDS] = {
        "GAPSCAN", "GAPCONNECT", "ECHO", "GAPDISC", "GATTBROWSE","GAPSETCP", "GAPCANCELCONNECT", "GATTWRITE", "GATTREAD", "SETBAUD",
        "GAPSETIOCAP", "GAPPAIR", "RESET", "GATTWRITENORESP", "IMGBLOCKUPDATE", "IMGERASEALL", "PASSKEYENTRY", "YESNOENTRY",
        "IMGCHECK", "SUOTASTART", "CLEARBOND", "GETBONDS", "GETFWVERSION"
};


typedef struct
{
        uint8_t hdr;
        char command[20];
        uint8_t num_params;
        char param1[MAX_ARG_LEN];
        char param2[20];
        char param3[10];
        char param4[10];
        char param5[10];
        char param6[10];
}at_command_parse;





uint8_t remove_all_chars(char* str, char c) {
    char *pr = str, *pw = str;
    uint8_t count = 5;


    while (*pr) {
        *pw = *pr++;
        pw += (*pw != c);

    }
    *pw = '\0';

    return count;
}
int parse_byte(char *byte)
{
        int ret = 0;
        uint8_t i;


        for(i = 0; i < 2; i++)
        {
            if(byte[i] >= 'a' && byte[i] <= 'f'){
                    ret += (byte[i] - 'a' + 10) << (i ^ 1)*4;

            }
            else if(byte[i] >= 'A' && byte[i] <= 'F')
            {
                    ret += (byte[i] - 'A' + 10) << (i ^ 1)*4;
            }
            else if(byte[i] >= '0' && byte[i] <= '9')
            {
                    ret += (byte[i] - '0') << (i ^ 1)*4;
            }
            else
            {
                    ret = -1;
                    break;
            }

        }
        return ret;
}
int string_to_bytes(char *val, uint8_t num_bytes)
{
        int ret = 0;
        int i;


        for(i = 2*num_bytes - 1; i >= 0; i--)
        {
            if(val[i] >= 'a' && val[i] <= 'f'){
                    ret += (val[i] - 'a' + 10) << (3-i)*4;

            }
            else if(val[i] >= 'A' && val[i] <= 'F')
            {
                    ret += (val[i] - 'A' + 10) << (3-i)*4;
            }
            else if(val[i] >= '0' && val[i] <= '9')
            {
                    ret += (val[i] - '0') << ((3-i)*4);

            }
            else
            {

                    ret = -1;
                    break;
            }

        }

        return ret;
}
uint8_t* char_hex_2_bytes(char *bytes, uint8_t length)
{
        uint8_t *ret = NULL;

        uint8_t i;
        for(i = 0; i<length; i++){
              sscanf(&bytes[i*2], "%x%x", (unsigned int *)&ret[i], (unsigned int *)&ret[i+1]);
        }
        return ret;
}

void at_parse_set_baud(at_command_parse *at_cmd)
{
        set_baud_cmd *cmd = OS_MALLOC(sizeof(set_baud_cmd));

        bool command_valid = false;

        if(at_cmd->num_params == 2)
        {

               int ret;
               char *_ptr;
               ret = (uint16_t)strtol(at_cmd->param1, &_ptr, 10);

               cmd->id = (HW_UART_ID)1;

               switch(ret)
               {
               case 0:
                       cmd->baud=HW_UART_BAUDRATE_4800;
                       command_valid = true;
                       break;
               case 1:
                       cmd->baud=HW_UART_BAUDRATE_9600;
                       command_valid = true;
                       break;
               case 2:
                       cmd->baud=HW_UART_BAUDRATE_14400;
                       command_valid = true;
                       break;
               case 3:
                       cmd->baud=HW_UART_BAUDRATE_19200;
                       command_valid = true;
                       break;
               case 4:
                       cmd->baud=HW_UART_BAUDRATE_28800;
                       command_valid = true;
                       break;
               case 5:
                       cmd->baud=HW_UART_BAUDRATE_38400;
                       command_valid = true;
                       break;
               case 6:
                       cmd->baud=HW_UART_BAUDRATE_57600;
                       command_valid = true;
                       break;
               case 7:
                       cmd->baud=HW_UART_BAUDRATE_115200;
                       command_valid = true;
                       break;
               case 8:
                       cmd->baud=HW_UART_BAUDRATE_230400;
                       command_valid = true;
                       break;
               case 9:
                       cmd->baud=HW_UART_BAUDRATE_500000;
                       command_valid = true;
                       break;
               case 10:
                       cmd->baud=HW_UART_BAUDRATE_1000000;
                       command_valid = true;
                       break;
               default:
                       command_valid = false;
                       break;
               }
        }

        if(command_valid)
        {
                msg m;
                msg_queue_init_msg(&at_cmd_queue, &m, (MSG_ID)1, (MSG_TYPE)SETBAUD, sizeof(gatt_write_cmd));
                memcpy(m.data, cmd, sizeof(set_baud_cmd));
                msg_queue_put(&at_cmd_queue, &m, OS_QUEUE_FOREVER);
                OS_TASK_NOTIFY(ble_handle, AT_CMD_AVAILABLE, OS_NOTIFY_SET_BITS);
        }
        else
        {
                uart_print_line("ERROR\r\n");
        }

        OS_FREE(cmd);
}
void at_parse_echo(at_command_parse *at_echo)
{

      if(at_echo->num_params == 1)
      {
              uart_print_line("OK");
      }

}
void at_parse_gatt_read(at_command_parse *at_cmd)
{
        gatt_read_cmd *cmd = OS_MALLOC(sizeof(gatt_read_cmd));


        bool command_valid = false;


        if(at_cmd->num_params == 3)
        {


                char *_ptr;
                int ret = 0;

                if(strlen(at_cmd->param1) == 1)
                {
                        cmd->conn_idx = (uint16_t)strtol(at_cmd->param1, &_ptr, 10);
                        command_valid = true;

                }
                if(command_valid){
                        ret = string_to_bytes(&at_cmd->param2[0],2);
                }
                if(ret>=0)
                {
                        cmd->handle = (uint16_t)ret;
                        cmd->offset = 0;
                }
                else
                {
                        command_valid = false;
                }

        }

        if(command_valid)
        {

                msg m;
                msg_queue_init_msg(&at_cmd_queue, &m, (MSG_ID)1, (MSG_TYPE)GATTREAD, sizeof(gatt_read_cmd));
                memcpy(m.data, cmd, sizeof(gatt_read_cmd));
                msg_queue_put(&at_cmd_queue, &m, OS_QUEUE_FOREVER);
                OS_TASK_NOTIFY(ble_handle, AT_CMD_AVAILABLE, OS_NOTIFY_SET_BITS);
        }
        else
        {
                uart_print_line("ERROR");
        }
        OS_FREE(cmd);

}

static void at_parse_write_generic(at_command_parse *at_cmd, gatt_prop_t write_type)
{
       bool command_valid = true;
       MSG_TYPE msg_type = 0;


       switch(write_type)
       {
               case GATT_PROP_WRITE:
                       msg_type = GATTWRITE;
                       break;
               case GATT_PROP_WRITE_NO_RESP:
                       msg_type = GATTWRITENORESP;
                       break;
               default:
                       msg_type = GATTWRITE;
                       break;
       }

       if(at_cmd->num_params == 4)
       {
               uint8_t i;
               //uint8_t *val;
               int ret = 0;
               uint8_t buffer_val[20];
               uint8_t hex_len = (uint8_t)strlen(at_cmd->param1) / 2;


               for(i = 0; i< hex_len; i++){
                    ret = parse_byte(&at_cmd->param1[i*2]);
                    if(ret == -1){
                            command_valid = false;
                    }
                    else
                    {
                            buffer_val[i] = (uint8_t)ret;
                    }

               }
             gatt_write_cmd *cmd = OS_MALLOC(sizeof(gatt_write_cmd));

             if(command_valid)
             {

                     if(strlen(at_cmd->param2) == 4)
                     {
                          ret = string_to_bytes(&at_cmd->param2[0], 2);

                     }

                     if(ret!=-1)
                     {
                             memcpy(cmd->value, &buffer_val[0], hex_len);
                             char *_ptr;
                             cmd->handle = (uint16_t)ret;
                             cmd->length = (uint16_t)hex_len;
                             cmd->offset = 0;
                             cmd->conn_idx = (uint16_t)strtol(at_cmd->param3,&_ptr,10);
                             command_valid = true;
                     }
                     else
                     {
                             command_valid = false;

                     }

             }


             if(command_valid)
             {

                     msg m;
                     msg_queue_init_msg(&at_cmd_queue, &m, (MSG_ID)1, msg_type, sizeof(gatt_write_cmd));
                     memcpy(m.data, cmd, sizeof(gatt_write_cmd));
                     msg_queue_put(&at_cmd_queue, &m, OS_QUEUE_FOREVER);
                     OS_TASK_NOTIFY(ble_handle, AT_CMD_AVAILABLE, OS_NOTIFY_SET_BITS);
             }
             else{
                     uart_print_line("ERROR");
             }
             OS_FREE(cmd);
       }
       else
       {
               uart_print_line("ERROR");
       }
}
void at_parse_gatt_write_no_rsp(at_command_parse *at_cmd)
{
        at_parse_write_generic(at_cmd, GATT_PROP_WRITE_NO_RESP);
}
void at_parse_gatt_write(at_command_parse *at_cmd)
{
        at_parse_write_generic(at_cmd, GATT_PROP_WRITE);
}
void at_parse_cancel_connect(at_command_parse *at_cancel_cmd)
{
        if(at_cancel_cmd->num_params == 1)
        {
                msg m;
                msg_queue_init_msg(&at_cmd_queue, &m, (MSG_ID)1, (MSG_TYPE)GAPCANCELCONNECT, sizeof(gap_disc_cmd));
                msg_queue_put(&at_cmd_queue, &m, OS_QUEUE_FOREVER);
                OS_TASK_NOTIFY(ble_handle, AT_CMD_AVAILABLE, OS_NOTIFY_SET_BITS);
        }
        else
        {
                uart_print_line("ERROR");
        }
}
void at_parse_set_cp(at_command_parse *at_set_cp)
{
        gap_set_cp_cmd *cp = OS_MALLOC(sizeof(gap_set_cp_cmd));
        bool command_valid;

        char *_ptr;

        if(at_set_cp->num_params==6)
        {
                cp->conn_idx = (uint16_t)strtol(at_set_cp->param1, &_ptr, 10);
                cp->gap_cp.interval_min = BLE_CONN_INTERVAL_FROM_MS((uint16_t)strtol(at_set_cp->param2, &_ptr,10));
                cp->gap_cp.interval_max = BLE_CONN_INTERVAL_FROM_MS((uint16_t)strtol(at_set_cp->param3, &_ptr,10));
                cp->gap_cp.slave_latency = (uint16_t)strtol(at_set_cp->param4, &_ptr,10);
                cp->gap_cp.sup_timeout = BLE_SUPERVISION_TMO_FROM_MS((uint16_t)strtol(at_set_cp->param5, &_ptr,10));

                command_valid = true;
        }
        else
        {
                command_valid = false;
        }

        if(command_valid)
        {
              msg m;

              msg_queue_init_msg(&at_cmd_queue, &m, (MSG_ID)1, (MSG_TYPE)GAPSETCP, sizeof(gap_disc_cmd));
              memcpy(m.data, cp, sizeof(gap_set_cp_cmd));
              msg_queue_put(&at_cmd_queue, &m, OS_QUEUE_FOREVER);
              OS_TASK_NOTIFY(ble_handle, AT_CMD_AVAILABLE, OS_NOTIFY_SET_BITS);
        }
        else
        {
                uart_print_line("ERROR");
        }

        OS_FREE(cp);
}

void at_parse_pk_entry(at_command_parse *at_cmd)
{
        gap_pk_reply_cmd_t *cmd = OS_MALLOC(sizeof(gap_pk_reply_cmd_t));
        bool command_valid = false;

        if(at_cmd->num_params == 4)
        {

                int ret;
                char *_ptr;

                ret = strtol(at_cmd->param1, &_ptr, 10);
                if(ret>=0)
                {
                        cmd->conn_idx = (uint16_t)ret;
                        command_valid = true;
                }
                if(command_valid){
                        ret = strtol(at_cmd->param2, &_ptr, 10);
                        switch(ret){
                        case 0:
                                cmd->accept = false;
                                break;
                        case 1:
                                cmd->accept = true;
                                break;
                        default:
                                command_valid=false;
                                break;
                        }
                }

                if(command_valid)
                {
                        ret = strtol(at_cmd->param3, &_ptr, 10);
                        cmd->passkey = ret;
                }

        }

        if(command_valid)
        {
                msg m;

                msg_queue_init_msg(&at_cmd_queue, &m, (MSG_ID)1, (MSG_TYPE)PASSKEYENTRY, sizeof(gap_pk_reply_cmd_t));
                memcpy(m.data, cmd, sizeof(gap_pk_reply_cmd_t));
                msg_queue_put(&at_cmd_queue, &m, OS_QUEUE_FOREVER);
                OS_TASK_NOTIFY(ble_handle, AT_CMD_AVAILABLE, OS_NOTIFY_SET_BITS);

        }else
        {
                uart_print_line("ERROR");
        }
        OS_FREE(cmd);

}

void at_parse_yes_no_entry(at_command_parse *at_cmd)
{

        gap_yes_no_reply_cmd_t *cmd = OS_MALLOC(sizeof(gap_yes_no_reply_cmd_t));
        bool command_valid = false;

        if(at_cmd->num_params == 3)
        {

                int ret;
                char *_ptr;

                ret = strtol(at_cmd->param1, &_ptr, 10);
                if(ret>=0)
                {
                        cmd->conn_idx = (uint16_t)ret;
                        command_valid = true;
                }

                if(command_valid)
                {
                        ret = strtol(at_cmd->param2, &_ptr, 10);
                        switch(ret){
                        case 0:
                                cmd->accept = false;
                                break;
                        case 1:
                                cmd->accept = true;
                                break;
                        default:
                                command_valid=false;
                                break;
                        }

                }

        }

        if(command_valid)
        {
                msg m;

                msg_queue_init_msg(&at_cmd_queue, &m, (MSG_ID)1, (MSG_TYPE)YESNOENTRY, sizeof(gap_yes_no_reply_cmd_t));
                memcpy(m.data, cmd, sizeof(gap_yes_no_reply_cmd_t));
                msg_queue_put(&at_cmd_queue, &m, OS_QUEUE_FOREVER);
                OS_TASK_NOTIFY(ble_handle, AT_CMD_AVAILABLE, OS_NOTIFY_SET_BITS);
        }
        else
        {
                uart_print_line("ERROR");
        }


        OS_FREE(cmd);

}
void at_parse_gap_pair(at_command_parse *at_cmd)
{
        gap_pair_cmd *cmd = OS_MALLOC(sizeof(gap_pair_cmd));
        bool command_valid = false;

        if(at_cmd->num_params == 3)
        {
                int ret;
                char *_ptr;

                ret = strtol(at_cmd->param1, &_ptr, 10);
                if(ret>=0)
                {
                        cmd->conn_idx = (uint16_t)ret;
                        command_valid = true;
                }

                if(command_valid){
                        ret = strtol(at_cmd->param2, &_ptr, 10);
                        switch(ret){
                        case 0:
                                cmd->bond = false;
                                break;
                        case 1:
                                cmd->bond = true;
                                break;
                        default:
                                command_valid=false;
                                break;
                        }
                }

        }

        if(command_valid)
        {
                msg m;

                msg_queue_init_msg(&at_cmd_queue, &m, (MSG_ID)1, (MSG_TYPE)GAPPAIR, sizeof(gap_pair_cmd));
                memcpy(m.data, cmd, sizeof(gap_pair_cmd));
                msg_queue_put(&at_cmd_queue, &m, OS_QUEUE_FOREVER);
                OS_TASK_NOTIFY(ble_handle, AT_CMD_AVAILABLE, OS_NOTIFY_SET_BITS);
        }
        else
        {
                uart_print_line("ERROR");
        }


        OS_FREE(cmd);
}
void at_parse_set_io_cap(at_command_parse *at_cmd)
{
        gap_set_io_cap_cmd *cmd = OS_MALLOC(sizeof(gap_set_io_cap_cmd));
        bool command_valid = false;

        if(at_cmd->num_params==2)
        {
                char *_ptr;
                int param = strtol(at_cmd->param1, &_ptr,10);
                switch(param)
                {
                case 0:
                        cmd->io_cap = GAP_IO_CAP_DISP_ONLY;
                        command_valid = true;
                        break;
                case 1:
                        cmd->io_cap = GAP_IO_CAP_DISP_YES_NO;
                        command_valid = true;
                        break;
                case 2:
                        cmd->io_cap = GAP_IO_CAP_KEYBOARD_ONLY;
                        command_valid=true;
                        break;
                case 3:
                        cmd->io_cap = GAP_IO_CAP_NO_INPUT_OUTPUT;
                        command_valid=true;
                        break;
                case 4:
                        cmd->io_cap = GAP_IO_CAP_KEYBOARD_DISP;
                        command_valid=true;
                        break;
                default:
                        command_valid=false;
                        break;
                }
        }

        if(command_valid)
        {
                msg m;

                msg_queue_init_msg(&at_cmd_queue, &m, (MSG_ID)1, (MSG_TYPE)GAPSETIOCAP, sizeof(gap_set_io_cap_cmd));
                memcpy(m.data, cmd, sizeof(gap_set_cp_cmd));
                msg_queue_put(&at_cmd_queue, &m, OS_QUEUE_FOREVER);
                OS_TASK_NOTIFY(ble_handle, AT_CMD_AVAILABLE, OS_NOTIFY_SET_BITS);
        }
        else{
                uart_print_line("ERROR");
        }
        OS_FREE(cmd);

}
void at_parse_browse(at_command_parse *at_gatt_browse)
{
        gatt_browse_cmd *browse_cmd = OS_MALLOC(sizeof(gatt_browse_cmd));

        bool command_valid;
        char *_ptr;

        if(at_gatt_browse->num_params == 2)
        {
                browse_cmd->conn_idx = (uint16_t)strtol(at_gatt_browse->param1, &_ptr, 10);
                browse_cmd->at_uuid = NULL;
                command_valid = true;

        }
        else
        {
                command_valid = false;
        }

        if(command_valid)
        {
              msg m;

              msg_queue_init_msg(&at_cmd_queue, &m, (MSG_ID)1, (MSG_TYPE)GATTBROWSE, sizeof(gap_disc_cmd));
              memcpy(m.data, browse_cmd, sizeof(gatt_browse_cmd));
              msg_queue_put(&at_cmd_queue, &m, OS_QUEUE_FOREVER);
              OS_TASK_NOTIFY(ble_handle, AT_CMD_AVAILABLE, OS_NOTIFY_SET_BITS);
        }
        else{
                uart_print_line("ERROR");
        }

        OS_FREE(browse_cmd);
}
void at_parse_disc(at_command_parse *at_gap_disc)
{
        gap_disc_cmd *disc_cmd = OS_MALLOC(sizeof(gap_disc_cmd));
        uint8_t reas;
        bool command_valid;

        char *_ptr;

        if(at_gap_disc->num_params==2 )
        {
              disc_cmd->conn_idx=(uint16_t)strtol(at_gap_disc->param1, &_ptr, 10);
              disc_cmd->reason = BLE_HCI_ERROR_REMOTE_USER_TERM_CON;
              command_valid = true;


        }
        else if(at_gap_disc->num_params == 3)
        {
                disc_cmd->conn_idx=(uint16_t)strtol(at_gap_disc->param1, &_ptr, 10);
                reas = (uint8_t)strtol(at_gap_disc->param2,&_ptr, 10);

                switch(reas)
                {
                case 0:
                        disc_cmd->reason = BLE_HCI_ERROR_REMOTE_USER_TERM_CON;
                        command_valid = true;
                        break;
                case 1:
                        disc_cmd->reason = BLE_HCI_ERROR_AUTH_FAILURE;
                        command_valid = true;
                        break;
                case 2:
                        disc_cmd->reason = BLE_HCI_ERROR_REMOTE_DEV_TERM_LOW_RESOURCES;
                        command_valid = true;
                        break;
                case 3:
                        disc_cmd->reason = BLE_HCI_ERROR_REMOTE_DEV_POWER_OFF;
                        command_valid = true;
                        break;
                case 4:
                        disc_cmd->reason = BLE_HCI_ERROR_UNSUPPORTED_REMOTE_FEATURE;
                        command_valid = true;
                        break;
                case 5:
                        disc_cmd->reason = BLE_HCI_ERROR_PAIRING_WITH_UNIT_KEY_NOT_SUP;
                        command_valid = true;
                        break;
                case 6:
                        disc_cmd->reason = BLE_HCI_ERROR_UNACCEPTABLE_CONN_INT;
                        command_valid = true;
                        break;
                default:
                        command_valid = false;
                        break;


                }
        }
        else
        {
                command_valid = false;

        }


        if(command_valid)
        {
              msg m;

              msg_queue_init_msg(&at_cmd_queue, &m, (MSG_ID)1, (MSG_TYPE)GAPDISC, sizeof(gap_disc_cmd));
              memcpy(m.data, disc_cmd, sizeof(gap_disc_cmd));
              msg_queue_put(&at_cmd_queue, &m, OS_QUEUE_FOREVER);
              OS_TASK_NOTIFY(ble_handle, AT_CMD_AVAILABLE, OS_NOTIFY_SET_BITS);
        }
        else
        {
                uart_print_line("ERROR");
        }
        OS_FREE(disc_cmd);

}
void at_parse_gap_connect(at_command_parse *at_gap_connect)
{
        gap_connect_cmd *connect_cmd = OS_MALLOC(sizeof(gap_connect_cmd));
        uint8_t bd_addr[6];

        if(strlen(at_gap_connect->param1) == 17 && at_gap_connect->num_params > 1)
        {
               uint8_t num_bytes = 6;
               uint8_t i;
               int tempbyte;
               char tempByte_char[2];
               uint8_t colon_count;
               uint8_t bd_addr_pos;
               colon_count = remove_all_chars(at_gap_connect->param1, ':');

               if(colon_count != 5)
               {
                       uart_print_line("ERROR");
                       goto DONE;
               }

               for(i = 0; i < num_bytes; i++)
               {
                    tempByte_char[0] = at_gap_connect->param1[i*2];
                    tempByte_char[1] = at_gap_connect->param1[i*2 + 1];
                    tempbyte = parse_byte(tempByte_char);

                    if(tempbyte != -1)
                    {
                         bd_addr_pos = num_bytes-1;
                         bd_addr_pos -= i;
                         bd_addr[bd_addr_pos] = (uint8_t)tempbyte;
                    }
                    else
                    {
                            uart_print_line("ERROR");
                            goto DONE;
                    }

               }

               memcpy(connect_cmd->addr.addr, bd_addr,6);
               //connect_cmd->addr.addr = (uint8_t *)bd_addr;

               if(at_gap_connect->num_params == 3)
               {
                       if(at_gap_connect->param2[0] == '0')
                       {
                              connect_cmd->addr.addr_type = PUBLIC_ADDRESS;
                       }
                       else if(at_gap_connect->param2[0] == '1')
                       {
                               connect_cmd->addr.addr_type = PRIVATE_ADDRESS;
                       }
                       else
                       {

                               uart_print_line("ERROR");
                               goto DONE;
                       }

                       connect_cmd->cp = default_gap_cp;
               }
               else if(at_gap_connect->num_params == 2)
               {
                       connect_cmd->addr.addr_type = PUBLIC_ADDRESS;
                       connect_cmd->cp = default_gap_cp;
               }
               else if(at_gap_connect->num_params > 3 && at_gap_connect->num_params == 6)
               {
                     char *ptr;
                     float min;
                     float max;
                     float slave;
                     float to;
                     /*
                     uint16_t min;
                     uint16_t max;
                     uint16_t slave;
                     uint16_t to;


                     min = (uint16_t)strtol(at_gap_connect->param3, &ptr, 10);
                     max = (uint16_t)strtol(at_gap_connect->param4, &ptr, 10);
                     slave = (uint16_t)strtol(at_gap_connect->param5, &ptr, 10);
                     to = (uint16_t)strtol(at_gap_connect->param6, &ptr, 10);
                     */
                     min = strtof(at_gap_connect->param3, &ptr);
                     max = strtof(at_gap_connect->param4, &ptr);
                     slave = strtof(at_gap_connect->param5, &ptr);
                     to = strtof(at_gap_connect->param6, &ptr);

                     if(at_gap_connect->param2[0] == '0')
                     {
                            connect_cmd->addr.addr_type = PUBLIC_ADDRESS;
                     }
                     else if(at_gap_connect->param2[0] == '1')
                     {
                             connect_cmd->addr.addr_type = PRIVATE_ADDRESS;
                     }
                     else
                     {

                             uart_print_line("ERROR");
                             goto DONE;
                     }

                     if( (min <= max) && min >7 )
                     {
                             connect_cmd->cp.interval_min = BLE_CONN_INTERVAL_FROM_MS( min );
                             connect_cmd->cp.interval_max = BLE_CONN_INTERVAL_FROM_MS( max );
                     }
                     else
                     {
                             uart_print_line("ERROR");
                             goto DONE;
                     }

                     if(to >= 2*max + max)
                     {
                             connect_cmd->cp.sup_timeout = BLE_SUPERVISION_TMO_FROM_MS(to);
                     }
                     else
                     {
                             connect_cmd->cp.sup_timeout = BLE_SUPERVISION_TMO_FROM_MS(2000);
                     }
                     connect_cmd->cp.slave_latency = slave;


               }
               else
               {
                       uart_print_line("ERROR");
                       goto DONE;
               }

               msg m;

               msg_queue_init_msg(&at_cmd_queue, &m, (MSG_ID)1, (MSG_TYPE)GAPCONNECT, sizeof(gap_connect_cmd));
               memcpy(m.data, connect_cmd, sizeof(gap_connect_cmd));
               msg_queue_put(&at_cmd_queue, &m, OS_QUEUE_FOREVER);
               OS_TASK_NOTIFY(ble_handle, AT_CMD_AVAILABLE, OS_NOTIFY_SET_BITS);


        }
        else
        {
                uart_print_line("ERROR");
        }

        DONE:;

        OS_FREE(connect_cmd);
}

static void at_parse_reset(at_command_parse *at_reset)
{
      if(at_reset->num_params == 1)
      {
              hw_cpm_reboot_system();
      }else
      {
              uart_print_line("ERROR");
      }
}
static void at_parse_gap_scan(at_command_parse *at_gap_scan)
{
        gap_scan_cmd *scan_cmd = OS_MALLOC(sizeof(gap_scan_cmd));

        scan_cmd->scan_type = GAP_SCAN_ACTIVE;
        scan_cmd->scan_filt_wlist = false;
        scan_cmd->scan_interval = BLE_SCAN_INTERVAL_FROM_MS(BLE_DEFAULT_SCAN_INTERVAL_MS);
        scan_cmd->scan_window = BLE_SCAN_INTERVAL_FROM_MS(BLE_DEFUALT_SCAN_WINDOW_MS);
        scan_cmd->scan_mode = GAP_SCAN_GEN_DISC_MODE;
#ifdef CUSTOM_CENTRAL
        scan_cmd->scan_filt_dupl = false;
#else

        scan_cmd->scan_filt_dupl = false;
#endif


        int32_t s_start;
        char *s_start_ptr;
        bool command_valid;

        if( (at_gap_scan->num_params > 1) && (at_gap_scan->num_params < 3))
        {
                s_start = (int32_t)strtol(at_gap_scan->param1, &s_start_ptr,10);

                if( s_start == (int32_t)0 )
                {
                        scan_cmd->scan_start = false;
                        command_valid=true;
                }
                else if( s_start == (int32_t)1)
                {
                        scan_cmd->scan_start = true;
                        command_valid=true;
                }
                else{
                        uart_print_line("ERROR");
                }

                if(command_valid)
                {
                        msg m;
                        //m.type = (uint16_t)GAPSCAN;
                        msg_queue_init_msg(&at_cmd_queue, &m, (MSG_ID)1, (MSG_TYPE)GAPSCAN, sizeof(gap_scan_cmd));
                        memcpy(m.data, scan_cmd, sizeof(gap_scan_cmd));
                        msg_queue_put(&at_cmd_queue, &m, OS_QUEUE_FOREVER);
                        OS_TASK_NOTIFY(ble_handle, AT_CMD_AVAILABLE, OS_NOTIFY_SET_BITS);

                }
        }
        else
        {
                uart_print_line("ERROR");
        }

        OS_FREE(scan_cmd);

}

static void at_parse_imgblock(at_command_parse *at_cmd)
{
        write_img_block_cmd_t *cmd = OS_MALLOC(sizeof(write_img_block_cmd_t));
        bool command_valid = false;

        memset(cmd ,0, sizeof(write_img_block_cmd_t));

        if(at_cmd->num_params == 6)
        {
                char *s_start_ptr;
                int block_size = strtol(at_cmd->param2, &s_start_ptr,10);
                int addr;
                int crc16;
                int write_to_flash;

                if(block_size <= MAX_ENCODED_b64)
                {
                        memcpy(cmd->img_block_b64, at_cmd->param1, (uint32_t)strlen(at_cmd->param1));
                        cmd->data_len = (uint16_t)block_size;

                        addr = strtol(at_cmd->param3, &s_start_ptr,10);
                        crc16 = strtol(at_cmd->param4, &s_start_ptr,10);
                        write_to_flash = strtol(at_cmd->param5, &s_start_ptr,10);

                        cmd->addr_wr = (uint32_t)addr;
                        cmd->crc16 = (uint16_t)crc16;
                        cmd->write_to_flash = (bool)write_to_flash;

                        command_valid = true;

                }

        }

        if(command_valid)
        {
                msg m;
                msg_queue_init_msg(&at_cmd_queue, &m, (MSG_ID)1, (MSG_TYPE)IMGBLOCKUPDATE, sizeof(write_img_block_cmd_t));
                memcpy(m.data, cmd, sizeof(write_img_block_cmd_t));
                msg_queue_put(&at_cmd_queue, &m, OS_QUEUE_FOREVER);
                OS_TASK_NOTIFY(ble_handle, AT_CMD_AVAILABLE, OS_NOTIFY_SET_BITS);
        }else
        {
                uart_print_line("ERROR");
        }

        OS_FREE(cmd);

}

static void at_parse_img_erase(at_command_parse *at_cmd)
{
        if(at_cmd->num_params == 1)
        {
                msg m;
                msg_queue_init_msg(&at_cmd_queue, &m, (MSG_ID)1, (MSG_TYPE)IMGERASEALL, 1);
                msg_queue_put(&at_cmd_queue, &m, OS_QUEUE_FOREVER);
                OS_TASK_NOTIFY(ble_handle, AT_CMD_AVAILABLE, OS_NOTIFY_SET_BITS);
        }else
        {
                uart_print_line("ERROR");
        }
}

static void at_parse_img_check(at_command_parse *at_cmd)
{
        if(at_cmd->num_params == 1)
        {
                msg m;
                msg_queue_init_msg(&at_cmd_queue, &m, (MSG_ID)1, (MSG_TYPE)IMGCHECK, 1);
                msg_queue_put(&at_cmd_queue, &m, OS_QUEUE_FOREVER);
                OS_TASK_NOTIFY(ble_handle, AT_CMD_AVAILABLE, OS_NOTIFY_SET_BITS);
        }else
        {
                uart_print_line("ERROR");
        }
}

static void at_parse_suota_start(at_command_parse *at_cmd)
{

        suota_start_cmd_t *cmd = OS_MALLOC(sizeof(suota_start_cmd_t));
        bool command_valid = false;

        if(at_cmd->num_params == 2)
        {

                char *s_start_ptr;
                int force_gatt = strtol(at_cmd->param1, &s_start_ptr,10);

                if(force_gatt < 2)
                {
                        cmd->force_gatt_update = (bool)force_gatt;
                        command_valid = true;
                }

        }else
        {
                uart_print_line("ERROR");
        }

        if(command_valid)
        {
                msg m;
                msg_queue_init_msg(&at_cmd_queue, &m, (MSG_ID)1, (MSG_TYPE)SUOTASTART, sizeof(suota_start_cmd_t));
                memcpy(m.data, cmd, sizeof(suota_start_cmd_t));
                msg_queue_put(&at_cmd_queue, &m, OS_QUEUE_FOREVER);
                OS_TASK_NOTIFY(ble_handle, AT_CMD_AVAILABLE, OS_NOTIFY_SET_BITS);
        }

        OS_FREE(cmd);

}

static void at_parse_clear_bonds(at_command_parse *at_cmd)
{

        clear_bond_cmd_t *bond_cmd = OS_MALLOC(sizeof(suota_start_cmd_t));
        uint8_t bd_addr[6];
        bool command_valid = true;

       if(strlen(at_cmd->param1) == 17 && at_cmd->num_params == 3)
       {
              uint8_t num_bytes = 6;
              uint8_t i;
              int tempbyte;
              char tempByte_char[2];
              uint8_t colon_count;
              uint8_t bd_addr_pos;
              colon_count = remove_all_chars(at_cmd->param1, ':');

              if(colon_count != 5)
              {
                      command_valid = false;
              }

              if(command_valid)
              {

                      for(i = 0; i < num_bytes; i++)
                      {
                           tempByte_char[0] = at_cmd->param1[i*2];
                           tempByte_char[1] = at_cmd->param1[i*2 + 1];
                           tempbyte = parse_byte(tempByte_char);

                           if(tempbyte != -1)
                           {
                                bd_addr_pos = num_bytes-1;
                                bd_addr_pos -= i;
                                bd_addr[bd_addr_pos] = (uint8_t)tempbyte;
                           }
                           else
                           {
                                   command_valid = false;
                           }

                      }


                      memcpy(bond_cmd->address.addr, bd_addr,6);
              }

              if(command_valid)
              {
                      char *s_start_ptr;
                      int addr_type = strtol(at_cmd->param2, &s_start_ptr,10);

                      if(addr_type < 2)
                      {
                              bond_cmd->address.addr_type = (addr_type_t)addr_type;
                      }
                      else
                      {
                            command_valid = false;
                      }
              }
       }

       if(command_valid)
       {
               msg m;
               msg_queue_init_msg(&at_cmd_queue, &m, (MSG_ID)1, (MSG_TYPE)CLEARBOND, sizeof(clear_bond_cmd_t));
               memcpy(m.data, bond_cmd, sizeof(clear_bond_cmd_t));
               msg_queue_put(&at_cmd_queue, &m, OS_QUEUE_FOREVER);
               OS_TASK_NOTIFY(ble_handle, AT_CMD_AVAILABLE, OS_NOTIFY_SET_BITS);
       }else
       {
               uart_print_line("ERROR");
       }

       OS_FREE(bond_cmd);

}

static void at_parse_get_bonds(at_command_parse *at_cmd)
{
        if(at_cmd->num_params == 1)
        {
                msg m;
                msg_queue_init_msg(&at_cmd_queue, &m, (MSG_ID)1, (MSG_TYPE)GETBONDS, 1);
                msg_queue_put(&at_cmd_queue, &m, OS_QUEUE_FOREVER);
                OS_TASK_NOTIFY(ble_handle, AT_CMD_AVAILABLE, OS_NOTIFY_SET_BITS);
        }else
        {
                uart_print_line("ERROR");
        }

}

static void at_parse_get_version(at_command_parse *at_cmd)
{
        if(at_cmd->num_params == 1)
        {
               msg m;
               msg_queue_init_msg(&at_cmd_queue, &m, (MSG_ID)1, (MSG_TYPE)GETVERSION, 1);
               msg_queue_put(&at_cmd_queue, &m, OS_QUEUE_FOREVER);
               OS_TASK_NOTIFY(ble_handle, AT_CMD_AVAILABLE, OS_NOTIFY_SET_BITS);
        }
        else
        {
               uart_print_line("ERROR");
        }
}

static void cp_parse_command(at_command_parse *at_cmd)
{

        switch(at_cmd->hdr){
        case GAPSCAN:
                at_parse_gap_scan(at_cmd);
                break;
        case GAPCONNECT:
                at_parse_gap_connect(at_cmd);
                break;
        case ECHO:
                at_parse_echo(at_cmd);
                break;
        case GAPDISC:
                at_parse_disc(at_cmd);
                break;
        case GATTBROWSE:
                at_parse_browse(at_cmd);
                break;
        case GAPSETCP:
                at_parse_set_cp(at_cmd);
                break;
        case GAPCANCELCONNECT:
                at_parse_cancel_connect(at_cmd);
                break;
        case GATTWRITE:
                at_parse_gatt_write(at_cmd);
                break;
        case GATTWRITENORESP:
                at_parse_gatt_write_no_rsp(at_cmd);
                break;
        case GATTREAD:
                at_parse_gatt_read(at_cmd);
                break;
        case SETBAUD:
                at_parse_set_baud(at_cmd);
                break;
        case GAPSETIOCAP:
                at_parse_set_io_cap(at_cmd);
                break;
        case GAPPAIR:
                at_parse_gap_pair(at_cmd);
                break;
        case RESET:
                at_parse_reset(at_cmd);
                break;
        case IMGBLOCKUPDATE:
                at_parse_imgblock(at_cmd);
                break;
        case IMGERASEALL:
                at_parse_img_erase(at_cmd);
                break;
        case PASSKEYENTRY:
                at_parse_pk_entry(at_cmd);
                break;
        case YESNOENTRY:
                at_parse_yes_no_entry(at_cmd);
                break;
        case IMGCHECK:
                at_parse_img_check(at_cmd);
                break;
        case SUOTASTART:
                at_parse_suota_start(at_cmd);
                break;
        case CLEARBOND:
                at_parse_clear_bonds(at_cmd);
                break;
        case GETBONDS:
                at_parse_get_bonds(at_cmd);
                break;
        case GETVERSION:
                at_parse_get_version(at_cmd);
        default:
                break;
        }

}
static void cp_parse_input(uint8_t *cmd, uint16_t cmd_len)
{


        if( uart_rx[0] == 'D' && uart_rx[1] == 'L' && uart_rx[2] == 'G')
        {

              if(cmd_len == 3)
              {
                      uart_print_line("OK");
              }
              else if(uart_rx[3] == '+' && cmd_len > 3)
              {

                      at_command_parse *input = OS_MALLOC(sizeof(at_command_parse));

                      uart_rx[cmd_len] = '\0';

                      input->num_params = sscanf(uart_rx, "DLG+%s %s %s %s %s %s", input->command,
                                                                             input->param1,
                                                                             input->param2,
                                                                             input->param3,
                                                                             input->param4,
                                                                             input->param5);

                      if(input->num_params > 0)
                      {

                              int8_t i = AT_NUM_GAP_COMMANDS - 1;
                              int ret;
                              while(i>=0)
                              {
                                     ret = strncmp(AT_GAP_COMMAND_STRING[i], input->command, strlen(AT_GAP_COMMAND_STRING[i]));
                                     if(ret == 0)
                                     {
                                         input->hdr = (uint8_t) i ;
                                         cp_parse_command(input);
                                         break;
                                     }
                                     i--;

                              }
                              if(i == -1 ){

                                      uart_print_line("ERROR");

                              }



                      }
                      else{

                            uart_print_line("ERROR");
                      }
                      OS_FREE(input);


              }
              else
              {
                      uart_print_line("ERROR");
              }
        }
        else
        {
                uart_print_line("ERROR");
        }

}
static void uart_drv_cb(uart_drv_cust_error_t err, uint16_t len)
{
        switch(err)
        {
                case UART_CUST_CR_DETECT:
                        rx_len = len-1;
                        OS_TASK_NOTIFY_FROM_ISR(command_parser_handle, UART_NOTIF_RX_CB, OS_NOTIFY_SET_BITS);
                        break;
                case UART_CUST_OVERFLOW:
                        break;
                default:
                        break;
        }
}
void command_parser_task(void *params)
{

        memset(uart_rx, 0, RX_BUFFER_LEN);

        uart_gpio_config_t config = {
                .uart_rx_func = SER1_RX_FUNC,
                .uart_rx_pin = SER1_RX_PIN,
                .uart_rx_port = SER1_RX_PORT,
                .uart_tx_func = SER1_TX_FUNC,
                .uart_tx_pin = SER1_TX_PIN,
                .uart_rx_port = SER1_TX_PORT,
        };

        uart_driver_custom_init(HW_UART2, BAUD_RATE, &config, &uart_drv_cb);

        uart_print_line("APP_INITIALIZED");

        for (;;) {

                OS_BASE_TYPE ret;
                uint32_t notif;


                /* notify watchdog on each loop */
                //sys_watchdog_notify(wdog_id2);

                /* suspend watchdog while blocking on OS_TASK_NOTIFY_WAIT() */
                //sys_watchdog_suspend(wdog_id2);

                /*
                 * Wait on any of the notification bits, then clear them all
                 */
                ret = OS_TASK_NOTIFY_WAIT(0, OS_TASK_NOTIFY_ALL_BITS, &notif, OS_TASK_NOTIFY_FOREVER);
                /* Blocks forever waiting for task notification. The return value must be OS_OK */
                OS_ASSERT(ret == OS_OK);

                /* resume watchdog */
                //sys_watchdog_notify_and_resume(wdog_id2);

                if(notif & UART_NOTIF_RX_CB)
                {
                        uart_drv_custom_get_data((uint8_t *)uart_rx, rx_len);
                        cp_parse_input((uint8_t *)uart_rx, rx_len);
                        memset(uart_rx, 0, RX_BUFFER_LEN);
                        rx_len = 0;

                }

        }
}

OS_TASK command_parser_start_task(const uint8 prio, OS_TASK handle)
{

        ble_handle = handle;
        OS_TASK_CREATE("command_task",                /* The text name assigned to the task, for
                                                                   debug only; not used by the kernel. */
                       command_parser_task,             /* The function that implements the task. */
                       NULL,                            /* The parameter passed to the task. */
                       5000 * OS_STACK_WORD_SIZE,        /* The number of bytes to allocate to the
                                                           stack of the task. */
                       prio,/* The priority assigned to the task. */
                       command_parser_handle
                       );                         /* The task handle. */
        OS_ASSERT(command_parser_handle);

        return command_parser_handle;

}

