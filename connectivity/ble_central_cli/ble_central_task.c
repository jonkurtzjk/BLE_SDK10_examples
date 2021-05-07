/*
 * ble_central_task.c
 *
 *  Created on: Oct 10, 2016
 *      Author: jkurtz
 */
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "osal.h"
#include "hw_uart.h"
#include "sys_watchdog.h"
#include "ble_att.h"
#include "ble_common.h"
#include "ble_config.h"
#include "ble_gap.h"
#include "ble_gatts.h"
#include "ble_gattc.h"
#include "ble_service.h"
#include "ble_uuid.h"
#include "common.h"
#include "gap_commands.h"
#include "gatt_commands.h"
#include "peripheral_commands.h"
#include "uart_driver_custom.h"
#include "dis_client.h"
#include "suota.h"
#include "suota_client.h"
#include "ble_l2cap.h"


#define L2CAP_CREDITS           5

/*
 * This is the maximum number of bytes the client is allowed to send before receiving a notification from
 * the peer to continue. Setting it too high will increase transfer speed but will also require more heap
 * on peer's side while SUOTA is ongoing.
 */
#define DEFAULT_PATCH_LEN       (1536)

#define MAX_FOUND_DEVICES       25



#define mainCOMMAND_PARSER_PRIORITY             ( OS_TASK_PRIORITY_NORMAL )
#define UART_TASK_PRIORITY                      ( OS_TASK_PRIORITY_NORMAL+ 1 )

typedef enum {
        PENDING_ACTION_ENABLE_NOTIF = (1 << 0),
        PENDING_ACTION_READ_L2CAP_PSM = (1 << 1),
        PENDING_ACTION_READ_MANUFACTURER = (1 << 2),
        PENDING_ACTION_READ_MODEL = (1 << 3),
        PENDING_ACTION_READ_FW_VERSION = (1 << 4),
        PENDING_ACTION_READ_SW_VERSION = (1 << 5),
        PENDING_ACTION_READ_SUOTA_VERSION = (1 << 6),
        PENDING_ACTION_READ_PATCH_DATA_CHAR_SIZE = (1 << 7),
} pending_action_t;

typedef struct {
        bd_address_t addr;
        uint16_t conn_idx;
        uint16_t mtu;
        uint16_t l2cap_mtu;
        uint16_t patch_data_char_size;

        ble_client_t *dis_client;
        ble_client_t *suota_client;

        pending_action_t pending_init;

        uint16_t psm;
        uint16_t scid;
        size_t pending_bytes;
        bool flow_stop;
} peer_info_t;

typedef struct {
        OS_TICK_TIME start_time;

        bool use_l2cap;

        size_t offset;

        size_t patch_len;
        size_t block_len;

        bool w4_write_completed : 1;    // waiting for completed event for GATT write
        bool w4_write_ack : 1;          // waiting for ACK after writing patch_len
        bool w4_transfer_status : 1;
} update_info_t;

typedef enum {
        APP_STATE_IDLE,
        APP_STATE_SCANNING,
        APP_STATE_CONNECTING,
        APP_STATE_CONNECTED,
        APP_STATE_UPDATING,
} app_state_t;


__RETAINED static peer_info_t peer_info;

__RETAINED static update_info_t update_info;

__RETAINED static size_t image_size;

__RETAINED static nvms_t nvms;

__RETAINED static app_state_t app_state;

#ifdef BD_CUSTOM_CENTRAL
/* return buffer in formatted hexdump  ASCII */
/* print buffer in formatted hexdump and ASCII */

static void format_value(uint16_t length, const uint8_t *value)
{
        static char buf1[49]; // buffer for hexdump (16 * 3 chars + \0)


        while (length) {
                int i;

                memset(buf1, 0, sizeof(buf1));

                for (i = 0; i < 16 && length > 0; i++, length--, value++) {
                        sprintf(&buf1[i * 3], "%02x ", (int) *value);


                }

                uart_printf("%-49s\r\n", buf1);
        }
}
#endif


static void send_patch_data(void)
{
        const void *data_ptr;
        size_t len = update_info.use_l2cap ? peer_info.l2cap_mtu - 2 : peer_info.mtu - 3;

        /* Check if transfer is completed */
        if (update_info.offset >= image_size) {
                uint32_t time = OS_TICKS_2_MS(OS_GET_TICK_COUNT() - update_info.start_time);
                uint32_t speed = image_size * 1000 / time;

                /* We should have *exactly* the same offset as total_size */
                OS_ASSERT(update_info.offset == image_size);

                uart_printf("EVT_SUOTA_TRANSFER_COMPLETE\r\n");
                uart_printf("\ttransferred %u bytes in %lu.%03lu seconds (%lu bytes/sec)\r\n", image_size, time / 1000, time % 1000, speed);

                suota_client_send_end_cmd(peer_info.suota_client);

                /* Now we are waiting for image transfer status */
                update_info.w4_transfer_status = true;
                return;
        }

        if (update_info.offset + len > image_size) {
                len = image_size - update_info.offset;
        }

        ad_nvms_get_pointer(nvms, update_info.offset, len, &data_ptr);

        if (update_info.use_l2cap) {
                ble_l2cap_send(peer_info.conn_idx, peer_info.scid, len, data_ptr);
                peer_info.pending_bytes = len;
                /* Offset will be advanced once data are sent over the air */
        } else {
                /* For GATT we cannot send more than patch_data_char_size bytes */
                if (len > peer_info.patch_data_char_size) {
                        len = peer_info.patch_data_char_size;
                }

                /* For GATT transfer we also need to check if data fits in patch_len */
                if (update_info.block_len + len > update_info.patch_len) {
                        len = update_info.patch_len - update_info.block_len;
                }

                suota_client_send_patch_data(peer_info.suota_client, len, data_ptr);

                update_info.offset += len;
                update_info.block_len += len;

                /* Wait for write completed event */
                update_info.w4_write_completed = true;

                /* After writing full patch_len, wait also for remote to ACK */
                if (update_info.block_len == update_info.patch_len) {
                        update_info.w4_write_ack = true;
                }
        }

        uart_printf("EVT_SUOTA_SENT: %d bytes\r\n", update_info.offset);
}

static void send_patch_data_gatt(void)
{
        /*
         * Simply return if waiting for either write completed or ACK - we will be back here when
         * flag is cleared.
         *
         * Note that this is a bit redundant as long as SUOTA uses Write Command to send image
         * blocks, but having this will also allow to work properly in case Write Request is used.
         */
        if (update_info.w4_write_completed || update_info.w4_write_ack) {
                return;
        }

        /* If full image block was written, we need to check if patch_len should be updated */
        if (update_info.block_len == update_info.patch_len) {
                update_info.block_len = 0;

                if (image_size - update_info.offset < update_info.patch_len) {
                        update_info.patch_len = image_size - update_info.offset;
                        suota_client_set_patch_len(peer_info.suota_client, update_info.patch_len);

                        /* New block write will be triggered when patch_len is updated */
                        return;
                }
        }

        send_patch_data();
}

