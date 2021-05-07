#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <uart_driver_custom.h>
#include "hw_uart.h"
#include "osal.h"

#define UART_CUSTOM_DATABITS                            HW_UART_DATABITS_8
#define UART_CUSTOM_STOPBITS                            HW_UART_STOPBITS_1
#define UART_CUSTOM_PARITY                              HW_UART_PARITY_NONE

#define UART_INTX(id) ((id) == HW_UART1 ? (UART_IRQn) : ((id) == HW_UART2 ? (UART2_IRQn) : (UART3_IRQn)))

#define UART_CLOCK_HZ                                   (32000000UL)
#define BUFF_SIZE                                       (1500)

#define PRINTF_SZ                                       (160)

/* Local data types */
typedef struct {
        uart_gpio_config_t      gpio_config;
        HW_UART_ID              uart_id;
        uint8_t                 rx_buff[BUFF_SIZE];
        uint16_t                rx_ptr;
        bool                    cr_detected;
        uart_driver_cust_cb     cb;
} uart_drv_env_t;


__RETAINED uart_drv_env_t               uart_drv_env;

PRIVILEGED_DATA volatile OS_MUTEX uart_write_mutex;


__RETAINED_CONST_INIT static uint8_t UART_FRAC_LOOKUP_TABLE[17] = {0, 6, 13, 19, 25, 31, 38, 44,
                                                        50, 56, 62, 69, 75, 81, 88, 94, 100};

__STATIC_INLINE void uart_drv_enable_rx_int(HW_UART_ID uart, bool enable)
{
        NVIC_DisableIRQ(UART_INTX(uart));
        HW_UART_REG_SETF(uart, IER_DLH, ERBFI_DLH0, enable);
        NVIC_EnableIRQ(UART_INTX(uart));
}
/**
 * \brief Calculates the integer Divisor and fractional divisor for UART baud rates
 *
 * \return bits[23...8] Integer for DLL and DLH Reg bits[7...0] Fractional divider for DLF Reg
 */
__RETAINED_CODE static uint32_t uart_calc_baudrate_divisor(uint32_t bitrate)
{
        uint32_t divisor = 0;
        uint32_t divisor_integer=0;
        uint8_t divisor_frac = 0;

        //baud rate formula * 100 to avoid float calculation of decimals
        divisor_integer = ((100*UART_CLOCK_HZ)/(16*bitrate));
        //calculate the integer version of the decimal
        uint32_t temp_frac = divisor_integer - ((divisor_integer/100)*100);
        //Shift out the decimal portion
        divisor_integer = divisor_integer/100;


        uint8_t i = 0;
        uint8_t temp_lower_bound;
        uint8_t temp_upper_bound;

        for(i=0; i<16 ;i++)
        {
                //First figure out if it is in between the values
               if(temp_frac >= UART_FRAC_LOOKUP_TABLE[i] && temp_frac < UART_FRAC_LOOKUP_TABLE[i+1])
               {
                       //Determine which value is the closest
                       temp_lower_bound = temp_frac - UART_FRAC_LOOKUP_TABLE[i];
                       temp_upper_bound = UART_FRAC_LOOKUP_TABLE[i+1] - temp_frac;

                       if(temp_lower_bound <= temp_upper_bound)
                       {
                              divisor_frac = i;
                              break;
                       }
                       else{
                               if(i == 15)
                               {
                                       //In this case we round up and add one to teh integer value
                                       divisor_frac = 0;
                                       divisor_integer +=1;
                                       break;
                               }else
                               {
                                       //upper value was the closer of the two
                                       divisor_frac = i+1;
                                       break;
                               }
                       }
               }
        }

        divisor  = (divisor_integer << 8) | divisor_frac;


        return divisor;
}

static void uart_rcv_avail(void)
{
        char temp_char;

        while (uart_drv_env.rx_ptr < BUFF_SIZE) {
                if (hw_uart_is_data_ready(uart_drv_env.uart_id)) {
                        temp_char = (char)hw_uart_rxdata_getf(uart_drv_env.uart_id);
                        uart_drv_env.rx_buff[uart_drv_env.rx_ptr++] = temp_char;

                        if(temp_char == '\r')
                        {
                              uart_drv_env.cr_detected = true;
                              break;
                        }

                } else {
                        break;
                }
        }

        if(uart_drv_env.cr_detected)
        {
                uart_drv_env.cb(UART_CUST_CR_DETECT, uart_drv_env.rx_ptr);
                uart_drv_env.cr_detected = false;
                uart_drv_enable_rx_int(uart_drv_env.uart_id, false);
        }

        if(uart_drv_env.rx_ptr == BUFF_SIZE)
        {
                uart_drv_env.cb(UART_CUST_OVERFLOW, uart_drv_env.rx_ptr);
                uart_drv_env.rx_ptr = 0;
                memset(uart_drv_env.rx_buff, 0, BUFF_SIZE);
        }
}
/**
 * \brief Handle the UART Interrupts generated
 *
 * \return void
 */

