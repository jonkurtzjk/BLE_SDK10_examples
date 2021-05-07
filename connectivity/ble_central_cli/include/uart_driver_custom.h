#ifndef UART_DRIVER_CUSTOM_
#define UART_DRIVER_CUSTOM_

#include "hw_uart.h"
#include "hw_gpio.h"

typedef struct {
        HW_GPIO_PORT    uart_rx_port;
        HW_GPIO_PIN     uart_rx_pin;
        HW_GPIO_FUNC    uart_rx_func;
        HW_GPIO_PORT    uart_tx_port;
        HW_GPIO_PIN     uart_tx_pin;
        HW_GPIO_FUNC    uart_tx_func;
} uart_gpio_config_t;

typedef enum
{
        UART_CUST_OK,
        UART_CUST_CR_DETECT,
        UART_CUST_OVERFLOW,
}uart_drv_cust_error_t;

typedef void (*uart_driver_cust_cb)(uart_drv_cust_error_t err, uint16_t len);


void                    uart_driver_custom_init(HW_UART_ID uart, uint32_t baud_rate,
                                        uart_gpio_config_t *config, uart_driver_cust_cb cb);

uart_drv_cust_error_t   uart_drv_custom_get_data(uint8_t *rcv, uint16_t len);

void                    uart_printf(const char *format, ...);

HW_UART_ID              uart_drv_get_id(void);


#endif