static void pending_clear_and_check(pending_action_t action)
{
        peer_info.pending_init &= ~action;

        /* If all flags are cleared, we're ready to start SUOTA */
        if (!peer_info.pending_init) {
                uart_printf("EVT_SUOTA_READY\r\n");
        }
}

void suota_set_event_state_completed_cb(ble_client_t *client, suota_client_event_t event,
                                                                                att_error_t status)
{
        pending_clear_and_check(PENDING_ACTION_ENABLE_NOTIF);
}

static void suota_status_notif_cb(ble_client_t *client, uint8_t status)
{
        uart_printf("EVT_SUOTA_NOTIF: status: %02x\r\n", status);

        if (update_info.w4_write_ack) {
                update_info.w4_write_ack = false;

                if (status == SUOTA_CMP_OK) {
                        send_patch_data_gatt();
                } else {
                        uart_printf("ERROR: SUOTA remote did not ACK data block\r\n");
                }
        }

        if (update_info.w4_transfer_status) {
                update_info.w4_transfer_status = false;

                if (status == SUOTA_CMP_OK) {
                        uart_printf("EVT_SUOTA_REBOOT_REMOTE\r\n");
                        suota_client_send_reboot_cmd(peer_info.suota_client);
                }
        }
}

static void suota_read_l2cap_psm_completed_cb(ble_client_t *client, att_error_t status, uint16_t psm)
{
        peer_info.psm = psm;

        if (status != ATT_ERROR_OK) {
                uart_printf("\tERROR: L2CAP PSM failed to read (0x%02x)\r\n", status);
        } else {
                uart_printf("EVT_SUOTA_L2CAP PSM: 0x%04x\r\n", psm);
        }

        pending_clear_and_check(PENDING_ACTION_READ_L2CAP_PSM);
}


static void suota_get_suota_version_completed_cb(ble_client_t *client, att_error_t status, uint8_t suota_version)
{
        if (status != ATT_ERROR_OK) {
                printf("ERROR: failed to get SUOTA version (code=0x%02x)\r\n", status);
                return;
        }

        uart_printf("EVT_SUOTA_VERSION: v%u\r\n", suota_version);

        /*
         * Query version specific characteristics
         */
        if (suota_version >= SUOTA_VERSION_1_3) {
                peer_info.pending_init |= PENDING_ACTION_READ_PATCH_DATA_CHAR_SIZE;
                suota_client_get_patch_data_char_size(client);
        }

        pending_clear_and_check(PENDING_ACTION_READ_SUOTA_VERSION);
}

static void suota_get_patch_data_char_size_completed_cb(ble_client_t *client, att_error_t status, uint16_t patch_data_char_size)
{
        if (status != ATT_ERROR_OK) {
                uart_printf("ERROR: failed to get patch data characteristic size (code=0x%02x)\r\n", status);
                return;
        }

        peer_info.patch_data_char_size = patch_data_char_size;

        pending_clear_and_check(PENDING_ACTION_READ_PATCH_DATA_CHAR_SIZE);
}

static void suota_set_mem_dev_completed_cb(ble_client_t *client, att_error_t status)
{
        if (status != ATT_ERROR_OK) {
                uart_printf("ERROR: failed to configure device (code=0x%02x)\r\n", status);
                return;
        }

        if (update_info.use_l2cap) {
                uart_printf("SUOTA: update via L2CAP CoC...\r\n");

                /* Connect L2CAP channel, this will trigger image transfer */
                ble_l2cap_connect(peer_info.conn_idx, peer_info.psm, L2CAP_CREDITS,
                                                                                &peer_info.scid);
        } else {
                printf("SUOTA: update via GATT...\r\n");
                update_info.start_time = OS_GET_TICK_COUNT();

                /* Write patch len, this will trigger image transfer */
                update_info.patch_len = DEFAULT_PATCH_LEN;

                suota_client_set_patch_len(peer_info.suota_client, update_info.patch_len);
        }
}

static void suota_send_end_cmd_completed_cb(ble_client_t *client, att_error_t status)
{
        if (status != ATT_ERROR_OK) {
                uart_printf("ERROR: SPOTAR_IMAGE_END failed on remote\r\n");
        }
}

static void suota_set_patch_len_completed_cb(ble_client_t *client, att_error_t status)
{
        if (status != ATT_ERROR_OK) {
                uart_printf("ERROR: Failed to set new patch block length (code=0x%02x)\r\n", status);
                return;
        }

        send_patch_data_gatt();
}

static void suota_send_patch_data_completed_cb(ble_client_t *client, att_error_t status)
{
        if (status != ATT_ERROR_OK) {
                uart_printf("ERROR: Failed to send patch data\r\n");
                return;
        }

        if (update_info.w4_write_completed) {
                update_info.w4_write_completed = false;
                send_patch_data_gatt();
        }
}

static const suota_client_callbacks_t suota_callbacks = {
                .set_event_state_completed = suota_set_event_state_completed_cb,
                .status_notif = suota_status_notif_cb,
                .read_l2cap_psm_completed = suota_read_l2cap_psm_completed_cb,
                .get_suota_version_completed = suota_get_suota_version_completed_cb,
                .get_patch_data_char_size_completed = suota_get_patch_data_char_size_completed_cb,
                .set_mem_dev_completed = suota_set_mem_dev_completed_cb,
                .send_end_cmd_completed = suota_send_end_cmd_completed_cb,
                .set_patch_len_completed = suota_set_patch_len_completed_cb,
                .send_patch_data_completed = suota_send_patch_data_completed_cb,
};

static void dis_read_completed_cb(ble_client_t *dis_client, att_error_t status,
                                                        dis_client_cap_t capability,
                                                        uint16_t length, const uint8_t *value)
{
        switch (capability) {
        case DIS_CLIENT_CAP_MANUFACTURER_NAME:
                uart_printf("\tManufacturer: %.*s\r\n", length, value);
                pending_clear_and_check(PENDING_ACTION_READ_MANUFACTURER);
                break;
        case DIS_CLIENT_CAP_MODEL_NUMBER:
                uart_printf("\tModel: %.*s\r\n", length, value);
                pending_clear_and_check(PENDING_ACTION_READ_MODEL);
                break;
        case DIS_CLIENT_CAP_FIRMWARE_REVISION:
                uart_printf("\tFirmware version: %.*s\r\n", length, value);
                pending_clear_and_check(PENDING_ACTION_READ_FW_VERSION);
                break;
        case DIS_CLIENT_CAP_SOFTWARE_REVISION:
                uart_printf("\tSoftware version: %.*s\r\n", length, value);
                pending_clear_and_check(PENDING_ACTION_READ_SW_VERSION);
                break;
        default:
                break;
        }
}