static void handle_uart_irq(void)
{
        volatile HW_UART_INT int_id;

        for(;;) {
                int_id = hw_uart_get_interrupt_id(uart_drv_env.uart_id);

                switch (int_id)
                {
                        case HW_UART_INT_TIMEOUT:
                                break;
                        case HW_UART_INT_MODEM_STAT:
                                break;
                        case HW_UART_INT_NO_INT_PEND:
                                return;
                                break;
                        case HW_UART_INT_THR_EMPTY:
                                break;
                        case HW_UART_INT_BUSY_DETECTED:
                                break;
                        case HW_UART_INT_RECEIVED_AVAILABLE:
                        {
                                uart_rcv_avail();
                        }
                        break;
                        case HW_UART_INT_RECEIVE_LINE_STAT:
                        {
                                break;
                        }


                        default:
                        {
                                ASSERT_ERROR(0);
                                break;
                        }

                }
        }
}

/**
 * \brief Custom Baud Rate set outside of SDK.  Takes any baud rate and calculates divisor and sets
 *
 * \return void
 */
__RETAINED_CODE static void uart_set_baudrate(uint32_t baud_rate)
{


        //get the appropriate divisor from the raw baud
        uint32_t new_baud = uart_calc_baudrate_divisor(baud_rate);


        // Set Divisor Latch Access Bit in LCR register to access DLL & DLH registers
        HW_UART_REG_SETF(uart_drv_env.uart_id, LCR, UART_DLAB, 1);
        // Set fraction byte of baud rate
        UBA(uart_drv_env.uart_id)->UART2_DLF_REG = 0xFF & new_baud;
        // Set low byte of baud rate
        UBA(uart_drv_env.uart_id)->UART2_RBR_THR_DLL_REG = 0xFF & (new_baud >> 8);
        // Set high byte of baud rare
        UBA(uart_drv_env.uart_id)->UART2_IER_DLH_REG = 0xFF & (new_baud >> 16);
        // Reset Divisor Latch Access Bit in LCR register
        HW_UART_REG_SETF(uart_drv_env.uart_id, LCR, UART_DLAB, 0);
        // DLAB will not reset if UART is busy, for example if UART Rx line is LOW
        // while UART_DLAB is SET. HW_UART_INT_BUSY_DETECTED could be caused
        // in different case. The caller could temporary disable any UARTx_RX GPIOs
        // to avoid LOW state on UART Rx.
        ASSERT_ERROR(HW_UART_REG_GETF(uart_drv_env.uart_id, LCR, UART_DLAB) == 0);

}
/**
  * \brief Initialize the UART block
  *
  * \return void
  */
 static void init_uart(uint32_t baud_rate)
 {
         uart_config uart_init = {
                 .data      = UART_CUSTOM_DATABITS,
                 .stop      = UART_CUSTOM_STOPBITS,
                 .parity    = UART_CUSTOM_PARITY,
                 .use_dma   = 0,
                 .use_fifo  = 1,
                 .rx_dma_channel = HW_DMA_CHANNEL_INVALID, /* DMA not used */
                 .tx_dma_channel = HW_DMA_CHANNEL_INVALID, /* DMA not used */
                 .baud_rate = HW_UART_BAUDRATE_1000000 // Configure Dummy to make SDK happy
         };

         hw_sys_pd_com_enable();


         //Configure the UART
         hw_uart_init(uart_drv_env.uart_id, &uart_init);
         //hw_uart_init sets the baud rate, but is protected from enum and passing 'odd values', reset baud rate here with lin api
         uart_set_baudrate(baud_rate);
         hw_uart_set_isr(uart_drv_env.uart_id, handle_uart_irq);

         //Let 1 character interrupt us
         hw_uart_rx_fifo_tr_lvl_setf(uart_drv_env.uart_id, 0);



         hw_gpio_set_pin_function(uart_drv_env.gpio_config.uart_tx_port,
                                                 uart_drv_env.gpio_config.uart_tx_pin,
                                                 HW_GPIO_MODE_OUTPUT,
                                                 uart_drv_env.gpio_config.uart_tx_func);

         hw_gpio_configure_pin_power(uart_drv_env.gpio_config.uart_tx_port,
                                                 uart_drv_env.gpio_config.uart_tx_pin,
                                                 HW_GPIO_POWER_V33);

         hw_gpio_set_pin_function(uart_drv_env.gpio_config.uart_rx_port,
                                                          uart_drv_env.gpio_config.uart_rx_pin,
                                                          HW_GPIO_MODE_OUTPUT,
                                                          uart_drv_env.gpio_config.uart_rx_func);

         hw_gpio_configure_pin_power(uart_drv_env.gpio_config.uart_rx_port,
                                                          uart_drv_env.gpio_config.uart_rx_pin,
                                                          HW_GPIO_POWER_V33);

         hw_gpio_pad_latch_enable(uart_drv_env.gpio_config.uart_tx_port, uart_drv_env.gpio_config.uart_tx_pin);

         hw_gpio_pad_latch_enable(uart_drv_env.gpio_config.uart_rx_port, uart_drv_env.gpio_config.uart_rx_pin);


         uart_drv_enable_rx_int(uart_drv_env.uart_id, true);

 }

 /**
  * \brief Intialize the UART driver Should be called prior to any UART API calls
  *
  * \param [in] Config   pointer to the uart configuration structure
  *
  * \return void
  */
 void uart_driver_custom_init(HW_UART_ID uart, uint32_t baud_rate, uart_gpio_config_t *config, uart_driver_cust_cb cb)
 {

         /* Store configuration for use when reconfiguring pin functions at wakeup */
         uart_drv_env.gpio_config.uart_rx_port = config->uart_rx_port;
         uart_drv_env.gpio_config.uart_rx_pin  = config->uart_rx_pin;
         uart_drv_env.gpio_config.uart_tx_port = config->uart_tx_port;
         uart_drv_env.gpio_config.uart_tx_pin  = config->uart_tx_pin;

          if (uart == HW_UART1) {
                  uart_drv_env.gpio_config.uart_tx_func = HW_GPIO_FUNC_UART_TX;
                  uart_drv_env.gpio_config.uart_rx_func  = HW_GPIO_FUNC_UART_RX;
          } else if (uart == HW_UART2) {
                  uart_drv_env.gpio_config.uart_tx_func = HW_GPIO_FUNC_UART2_TX;
                  uart_drv_env.gpio_config.uart_rx_func  = HW_GPIO_FUNC_UART2_RX;
          } else {
                  uart_drv_env.gpio_config.uart_tx_func = HW_GPIO_FUNC_UART3_TX;
                  uart_drv_env.gpio_config.uart_rx_func  = HW_GPIO_FUNC_UART3_RX;
          }


          uart_drv_env.uart_id = uart;
          uart_drv_env.cb = cb;
          init_uart(baud_rate);

          OS_MUTEX_CREATE(uart_write_mutex);

 }

 uart_drv_cust_error_t uart_drv_custom_get_data(uint8_t *rcv, uint16_t len)
 {
         uart_drv_cust_error_t err =  UART_CUST_OK;
         GLOBAL_INT_DISABLE();

         memcpy(rcv, uart_drv_env.rx_buff, len);

         GLOBAL_INT_RESTORE();

         memset(uart_drv_env.rx_buff, 0, BUFF_SIZE);
         uart_drv_env.rx_ptr = 0;
         uart_drv_enable_rx_int(uart_drv_env.uart_id, true);

         return err;
 }

 void uart_printf(const char *format, ...)
 {

         OS_MUTEX_GET(uart_write_mutex, OS_MUTEX_FOREVER);

         int written;
         char print_buffer[PRINTF_SZ];

         va_list arg;
         va_start(arg, format);

         written = vsprintf(print_buffer, format, arg);
         va_end(arg);
         hw_uart_write_buffer(uart_drv_env.uart_id, print_buffer, (uint16_t)written);

         OS_MUTEX_PUT(uart_write_mutex);


 }

 HW_UART_ID uart_drv_get_id(void)
 {
         return uart_drv_env.uart_id;
 }
