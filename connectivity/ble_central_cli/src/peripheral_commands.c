#include <string.h>
#include <stdio.h>
#include <osal.h>
#include <resmgmt.h>
#include "peripheral_commands.h"
#include "ad_uart.h"
#include "hw_uart.h"
#include "uart_driver_custom.h"
#include "ad_flash.h"
#include "ad_nvms.h"
#include "sdk_crc16.h"
#include "suota.h"
#include "time.h"
#include "suota_client.h"


__RETAINED uint8_t                   image_sector[FLASH_SECTOR_SIZE];
__RETAINED volatile     uint16_t     sector_pointer;
__RETAINED uint16_t                  crc;
__RETAINED nvms_t                    bin_part;
__RETAINED static size_t             img_size;

static __attribute__((optimize("O0"))) int b64_decode(char *b64src, uint16_t *clrdst) {
   int phase, i;
   unsigned char in[8], c;
   int numItems = 0;

   phase = 0; i=0;
   while(b64src[i]) {
      c = (unsigned char)b64src[i];
      if(c == '=') {
         /*
          * We make sure at the Terminal end that data is always a multiple of 16-bits
          * and so we only ever have multiple of 2 bytes. This catches the 2 or 4 byte
          * case
          */
         *clrdst++ = (((in[0] << 2) & 0xFC) | ((in[1] >> 4) & 0x03)) | ((((in[1] << 4) & 0xF0) | ((in[2] >> 2) & 0x0F))<<8);
         numItems++;
         if (phase > 3) {
                 *clrdst++ = (((in[2] << 6) & 0xC0) | ((in[3] >> 0) & 0x3F)) | ((((in[4] << 2) & 0xFC) | ((in[5] >> 4) & 0x03))<<8);
                 numItems++;
         }
         break;
      }

      // Find the index of c
      if (c>= 'a')
         c = c-'a'+26;
      else if (c>='A')
         c = c-'A'+0;
      else if (c>='0')
         c = c-'0'+52;
      else if (c == '+')
         c = 62;
      else
         c = 63;

      in[phase] = c;
      phase = (phase + 1) & 0x07;
      if (phase == 0) {
        *clrdst++ = (((in[0] << 2) & 0xFC) | ((in[1] >> 4) & 0x03)) | ((((in[1] << 4) & 0xF0) | ((in[2] >> 2) & 0x0F))<<8);
        *clrdst++ = (((in[2] << 6) & 0xC0) | ((in[3] >> 0) & 0x3F)) | ((((in[4] << 2) & 0xFC) | ((in[5] >> 4) & 0x03))<<8);
        *clrdst++ = (((in[5] << 4) & 0xF0) | ((in[6] >> 2) & 0x0F)) | ((((in[6] << 6) & 0xC0) | ((in[7] >> 0) & 0x3F))<<8);
        numItems += 3;
      }
      i++;
   }
   return numItems;
}

int update_flash_image(uint32_t addr)
{
     int ret =  ad_nvms_write(bin_part, addr, (const uint8_t *)image_sector, sector_pointer);
     sector_pointer = 0;
     memset(image_sector, 0xff, FLASH_SECTOR_SIZE);

     return ret;
}

void handle_set_baud_rate(set_baud_cmd *evt)
{
        uart_printf("OK\r\n");
        OS_DELAY(OS_TIME_TO_TICKS(100));
        evt->id = uart_drv_get_id();
        hw_uart_baudrate_set(evt->id, evt->baud);
}