static const dis_client_callbacks_t dis_callbacks = {
        .read_completed = dis_read_completed_cb,
};

static void handle_suota_start(suota_start_cmd_t *cmd)
{
        image_size = peripheral_get_image_size();
        if(!image_size)
        {
             uart_printf("ERROR: No valid image detected\r\n") ;
             return;
        }
        if (!peer_info.suota_client) {
                uart_printf("\tERROR: peer does not support SUOTA or GATTBROWSE was never completed\r\n");

                return;
        }

        if (app_state < APP_STATE_CONNECTED) {
                printf("ERROR: not connected\r\n");
                return;
        }
        uart_printf("OK\r\n");

        uart_printf("Updating...\r\n");

        memset(&update_info, 0, sizeof(update_info));
        update_info.use_l2cap = peer_info.psm && !cmd->force_gatt_update;

        suota_client_set_mem_dev(peer_info.suota_client, SUOTA_CLIENT_MEM_DEV_SPI_FLASH, 0);

        app_state = APP_STATE_UPDATING;


}


static void handle_evt_l2cap_connected(ble_evt_l2cap_connected_t *evt)
{
        uart_printf("SUOTA: L2CAP Connected\r\n");
        update_info.start_time = OS_GET_TICK_COUNT();

        peer_info.l2cap_mtu = evt->mtu;

        send_patch_data();
}

static void handle_evt_l2cap_connection_failed(ble_evt_l2cap_connection_failed_t *evt)
{
        uart_printf("Data channel connection failed\r\n");
        uart_printf("\tStatus: %d\r\n", evt->status);
}

static void handle_evt_l2cap_disconnected(ble_evt_l2cap_disconnected_t *evt)
{
        uart_printf("Data channel disconnected (reason=0x%04x)\r\n", evt->reason);

        if (app_state == APP_STATE_UPDATING) {
                app_state = APP_STATE_CONNECTED;
                return;
        }
}

static void handle_evt_l2cap_sent(ble_evt_l2cap_sent_t *evt)
{
        switch (evt->status) {
        case BLE_STATUS_OK:
                /* Mark data as sent */
                update_info.offset += peer_info.pending_bytes;
                peer_info.pending_bytes = 0;
                break;
        case BLE_ERROR_L2CAP_MTU_EXCEEDED:
                /*
                 * This should not happen because we obey MTU set on channel. To recover we can set
                 * "usable" MTU to minimul value and retry.
                 */
                peer_info.l2cap_mtu = 23;

                OS_ASSERT(0);
                break;
        case BLE_ERROR_L2CAP_NO_CREDITS:
                /* Do nothing, this will just stop flow and wait for credits from remote */
                break;
        default:
                /* This means wither channel or connection no longer exist, nothing more to do.  */
                return;
        }

        if (evt->remote_credits > 0) {
                send_patch_data();
                return;
        }

        peer_info.flow_stop = true;
}

static void handle_evt_l2cap_remote_credits_changed(ble_evt_l2cap_credit_changed_t *evt)
{
        if (!peer_info.flow_stop || evt->remote_credits == 0) {
                return;
        }

        send_patch_data();

        peer_info.flow_stop = false;
}

static void handle_evt_l2cap_data_ind(ble_evt_l2cap_data_ind_t *evt)
{
        ble_l2cap_add_credits(evt->conn_idx, evt->scid, evt->local_credits_consumed);
}
static const char *format_uuid(const att_uuid_t *uuid)
{
        static char buf[37];

        if (uuid->type == ATT_UUID_16) {
                sprintf(buf, "0x%04x", uuid->uuid16);
        } else {
                int i;
                int idx = 0;

                for (i = ATT_UUID_LENGTH; i > 0; i--) {
                        if (i == 12 || i == 10 || i == 8 || i == 6) {
                                buf[idx++] = '-';
                        }

                        idx += sprintf(&buf[idx], "%02x", uuid->uuid128[i - 1]);
                }
        }

        return buf;
}
static const char *format_properties(uint8_t properties)
{
        static const char props_str[] = "BRXWNISE"; // each letter corresponds to single property
        static char buf[9];
        int i;

        // copy full properties mask
        memcpy(buf, props_str, sizeof(props_str));

        for (i = 0; i < 8; i++) {
                // clear letter from mask if property not present
                if ((properties & (1 << i)) == 0) {
                        buf[i] = '-';
                }
        }

        return buf;
}

