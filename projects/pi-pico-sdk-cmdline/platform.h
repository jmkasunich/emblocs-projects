/***************************************************************
 * 
 * platform.h - platform specific code for RP2040
 * 
 * 
 * *************************************************************/

#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>

#include <bundle.h>


#define MONITOR_RX_STRING_BUF_SIZE 100
#define MONITOR_TX_STRING_BUF_SIZE 500

#define MONITOR_UART_IRQ_PRIORITY   64
#define HIGHEST_THREAD_PRIORITY     128
#define SECOND_THREAD_PRIORITY      192
#define PROFILER_PRIORITY           0


extern bdl_rx_t monitor_rx;
extern bdl_tx_t monitor_tx;

/* platform_init() performs core initialization
 *
 *    initializes clock subsystem, sets all clocks to full speed
 *    initializes console serial port
 *    initializes time-stamp counter
 */

void platform_init(void);


#define CLK_MHZ 125

/* some of the functions declared in platform.h are implemented here as macros */

#endif // PLATFORM_H
