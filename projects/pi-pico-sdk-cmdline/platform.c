/***************************************************************
 * 
 * platform.c - platform specific code for RP2040
 * 
 * 
 * 
 * *************************************************************/
#include "platform.h"
#include <bundle.h>

#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#include <pico/stdio.h>
#include <pico/stdio/driver.h>

#include <hardware/uart.h>
#include <hardware/irq.h>
#include <hardware/gpio.h>

#define MONITOR_UART    uart0

static uint8_t monitor_rx_buf[MONITOR_RX_STRING_BUF_SIZE];
static uint8_t monitor_tx_buf[MONITOR_TX_STRING_BUF_SIZE];

void Monitor_UART_IRQHandler(void);
void Monitor_UART_start_tx(void);


bdl_rx_t monitor_rx;
const bdl_rx_config_t monitor_rx_config = {
    .string_buf          = monitor_rx_buf,
    .string_buf_size     = sizeof(monitor_rx_buf),
    .string_avail        = NULL,
    .crc16               = bdl_crc16_lookup,
};

bdl_tx_t monitor_tx;
const bdl_tx_config_t monitor_tx_config = {
    .string_buf          = monitor_tx_buf,
    .string_buf_size     = sizeof(monitor_tx_buf),
    .string_not_full     = NULL,
    .crc16               = bdl_crc16_lookup,
    .tx_bytes_available  = Monitor_UART_start_tx,
};

/* console serial port
 *
 * The 'console' is just a UART, but on an MCU with multiple UARTs, it is the 
 * one that is used for debugging, etc.  Other UART functions likely require 
 * passing a handle or something to identify which UART; the console functions
 * hard-code the UART handle for convenience and speed.
 */


static void bdl_out_chars(const char *buf, int len)
{
    for (int i = 0; i < len; i++) {
        if (!bdl_string_put_nb(&monitor_tx, (uint8_t)buf[i])) {
            break;   // queue full — drop remainder
        }
    }
}

static stdio_driver_t monitor_uart_stdio_driver = {
    .out_chars = bdl_out_chars,
    .out_flush = NULL,     // no-op is fine; nothing to flush with a queue+ISR design
    .in_chars  = NULL,     // add later if you want stdin support
#if PICO_STDIO_ENABLE_CRLF_SUPPORT
    .crlf_enabled = false,
#endif
};



/* platform_init() performs core initialization
 *
 *    initializes clock subsystem, sets all clocks to full speed
 *    initializes console serial port
 *    initializes time-stamp counter
 */

void platform_init(void)
{
    /* Console UART Configuration
     *      Console UART      = UART0
     *      Baud rate         = 115200
     *      Mode              = 8N1
     */

    uart_init(uart0, 115200);
    uart_set_format(uart0, 8, 1, UART_PARITY_NONE);
    gpio_set_function(0, GPIO_FUNC_UART);   // GP0 -> UART TX (for uart0)
    gpio_set_function(1, GPIO_FUNC_UART);   // GP1 -> UART RX (for uart0)


    // set up bundle objects
    bdl_init_rx(&monitor_rx, &monitor_rx_config);
    bdl_init_tx(&monitor_tx, &monitor_tx_config);
    // set up interrupts
    irq_set_exclusive_handler(UART0_IRQ, Monitor_UART_IRQHandler);
    irq_set_priority(UART0_IRQ, MONITOR_UART_IRQ_PRIORITY);   // 0-255 on Cortex-M0+, fewer effective levels than G431
    irq_set_enabled(UART0_IRQ, true);
    uart_set_irq_enables(uart0, true, false);  // only RX interrupt enabled
    // enable reciever not-empty interrupt

    stdio_set_driver_enabled(&monitor_uart_stdio_driver, true);

     /* Timestamp Counter Configuration
     *      Counter           = TIM2
     *      Clock             = 170MHz
     */

    // defaults are all good, just turn it on
    // REPLACE ME - TIM2->CR1 |= TIM_CR1_CEN;
}

void Monitor_UART_IRQHandler(void)
{
    uint32_t c;

    // check for UART received data
    while (uart_is_readable(MONITOR_UART)) {
        bdl_put_rx_byte(&monitor_rx, (uint8_t)((uart_get_hw(MONITOR_UART)->dr)));
    }
    // check for UART ready to send data
    while (uart_is_writable(MONITOR_UART)) {
        c = bdl_get_tx_byte(&monitor_tx);
        if ( c <= 255 ) {
            // there is a byte to send, write it to the UART
            uart_get_hw(MONITOR_UART)->dr = (uint8_t)c;
        } else {
            // no data to send, disable TX FIFO threshold interrupt
            uart_set_irq_enables(MONITOR_UART, true, false);
            break;
        }
    }
}

void Monitor_UART_start_tx(void)
{
    // enable TX FIFO threshold interrupt to kick things off
    uart_set_irq_enables(MONITOR_UART, true, true);
}

/* time stamp counter
 *
 * used for high-resolution time measurements
 * counts at the MCU clock rate, or as close as possible
 * 
 */

 #if 0
/* tsc_read() captures the time stamp counter value */
/*   note: defined as a macro in platform_g431.h, this is here
 *         in case someone wants a pointer to the function
 */
uint32_t (tsc_read)(void)
{
    return tsc_read();
}

/* tsc_to_usec() converts time stamp counts to microseconds */
/* note that this rounds to nearest microsecond, which means small times round to zero */
uint32_t tsc_to_usec(uint32_t tsc_counts)
{
    return (tsc_counts+63)/125;
}
#endif