void handle_read_printout(ble_evt_gattc_read_completed_t *evt)
{
        uart_printf("EVT_GATT_READ: conn_idx:%04x handle:%04x offset:%04x\r\n", evt->conn_idx, evt->handle, evt->offset);
        uart_printf("\tValue:");

        uint8_t i;
        for(i = 0; i < evt->length ; i++)
        {
                uart_printf("%02x ", (int)evt->value[i]);
        }

        uart_printf("\r\n");


}
static void handle_evt_gap_security_request(ble_evt_gap_security_request_t *evt)
{
        uart_printf("EVT_PAIR_REQUEST\r\n\tconn_idx: 0x%04x, bond: %d, mitm: %d\r\n", evt->conn_idx, evt->bond,evt->mitm );
        ble_gap_pair(evt->conn_idx, evt->bond);
}
void handle_evt_gap_pair_completed(ble_evt_gap_pair_completed_t *evt)
{
        uart_printf("EVT_GAP_PAIR_COMPLETE\r\n\tconn_idx: 0x%04x, bond: %d, mitm: %d, status: %d\r\n", evt->conn_idx,
                evt->bond, evt->mitm, evt->status);
}
void handle_evt_gattc_discover_char(ble_evt_gattc_discover_char_t *evt)
{
        ble_gattc_read(evt->conn_idx, evt->value_handle, 0);
}
static void handle_evt_gattc_read_completed(ble_evt_gattc_read_completed_t *evt)
{
       switch(evt->status)
       {
       case ATT_ERROR_OK:
               uart_printf("ATT_ERROR_OK\r\n");
               handle_read_printout(evt);
               break;
       case ATT_ERROR_INVALID_HANDLE:
               uart_printf("ATT_ERROR_INVALID_HANDLE\r\n");
               break;
       case ATT_ERROR_READ_NOT_PERMITTED:
               uart_printf("ATT_ERROR_READ_NOT_PERMITTED\r\n");
               break;
       case ATT_ERROR_WRITE_NOT_PERMITTED:
               uart_printf("ATT_ERROR_WRITE_NOT_PERMITTED\r\n");
               break;
       case ATT_ERROR_INVALID_PDU:
               uart_printf("ATT_ERROR_INVALID_PDU\r\n");
               break;
       case ATT_ERROR_INSUFFICIENT_AUTHENTICATION:
               uart_printf("ATT_ERROR_INSUFFICIENT_AUTHENTICATION\r\n");
               break;
       case ATT_ERROR_REQUEST_NOT_SUPPORTED:
               uart_printf("ATT_ERROR_REQUEST_NOT_SUPPORTED\r\n");
               break;
       case ATT_ERROR_INVALID_OFFSET:
               uart_printf("ATT_ERROR_INVALID_OFFSET\r\n");
               break;
       case ATT_ERROR_INSUFFICIENT_AUTHORIZATION:
               uart_printf("ATT_ERROR_INSUFFICIENT_AUTHORIZATION\r\n");
               break;
       case ATT_ERROR_PREPARE_QUEUE_FULL:
               uart_printf("ATT_ERROR_PREPARE_QUEUE_FULL\r\n");
               break;
       case ATT_ERROR_ATTRIBUTE_NOT_FOUND:
               uart_printf("ATT_ERROR_ATTRIBUTE_NOT_FOUND\r\n");
               break;
       case ATT_ERROR_ATTRIBUTE_NOT_LONG:
               uart_printf("ATT_ERROR_ATTRIBUTE_NOT_LONG\r\n");
               break;
       case ATT_ERROR_INSUFFICIENT_KEY_SIZE:
               uart_printf("ATT_ERROR_INSUFFICIENT_KEY_SIZE\r\n");
               break;
       case ATT_ERROR_INVALID_VALUE_LENGTH:
               uart_printf("ATT_ERROR_INVALID_VALUE_LENGTH\r\n");
               break;
       case ATT_ERROR_UNLIKELY:
               uart_printf("ATT_ERROR_UNLIKELY\r\n");
               break;
       case ATT_ERROR_INSUFFICIENT_ENCRYPTION:
               uart_printf("ATT_ERROR_INSUFFICIENT_ENCRYPTION\r\n");
               break;
       case ATT_ERROR_UNSUPPORTED_GROUP_TYPE:
               uart_printf("ATT_ERROR_UNSUPPORTED_GROUP_TYPE\r\n");
               break;
       case ATT_ERROR_INSUFFICIENT_RESOURCES:
               uart_printf("ATT_ERROR_INSUFFICIENT_RESOURCES\r\n");
               break;
       case ATT_ERROR_APPLICATION_ERROR:
               uart_printf("ATT_ERROR_APPLICATION_ERROR\r\n");
               break;
       case ATT_ERROR_CCC_DESCRIPTOR_IMPROPERLY_CONFIGURED:
               uart_printf("ATT_ERROR_CCC_DESCRIPTOR_IMPROPERLY_CONFIGURED\r\n");
               break;
       case ATT_ERROR_PROCEDURE_ALREADY_IN_PROGRESS:
               uart_printf("ATT_ERROR_PROCEDURE_ALREADY_IN_PROGRESS\r\n");
               break;
       default:
               uart_printf("EVT_READ_ERROR\r\n");
               break;
       }
}
static void handle_evt_gattc_notification(ble_evt_gattc_notification_t *evt)
{
        uart_printf("EVT_NOTIF_RECEIVED: conn_idx=0x%04x handle=0x%04x ", evt->conn_idx, evt->handle);
        uart_printf("Value:");
        uint8_t i;
        for(i = 0; i < evt->length ; i++)
        {
                uart_printf("%02x ", (int)evt->value[i]);
        }

        uart_printf("\r\n");

}