int num_items_debug;
void handle_img_block_update(write_img_block_cmd_t *evt)
{
        int num_items;
        uint16_t img_block[MAX_IMG_BLOCK_SIZE];

        uint32_t addr_sanity_check = evt->addr_wr % FLASH_SECTOR_SIZE;

        if(addr_sanity_check == sector_pointer){
                num_items = b64_decode(evt->img_block_b64, img_block);
                num_items_debug = num_items;

                if(evt->data_len == num_items*2){
                        crc = crc16_calculate((const uint8_t *)img_block, evt->data_len);
                        if(crc == evt->crc16)
                        {
                                memcpy(&image_sector[sector_pointer], img_block, evt->data_len);
                                sector_pointer += evt->data_len;
                                if(evt->write_to_flash){
                                        uint32_t addr = evt->addr_wr - sector_pointer + evt->data_len;

                                        if(update_flash_image(addr) >= 0)
                                        {
                                                uart_printf("OK\r\n");
                                        }else
                                        {
                                                uart_printf("ERROR: FLASH_WRITE\r\n");
                                        }
                                }else
                                {
                                        if(sector_pointer > (FLASH_SECTOR_SIZE))
                                        {
                                                memset(image_sector, 0xff, FLASH_SECTOR_SIZE);
                                                sector_pointer = 0;
                                                uart_printf("ERROR: BUFFER_FULL\r\n");
                                        }
                                        else
                                        {
                                                uart_printf("OK\r\n");
                                        }
                                }
                        }else
                        {
                                uart_printf("ERROR: CRC\r\n");
                        }

                }
                else
                {
                        uart_printf("ERROR: Invalid elements\r\n");
                }
        }else
        {
                uart_printf("ERROR: Sector Pointer \r\n");
        }

}
void handle_image_erase_cmd(void)
{
        size_t region_size = ad_nvms_get_size(bin_part);
        int ret = ad_nvms_erase_region(bin_part, 0, region_size);
        sector_pointer = 0;
        memset(image_sector, 0, FLASH_SECTOR_SIZE);

        if(ret >= 0)
        {
                uart_printf("OK\r\n");
                img_size = 0;
        }else
        {
                uart_printf("ERROR\r\n");
        }
}
nvms_t peripheral_cmd_init(void)
{
        memset(image_sector, 0xff, FLASH_SECTOR_SIZE);
        sector_pointer = 0;

        img_size = 0;

        bin_part = ad_nvms_open(NVMS_BIN_PART);

        return bin_part;
}

bool peripheral_check_image_signature(suota_1_1_image_header_da1469x_t *hdr)
{

        /* Read header and check if update image is valid */
        ad_nvms_read(bin_part, 0, (void *)hdr, sizeof(suota_1_1_image_header_da1469x_t));

        if (hdr->FIELD(signature, image_identifier)[0] != HEADER_SIGNATURE_B1 ||
                        hdr->FIELD(signature, image_identifier)[1] != HEADER_SIGNATURE_B2) {
                return false;
        }

        /* Store total image size to be transferred */
        img_size = hdr->FIELD(exec_location, pointer_to_ivt) + hdr->FIELD(code_size, size);

        return true;

}
void peripheral_handle_suota_check(void)
{
        suota_1_1_image_header_da1469x_t header;
        time_t rawtime;
        struct tm *timeinfo;


        if(!peripheral_check_image_signature(&header))
        {
                uart_printf("ERROR: No Valid Image\r\n");
                return;
        }



        uart_printf("OK\r\n");


        uart_printf("FW Image size: %u bytes\r\n", img_size);

        /* Dump image info */
        rawtime = header.timestamp;
        timeinfo = gmtime(&rawtime);

        uart_printf("FW Image information:\r\n");
        uart_printf("\tCode Size: %d bytes\r\n", header.FIELD(code_size, size));
        uart_printf("\tVersion: %.*s\r\n", sizeof(header.FIELD(version, version_string)),
                                                        header.FIELD(version, version_string));
        if (timeinfo) {
                uart_printf("\tTimestamp: %04d-%02d-%02d %02d:%02d:%02d UTC\r\n",
                                timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
                                timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
        } else {
                uart_printf("\tTimestamp: INVALID TIMESTAMP\r\n");
        }
        uart_printf("\tCRC: 0x%08x\r\n", (unsigned int)header.crc);
        uart_printf("END IMAGE\r\n");
}

size_t peripheral_get_image_size(void)
{
        return img_size;
}
