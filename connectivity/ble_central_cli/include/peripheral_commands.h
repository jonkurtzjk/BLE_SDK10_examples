#ifndef PERIPHERAL_COMMANDS_H_
#define PERIPHERAL_COMMANDS_H_

#include "hw_uart.h"
#include "ad_nvms.h"
#include "suota.h"
#include "suota_client.h"

#define MAX_ENCODED_b64         (1369) //1024 bytes encoded plus null terminator
#define MAX_IMG_BLOCK_SIZE      (1024)

typedef struct{
        HW_UART_BAUDRATE baud;
        HW_UART_ID id;
}set_baud_cmd;

typedef struct
{
        char            img_block_b64[MAX_ENCODED_b64];
        uint16_t        data_len;
        uint32_t        addr_wr;
        uint16_t        crc16;
        bool            write_to_flash;

}write_img_block_cmd_t;

typedef struct
{
        bool force_gatt_update;
}suota_start_cmd_t;



void handle_set_baud_rate(set_baud_cmd *evt);

void handle_img_block_update(write_img_block_cmd_t *evt);

void handle_image_erase_cmd(void);

nvms_t peripheral_cmd_init(void);

void peripheral_handle_suota_check(void);

size_t peripheral_get_image_size(void);

bool peripheral_check_image_signature(suota_1_1_image_header_da1469x_t *hdr);

void peripheral_display_fw_version(void);

#endif