static void handle_evt_gattc_indication(ble_evt_gattc_indication_t *evt)
{
        uart_printf("EVT_IND_RECEIVED: conn_idx=0x%04x handle=0x%04x ", evt->conn_idx, evt->handle);
        uart_printf("Value:");
        uint8_t i;
        for(i = 0; i < evt->length ; i++)
        {
                uart_printf("%02x ", (int)evt->value[i]);
        }

        uart_printf("\r\n");

}
static void handle_evt_gattc_write_completed(ble_evt_gattc_write_completed_t *evt)
{

        switch(evt->status)
        {
        case ATT_ERROR_OK:
                uart_printf("ATT_ERROR_OK\r\n");
                break;
        case ATT_ERROR_INVALID_HANDLE:
                uart_printf("ATT_ERROR_INVALID_HANDLE\r\n");
                break;
        case ATT_ERROR_READ_NOT_PERMITTED:
                uart_printf("ATT_ERROR_READ_NOT_PERMITTED\r\n");
                break;
        case ATT_ERROR_WRITE_NOT_PERMITTED:
                uart_printf("ATT_ERROR_WRITE_NOT_PERMITTED\r\n");
                break;
        case ATT_ERROR_INVALID_PDU:
                uart_printf("ATT_ERROR_INVALID_PDU\r\n");
                break;
        case ATT_ERROR_INSUFFICIENT_AUTHENTICATION:
                uart_printf("ATT_ERROR_INSUFFICIENT_AUTHENTICATION\r\n");
                break;
        case ATT_ERROR_REQUEST_NOT_SUPPORTED:
                uart_printf("ATT_ERROR_REQUEST_NOT_SUPPORTED\r\n");
                break;
        case ATT_ERROR_INVALID_OFFSET:
                uart_printf("ATT_ERROR_INVALID_OFFSET\r\n");
                break;
        case ATT_ERROR_INSUFFICIENT_AUTHORIZATION:
                uart_printf("ATT_ERROR_INSUFFICIENT_AUTHORIZATION\r\n");
                break;
        case ATT_ERROR_PREPARE_QUEUE_FULL:
                uart_printf("ATT_ERROR_PREPARE_QUEUE_FULL\r\n");
                break;
        case ATT_ERROR_ATTRIBUTE_NOT_FOUND:
                uart_printf("ATT_ERROR_ATTRIBUTE_NOT_FOUND\r\n");
                break;
        case ATT_ERROR_ATTRIBUTE_NOT_LONG:
                uart_printf("ATT_ERROR_ATTRIBUTE_NOT_LONG\r\n");
                break;
        case ATT_ERROR_INSUFFICIENT_KEY_SIZE:
                uart_printf("ATT_ERROR_INSUFFICIENT_KEY_SIZE\r\n");
                break;
        case ATT_ERROR_INVALID_VALUE_LENGTH:
                uart_printf("ATT_ERROR_INVALID_VALUE_LENGTH\r\n");
                break;
        case ATT_ERROR_UNLIKELY:
                uart_printf("ATT_ERROR_UNLIKELY\r\n");
                break;
        case ATT_ERROR_INSUFFICIENT_ENCRYPTION:
                uart_printf("ATT_ERROR_INSUFFICIENT_ENCRYPTION\r\n");
                break;
        case ATT_ERROR_UNSUPPORTED_GROUP_TYPE:
                uart_printf("ATT_ERROR_UNSUPPORTED_GROUP_TYPE\r\n");
                break;
        case ATT_ERROR_INSUFFICIENT_RESOURCES:
                uart_printf("ATT_ERROR_INSUFFICIENT_RESOURCES\r\n");
                break;
        case ATT_ERROR_APPLICATION_ERROR:
                uart_printf("ATT_ERROR_APPLICATION_ERROR\r\n");
                break;
        case ATT_ERROR_CCC_DESCRIPTOR_IMPROPERLY_CONFIGURED:
                uart_printf("ATT_ERROR_CCC_DESCRIPTOR_IMPROPERLY_CONFIGURED\r\n");
                break;
        case ATT_ERROR_PROCEDURE_ALREADY_IN_PROGRESS:
                uart_printf("ATT_ERROR_PROCEDURE_ALREADY_IN_PROGRESS\r\n");
                break;
        default:
                uart_printf("WRITE_ERROR\r\n");
                break;

        }
}
static void handle_evt_gap_update_param_completed(ble_evt_gap_conn_param_updated_t* evt)
{
        uart_printf("EVT_PARAMS_UPDATE_COMPLETE\r\n");
}
static void handle_evt_gap_update_req(ble_evt_gap_conn_param_update_req_t * evt)
{
        uart_printf("EVT_PARAM_UPDATE_REQUEST: conn_idx=%d\r\n", evt->conn_idx);
        uart_printf("\tInterval Min:  %d\r\n", evt->conn_params.interval_min);
        uart_printf("\tInterval Max:  %d\r\n", evt->conn_params.interval_max);
        uart_printf("\tSlave Latency:  %d\r\n", evt->conn_params.slave_latency);
        uart_printf("\tSupervision TO:  %d\r\n", evt->conn_params.sup_timeout);


        ble_gap_conn_param_update_reply(evt->conn_idx, true);

}
static void handle_evt_gattc_browse_completed(ble_evt_gattc_browse_completed_t *evt)
{
        uart_printf("EVT_BROWSE_COMPLETED\r\n");

        if(!peer_info.suota_client)
        {
                return;
        }

        if (peer_info.dis_client) {
                dis_client_cap_t cap = dis_client_get_capabilities(peer_info.dis_client);

                /* Read manufacturer name (if supported by DIS server) */
                if (cap & DIS_CLIENT_CAP_MANUFACTURER_NAME) {
                        peer_info.pending_init |= PENDING_ACTION_READ_MANUFACTURER;
                        dis_client_read(peer_info.dis_client, DIS_CLIENT_CAP_MANUFACTURER_NAME);
                }

                /* Read model number (if supported by DIS server) */
                if (cap & DIS_CLIENT_CAP_MODEL_NUMBER) {
                        peer_info.pending_init |= PENDING_ACTION_READ_MODEL;
                        dis_client_read(peer_info.dis_client, DIS_CLIENT_CAP_MODEL_NUMBER);
                }

                /* Read firmware version (if supported by DIS server) */
                if (cap & DIS_CLIENT_CAP_FIRMWARE_REVISION) {
                        peer_info.pending_init |= PENDING_ACTION_READ_FW_VERSION;
                        dis_client_read(peer_info.dis_client, DIS_CLIENT_CAP_FIRMWARE_REVISION);
                }

                /* Read software version (if supported by DIS server) */
                if (cap & DIS_CLIENT_CAP_SOFTWARE_REVISION) {
                        peer_info.pending_init |= PENDING_ACTION_READ_SW_VERSION;
                        dis_client_read(peer_info.dis_client, DIS_CLIENT_CAP_SOFTWARE_REVISION);
                }
        }

        /* Enable state notifications (write CCC) */
        peer_info.pending_init |= PENDING_ACTION_ENABLE_NOTIF;
        suota_client_set_event_state(peer_info.suota_client, SUOTA_CLIENT_EVENT_STATUS_NOTIF, true);

        /* Read L2CAP PSM (if supported by SUOTA server) */
        if (suota_client_get_capabilities(peer_info.suota_client) & SUOTA_CLIENT_CAP_L2CAP_PSM) {
                peer_info.pending_init |= PENDING_ACTION_READ_L2CAP_PSM;
                suota_client_read_l2cap_psm(peer_info.suota_client);
        }

        if (suota_client_get_capabilities(peer_info.suota_client) & SUOTA_CLIENT_CAP_SUOTA_VERSION) {
                peer_info.pending_init |= PENDING_ACTION_READ_SUOTA_VERSION;
                suota_client_get_suota_version(peer_info.suota_client);
        }

        uart_printf("EVT_READING_SUOTA_DATABASE_CHARS...\r\n");
}
static void handle_evt_gattc_browse_svc(ble_evt_gattc_browse_svc_t *evt)
{
        uint8_t prop = 0;
        int i;

        uart_printf("EVT_SERVICE_FOUND: conn_idx=%04x start_h=%04x end_h=%04x\r\n", evt->conn_idx,
                                                                        evt->start_h, evt->end_h);

        //ble_gattc_discover_char(evt->conn_idx, evt->start_h, evt->end_h, NULL);

        uart_printf("\t%04x serv %s\r\n", evt->start_h, format_uuid(&evt->uuid));

        switch (evt->uuid.uuid16) {
                case UUID_SUOTA:
                {
                        if (!peer_info.suota_client) {
                                peer_info.suota_client = suota_client_init(&suota_callbacks, evt);
                                if(peer_info.suota_client)
                                {
                                        ble_client_add(peer_info.suota_client);
                                }
                        }

                }break;
                case UUID_SERVICE_DIS:
                {
                        if (!peer_info.dis_client) {
                                peer_info.dis_client = dis_client_init(&dis_callbacks, evt);
                                if(peer_info.dis_client)
                                {
                                        ble_client_add(peer_info.dis_client);
                                }
                        }


                }break;

                default:
                        break;
        }

        for (i = 0; i < evt->num_items; i++) {
                gattc_item_t *item = &evt->items[i];
                att_uuid_t uuid;

                switch (item->type) {
                case GATTC_ITEM_TYPE_INCLUDE:
                        uart_printf("\t%04x incl %s\r\n", item->handle, format_uuid(&item->uuid));
                        break;
                case GATTC_ITEM_TYPE_CHARACTERISTIC:
                        uart_printf("\t%04x char %s prop=%02x (%s)\r\n", item->handle,
                                                format_uuid(&evt->uuid), item->c.properties,
                                                format_properties(item->c.properties));

                        uart_printf("\t%04x ---- %s\r\n", item->handle, format_uuid(&item->uuid));
                        // store properties, useful when handling descriptor later
                        prop = item->c.properties;
                        break;
                case GATTC_ITEM_TYPE_DESCRIPTOR:
                        uart_printf("\t%04x desc %s\r\n", item->handle, format_uuid(&item->uuid));
                        uint16_t ccc = 0;
                        ble_uuid_create16(UUID_GATT_CLIENT_CHAR_CONFIGURATION, &uuid);
                        if (ble_uuid_equal(&uuid, &item->uuid) && (prop & GATT_PROP_NOTIFY)) {

                                ccc = GATT_CCC_NOTIFICATIONS;
                                ble_gattc_write(evt->conn_idx, item->handle, 0,
                                                                sizeof(ccc), (uint8_t *) &ccc);
                        }

                        if(ble_uuid_equal(&uuid, &item->uuid) && (prop & GATT_PROP_INDICATE))
                        {
                                ccc = GATT_CCC_INDICATIONS;
                                ble_gattc_write(evt->conn_idx, item->handle, 0,
                                                                sizeof(ccc), (uint8_t *) &ccc);
                        }

                        break;
                default:
                        uart_printf("\t%04x ????\r\n", item->handle);
                        break;
                }
        }

}

static void handle_evt_gap_data_length_changed(ble_evt_gap_data_length_changed_t *evt)
{
        uart_printf("BLE_EVT_DATA_LEN_CHANGED: \tconn_idx:0x%04x rx_len:%04x tx_len:%04x\r\n",
                                        evt->conn_idx, evt->max_rx_length, evt->max_tx_length);
}

static void handle_evt_gap_peer_version(ble_evt_gap_peer_version_t *evt)
{
        uart_printf("BLE_EVT_GAP_PEER_VERSION: \tconn_idx:%04x lmp_version:%02x comp_id:%04x lmp_subversion:%04x\r\n",
                                        evt->conn_idx, evt->lmp_version, evt->company_id, evt->lmp_subversion);
}
static void handle_evt_mtu_changed(ble_evt_gattc_mtu_changed_t *evt)
{
        uart_printf("BLE_EVT_MTU_CHANGED: \tconn_idx:%04x mtu_size:%04x\r\n", evt->conn_idx, evt->mtu);
}
static void handle_evt_gap_passkey_notify(ble_evt_gap_passkey_notify_t * evt)
{
        uart_printf("BLE_EVT_GAP_PASSKEY_DISPLAY: conn_idx: %04x passkey: %u\r\n",
                                                                evt->conn_idx, evt->passkey);
}
static void handle_evt_gap_passkey_request(ble_evt_gap_passkey_request_t *evt)
{
        uart_printf("BLE_EVT_GAP_PASSKEY_REQUEST: conn_idx: %04x\r\n", evt->conn_idx);
}
static void handle_evt_gap_numeric_request(ble_evt_gap_numeric_request_t *evt)
{
        uart_printf("BLE_EVT_GAP_NUMERIC_REQUEST: conn_idx: %04x passkey: %u\r\n",
                                                                evt->conn_idx, evt->num_key);
}
static void handle_evt_gap_sec_level_changed(ble_evt_gap_sec_level_changed_t *evt)
{
        uart_printf("BLE_EVT_GAP_SEC_LEVEL_CHANGED: conn_idx: %04x level:%d\r\n",
                                                                evt->conn_idx,evt->level);
}
static void handle_evt_gap_addr_resolved(ble_evt_gap_address_resolved_t *evt)
{
        uart_printf("BLE_EVT_GAP_ADDR_RESOLVED: conn_idx: %04x addr: %s resolved_addr: %s\r\n",
                evt->conn_idx, format_bd_address(&evt->address), format_bd_address(&evt->resolved_address));
}
static void handle_evt_gap_peer_features(ble_evt_gap_peer_features_t *evt)
{
        uart_printf("BLE_EVT_GAP_PEER_FEATURES: \tconn_idx:%04x features:%02x\r\n",
                                                        evt->conn_idx, evt->le_features);
}
static void handle_evt_gap_disconnect_failed(ble_evt_gap_disconnect_failed_t *evt)
{
        uart_printf("ERROR: DISCONNECT FAILED\r\n");
}
static void handle_evt_gap_disconnected(ble_evt_gap_disconnected_t *evt)
{

        if (peer_info.suota_client) {
                ble_client_cleanup(peer_info.suota_client);
        }

        if (peer_info.dis_client) {
                ble_client_cleanup(peer_info.dis_client);
        }

        memset(&peer_info, 0, sizeof(peer_info));
        peer_info.conn_idx = BLE_CONN_IDX_INVALID;

        app_state = APP_STATE_IDLE;

        uart_printf("EVT_DISCONNECTED - ADDR: %s, CONN_ID: %d REASON: %02x\r\n",
                                        format_bd_address(&evt->address), evt->conn_idx, evt->reason);
}

static void handle_evt_scan_completed(ble_evt_gap_scan_completed_t * evt)
{
        uart_printf("EVT_SCAN_COMPLETED: %02x\r\n", evt->status);
}
static void handle_evt_gap_connected(ble_evt_gap_connected_t *evt)
{
        gap_sec_level_t *level = OS_MALLOC(sizeof(gap_sec_level_t));
        ble_error_t err;
        gap_device_t device;

        peer_info.addr = evt->peer_address;
        peer_info.conn_idx = evt->conn_idx;
        peer_info.patch_data_char_size = 120;
        peer_info.pending_init = 0;

        app_state = APP_STATE_CONNECTED;

        ble_gattc_exchange_mtu(evt->conn_idx);

        err = ble_gap_get_device_by_conn_idx(evt->conn_idx, &device);
        uart_printf("EVT_CONNECTED\r\n");

        uart_printf("\tADDR: %s, ADDR_TYPE: %d CONN_ID: %d \r\n", format_bd_address(&evt->peer_address),
                                                                evt->peer_address.addr_type, evt->conn_idx);

        if(err == BLE_STATUS_OK){
                uart_printf("EVT_BONDED: %d MITM: %d SEC: %d\r\n",
                                                device.bonded, device.mitm, device.secure);
        }else
        {
                uart_printf("ERROR: GET DEVICE SECURITY %02x\r\n", err);
        }

        if(device.bonded)
        {
                gap_sec_level_t level;
                if(device.secure)
                {
                        level = GAP_SEC_LEVEL_4;
                        err =  ble_gap_set_sec_level(evt->conn_idx, level);
                }else if(!device.secure && device.mitm)
                {
                        level = GAP_SEC_LEVEL_3;
                        err =  ble_gap_set_sec_level(evt->conn_idx, level);
                }else
                {
                        level = GAP_SEC_LEVEL_2;
                        err =  ble_gap_set_sec_level(evt->conn_idx, level);
                }

                uart_printf("EVT_BLE_GAP_SET_SEC_LEVEL: level: %d err: %d\r\n", level, err);
        }



        OS_FREE(level);
}

static void handle_evt_gap_connect_complete(ble_evt_gap_connection_completed_t * evt)
{

        /* Successful connections are handled in separate event */
        if (evt->status == BLE_STATUS_OK) {
                return;
        }

        uart_printf("EVT_CONNECT_FAILED: %02x\r\n", evt->status);
        app_state = APP_STATE_IDLE;
}
static void handle_evt_gap_adv_report(ble_evt_gap_adv_report_t *evt)
{

        uart_printf("EVT_ADV - ADDR: %s, ADDR_TYPE: %02x, ADV_TYPE: %02x, RSSI: %d, DATA:  ",
                format_bd_address(&evt->address), evt->address.addr_type, evt->type, evt->rssi );

        uint8_t i;
        for(i = 0; i < evt->length ; i++)
        {
                uart_printf("%02x ", (int)evt->data[i]);
        }

        uart_printf("\r\n");
}

void ble_central_task(void *params)
{

        msg_queue_create(&at_cmd_queue, 5, DEFAULT_OS_ALLOCATOR);

        nvms = peripheral_cmd_init();
        /* Start main task here (text menu available via UART1 to control application) */
        command_parser_start_task(mainCOMMAND_PARSER_PRIORITY, OS_GET_CURRENT_TASK());

        ble_register_app();
        ble_central_start();

        ble_gap_set_io_cap(GAP_IO_CAP_KEYBOARD_DISP);

        ble_gap_mtu_size_set(512);


        for (;;) {
                OS_BASE_TYPE ret;
                uint32_t notif;

                /*
                 * Wait on any of the notification bits, then clear them all
                 */
                ret = OS_TASK_NOTIFY_WAIT(0, OS_TASK_NOTIFY_ALL_BITS, &notif, OS_TASK_NOTIFY_FOREVER);
                /* Blocks forever waiting for task notification. The return value must be OS_OK */
                OS_ASSERT(ret == OS_OK);

                /* resume watchdog */
                //sys_watchdog_notify_and_resume(wdog_id);

                /* notified from BLE manager, can get event */
                if (notif & BLE_APP_NOTIFY_MASK) {
                        ble_evt_hdr_t *hdr;

                        hdr = ble_get_event(false);
                        if (!hdr) {
                                goto no_event;
                        }

                        if (!ble_service_handle_event(hdr)) {
                                switch (hdr->evt_code) {
                                case BLE_EVT_GAP_CONNECTED:
                                        handle_evt_gap_connected((ble_evt_gap_connected_t *) hdr);
                                        break;
                                case BLE_EVT_GAP_CONNECTION_COMPLETED:
                                        handle_evt_gap_connect_complete((ble_evt_gap_connection_completed_t *) hdr);
                                        break;
                                case BLE_EVT_GAP_DISCONNECTED:
                                        handle_evt_gap_disconnected((ble_evt_gap_disconnected_t *) hdr);
                                        break;
                                case BLE_EVT_GAP_DISCONNECT_FAILED:
                                        handle_evt_gap_disconnect_failed((ble_evt_gap_disconnect_failed_t *) hdr);
                                        break;
                                case BLE_EVT_GAP_SECURITY_REQUEST:
                                        handle_evt_gap_security_request((ble_evt_gap_security_request_t *) hdr);
                                        break;
                                case BLE_EVT_GAP_SCAN_COMPLETED:
                                        handle_evt_scan_completed((ble_evt_gap_scan_completed_t *) hdr);
                                        break;
                                case BLE_EVT_GAP_PAIR_COMPLETED:
                                        handle_evt_gap_pair_completed((ble_evt_gap_pair_completed_t *) hdr);
                                        break;
                                case BLE_EVT_GAP_PASSKEY_NOTIFY:
                                        handle_evt_gap_passkey_notify((ble_evt_gap_passkey_notify_t *) hdr);
                                        break;
                                case BLE_EVT_GAP_PASSKEY_REQUEST:
                                        handle_evt_gap_passkey_request((ble_evt_gap_passkey_request_t *)hdr);
                                        break;
                                case BLE_EVT_GAP_NUMERIC_REQUEST:
                                        handle_evt_gap_numeric_request((ble_evt_gap_numeric_request_t *)hdr);
                                        break;
                                case BLE_EVT_GAP_SEC_LEVEL_CHANGED:
                                        handle_evt_gap_sec_level_changed((ble_evt_gap_sec_level_changed_t *)hdr);
                                        break;
                                case BLE_EVT_GAP_ADDRESS_RESOLVED:
                                        handle_evt_gap_addr_resolved((ble_evt_gap_address_resolved_t *)hdr);
                                        break;
                                case BLE_EVT_GAP_PEER_FEATURES:
                                        handle_evt_gap_peer_features((ble_evt_gap_peer_features_t *)hdr);
                                        break;
                                case BLE_EVT_GAP_PEER_VERSION:
                                        handle_evt_gap_peer_version((ble_evt_gap_peer_version_t *)hdr);
                                        break;
                                case BLE_EVT_GATTC_MTU_CHANGED:
                                        handle_evt_mtu_changed((ble_evt_gattc_mtu_changed_t *)hdr);
                                        break;
                                case BLE_EVT_GAP_DATA_LENGTH_CHANGED:
                                        handle_evt_gap_data_length_changed((ble_evt_gap_data_length_changed_t *)hdr);
                                        break;
                                case BLE_EVT_GATTC_BROWSE_SVC:
                                        handle_evt_gattc_browse_svc((ble_evt_gattc_browse_svc_t *) hdr);
                                        break;
                                case BLE_EVT_GATTC_BROWSE_COMPLETED:
                                        handle_evt_gattc_browse_completed((ble_evt_gattc_browse_completed_t *) hdr);
                                        break;

                                case BLE_EVT_GATTC_DISCOVER_SVC:
                                        //handle_evt_gattc_discover_svc((ble_evt_gattc_discover_svc_t *) hdr);
                                        break;
                                case BLE_EVT_GATTC_DISCOVER_CHAR:
                                        handle_evt_gattc_discover_char((ble_evt_gattc_discover_char_t *) hdr);
                                        break;
                                case BLE_EVT_GATTC_DISCOVER_DESC:
                                        //handle_evt_gattc_discover_desc((ble_evt_gattc_discover_desc_t *) hdr);
                                        break;
                                case BLE_EVT_GATTC_DISCOVER_COMPLETED:
                                        //handle_evt_gattc_discover_completed((ble_evt_gattc_discover_completed_t *) hdr);
                                        break;
                                case BLE_EVT_GATTC_READ_COMPLETED:
                                        handle_evt_gattc_read_completed((ble_evt_gattc_read_completed_t *) hdr);
                                        break;
                                case BLE_EVT_GATTC_WRITE_COMPLETED:
                                        handle_evt_gattc_write_completed((ble_evt_gattc_write_completed_t *) hdr);
                                        break;
                                case BLE_EVT_GATTC_NOTIFICATION:
                                        handle_evt_gattc_notification((ble_evt_gattc_notification_t *) hdr);
                                        break;
                                case BLE_EVT_GATTC_INDICATION:
                                        handle_evt_gattc_indication(((ble_evt_gattc_indication_t *) hdr));
                                        break;
                                case BLE_EVT_GAP_ADV_REPORT:
                                        handle_evt_gap_adv_report((ble_evt_gap_adv_report_t *) hdr);
                                        break;
                                case BLE_EVT_GAP_CONN_PARAM_UPDATE_REQ:
                                        handle_evt_gap_update_req((ble_evt_gap_conn_param_update_req_t *) hdr);
                                        break;
                                case BLE_EVT_GAP_CONN_PARAM_UPDATED:
                                        handle_evt_gap_update_param_completed((ble_evt_gap_conn_param_updated_t*) hdr);
                                        break;
                                case BLE_EVT_L2CAP_CONNECTED:
                                        handle_evt_l2cap_connected((ble_evt_l2cap_connected_t *) hdr);
                                        break;
                                case BLE_EVT_L2CAP_CONNECTION_FAILED:
                                        handle_evt_l2cap_connection_failed((ble_evt_l2cap_connection_failed_t *) hdr);
                                        break;
                                case BLE_EVT_L2CAP_DISCONNECTED:
                                        handle_evt_l2cap_disconnected((ble_evt_l2cap_disconnected_t *) hdr);
                                        break;
                                case BLE_EVT_L2CAP_SENT:
                                        handle_evt_l2cap_sent((ble_evt_l2cap_sent_t *) hdr);
                                        break;
                                case BLE_EVT_L2CAP_REMOTE_CREDITS_CHANGED:
                                        handle_evt_l2cap_remote_credits_changed((ble_evt_l2cap_credit_changed_t *) hdr);
                                        break;
                                case BLE_EVT_L2CAP_DATA_IND:
                                        handle_evt_l2cap_data_ind((ble_evt_l2cap_data_ind_t *) hdr);
                                        break;
                                default:
                                        uart_printf("default: %04x\r\n", hdr->evt_code);
                                        ble_handle_event_default(hdr);
                                        break;
                                }
                        }

                        ble_client_handle_event(hdr);

                        OS_FREE(hdr);

no_event:
                        // notify again if there are more events to process in queue
                        if (ble_has_event()) {
                                OS_TASK_NOTIFY(OS_GET_CURRENT_TASK(), BLE_APP_NOTIFY_MASK, eSetBits);
                        }


                }

                if(notif & AT_CMD_AVAILABLE)
                {
                        msg *ble_msg = OS_MALLOC(sizeof(msg));
                        msg_queue_get(&at_cmd_queue, ble_msg, OS_QUEUE_FOREVER);
                        switch(ble_msg->type)
                        {
                        case GAPSCAN:
                                handle_gap_scan_request((gap_scan_cmd *)ble_msg->data);
                                break;
                        case GAPCONNECT:
                                handle_gap_connect((gap_connect_cmd *)ble_msg->data);
                                break;
                        case GAPDISC:
                                handle_gap_disc((gap_disc_cmd *)ble_msg->data);
                                break;
                        case GATTBROWSE:
                                handle_gatt_browse_cmd((gatt_browse_cmd *)ble_msg->data);
                                break;
                        case GAPSETCP:
                                handle_gap_set_cp((gap_set_cp_cmd *)ble_msg->data);
                                break;
                        case GAPCANCELCONNECT:
                                handle_gap_cancel_conn_cmd();
                                break;
                        case GATTWRITE:
                                handle_gatt_write_cmd((gatt_write_cmd *)ble_msg->data);
                                break;
                        case GATTWRITENORESP:
                                handle_gatt_write_no_rsp_cmd((gatt_write_cmd *)ble_msg->data);
                                break;
                        case GATTREAD:
                                handle_gatt_read_cmd((gatt_read_cmd *)ble_msg->data);
                                break;
                        case GAPSETIOCAP:
                                handle_gap_set_io_cmd((gap_set_io_cap_cmd *)ble_msg->data);
                                break;
                        case GAPPAIR:
                                handle_gap_pair((gap_pair_cmd *)ble_msg->data);
                                break;
                        case PASSKEYENTRY:
                                handle_gap_pk_reply((gap_pk_reply_cmd_t *)ble_msg->data);
                                break;
                        case YESNOENTRY:
                                handle_gap_yes_no_reply((gap_yes_no_reply_cmd_t *)ble_msg->data);
                                break;
                        case IMGBLOCKUPDATE:
                                handle_img_block_update((write_img_block_cmd_t *)ble_msg->data);
                                break;
                        case IMGERASEALL:
                                handle_image_erase_cmd();
                                break;
                        case IMGCHECK:
                                peripheral_handle_suota_check();
                                break;
                        case SUOTASTART:
                                handle_suota_start((suota_start_cmd_t *)ble_msg->data);
                                break;
                        case CLEARBOND:
                                handle_clear_bond((clear_bond_cmd_t *)ble_msg->data);
                                break;
                        case GETBONDS:
                                handle_get_bonds();
                                break;
                        default:
                                handle_set_baud_rate((set_baud_cmd *)ble_msg->data);
                                break;
                        }

                        msg_release(ble_msg);

                        OS_FREE(ble_msg);

                }



        }


}

