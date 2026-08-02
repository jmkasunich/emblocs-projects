/***************************************************************
 * 
 * platform.c - platform specific code for STM32G431
 * 
 * 
 * 
 * *************************************************************/
#include "platform.h"
#include "bundle.h"

#include <stdio.h>
#include <errno.h>
#include <unistd.h>


#include "stm32g4xx_ll_bus.h"
#include "stm32g4xx_ll_pwr.h"
#include "stm32g4xx_ll_rcc.h"
#include "stm32g4xx_ll_system.h"

void USART2_start_tx(void);


static uint8_t monitor_rx_buf[MONITOR_RX_STRING_BUF_SIZE];
static uint8_t monitor_tx_buf[MONITOR_TX_STRING_BUF_SIZE];

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
    .tx_bytes_available  = USART2_start_tx,
};


/* platform_init() performs core initialization
 *
 *    initializes clock subsystem, sets all clocks to full speed
 *    initializes console serial port
 *    initializes time-stamp counter
 */

void platform_init(void)
{
    uint32_t reg;

    /* System Clock Configuration
     *      System Clock source     = PLL (HSE)
     *      SYSCLK(Hz)              = 170000000
     *      HCLK(Hz)                = 170000000
     *      AHB Prescaler           = 1
     *      APB1 Prescaler          = 1
     *      APB2 Prescaler          = 1
     *      HSE Frequency(Hz)       = 8000000
     *      PLL_M                   = 2 means PLL input is 4MHz
     *      PLL_N                   = 85 means VCO output is 340MHz
     *      PLL_P                   = 2 means PLLP is 170MHz
     *      PLL_Q                   = 2 means PLLQ is 170MHz
     *      PLL_R                   = 2 means PLLR is 170MHz
     *      Flash Latency(WS)       = 4 wait states
     */

    /* Enable voltage range 1 boost mode for frequency above 150 Mhz */
    /* need to enable PWR peripheral clock first */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
    LL_PWR_EnableRange1BoostMode();
    LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_PWR);

    /* Set Flash Latency */
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_4);

    /* Running on HSI at reset, HSE must be turned on */
    LL_RCC_HSE_Enable();
    while(LL_RCC_HSE_IsReady() != 1) {};

    /* Main PLL configuration and activation */
    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_2, 85, LL_RCC_PLLR_DIV_2);
    LL_RCC_PLL_Enable();
    while(LL_RCC_PLL_IsReady() != 1) {};

    /* PLL system Output activation */
    LL_RCC_PLL_EnableDomain_SYS();

    /* Sysclk activation on the main PLL */
    /* need to run at half-speed for a bit when final speed is > 80MHz */
    /* set prescaler to 2 temporarily */
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_2);
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
    while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL) {};

    /* Insure 1µs transition state at intermediate medium speed clock based on DWT */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    DWT->CYCCNT = 0;
    while(DWT->CYCCNT < 100) {};

    /* switch to full speed; set AHB prescaler to 1 */
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);

    /* Set APB1 & APB2 prescaler*/
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

    /* GPIO Configuration
     *
     *  These pins are specific to the B-G631B-ESC1 dev board
     *   ENC_A = PB6
     *   ENC_B = PB7
     *   ENC_Z = PB8
     *   PWM = PA15
     * 
     *   LED = PC6
     *
     */

    // Turn on clocks to all GPIO modules
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIODEN;
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN;
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOFEN;
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOGEN;

    // delay for clock activation
    reg = READ_REG(RCC->AHB2ENR);
    (void)reg;

    // Put pins PB6-8 in general purpose output mode
    reg = GPIOB->MODER;
    reg &= ~GPIO_MODER_MODE6_Msk;
    reg |= 0x1 << GPIO_MODER_MODE6_Pos;
    reg &= ~GPIO_MODER_MODE7_Msk;
    reg |= 0x1 << GPIO_MODER_MODE7_Pos;
    reg &= ~GPIO_MODER_MODE8_Msk;
    reg |= 0x1 << GPIO_MODER_MODE8_Pos;
    GPIOB->MODER = reg;
    // likewise for PA15
    reg = GPIOA->MODER;
    reg &= ~GPIO_MODER_MODE15_Msk;
    reg |= 0x1 << GPIO_MODER_MODE15_Pos;
    GPIOA->MODER = reg;
    // and PC6
    reg = GPIOC->MODER;
    reg &= ~GPIO_MODER_MODE6_Msk;
    reg |= 0x1 << GPIO_MODER_MODE6_Pos;
    GPIOC->MODER = reg;

    // test them
    PB6_OUT(1);
    PB7_OUT(1);
    PB8_OUT(1);
    PA15_OUT(1);
    PB6_OUT(0);
    PB7_OUT(0);
    PB8_OUT(0);
    PA15_OUT(0);

    // turn on clock to timestamp counter (TIM2)
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;

    /* Console UART Configuration
     *      Console UART      = USART2
     *      Baud rate         = 115200
     *      Mode              = 8N1
     */

    // enable UART clock
    SET_BIT(RCC->APB1ENR1, RCC_APB1ENR1_USART2EN);
    // Put pins PB3 and PB4 in alternate function mode
    reg = GPIOB->MODER;
    reg &= ~GPIO_MODER_MODE3_Msk;
    reg |= 0x2 << GPIO_MODER_MODE3_Pos;
    reg &= ~GPIO_MODER_MODE4_Msk;
    reg |= 0x2 << GPIO_MODER_MODE4_Pos;
    GPIOB->MODER = reg;
    // select alternate function 7
    reg = GPIOB->AFR[0];
    reg &= ~GPIO_AFRL_AFSEL3_Msk;
    reg |= 7 << GPIO_AFRL_AFSEL3_Pos;
    reg &= ~GPIO_AFRL_AFSEL4_Msk;
    reg |= 7 << GPIO_AFRL_AFSEL4_Pos;
    GPIOB->AFR[0] = reg;
    // delay for clock activation
    reg = READ_REG(RCC->APB1ENR1);
    (void)reg;
    // configure the UART
    USART2->CR1 = 0; // all bits in default mode, UART still disabled
    USART2->CR2 = 0; // all bits in default mode
    USART2->CR3 = 0; // all bits in default mode
    USART2->CR1 |= USART_CR1_FIFOEN; // enable hardware FIFO
    USART2->CR3 |= (0x2u << USART_CR3_TXFTCFG_Pos); // set TX FIFO threshold to 50%
    // set baud rate
    // we have 170MHz clock and want 115200 baud
    //   170,000,000 / 115,200 = 1475.69, so use 1476
    USART2->BRR = 1476;
    // enable the UART
    USART2->CR1 |= USART_CR1_UE;
    // enable reciever and transmitter
    USART2->CR1 |= USART_CR1_TE | USART_CR1_RE;
    // set up bundle objects
    bdl_init_rx(&monitor_rx, &monitor_rx_config);
    bdl_init_tx(&monitor_tx, &monitor_tx_config);
    // enable reciever not-empty interrupt
    USART2->CR1 |= USART_CR1_RXNEIE_RXFNEIE;
    // enable UART master interrupt
    NVIC_SetPriority(USART2_IRQn, MONITOR_UART_IRQ_PRIORITY);
    NVIC_EnableIRQ(USART2_IRQn);
    // enable global interrupts
    __enable_irq();

    // tell libc that stdout and stderr are unbuffered
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    /* Timestamp Counter Configuration
     *      Counter           = TIM2
     *      Clock             = 170MHz
     */

    // defaults are all good, just turn it on
    TIM2->CR1 |= TIM_CR1_CEN;
}

/* console serial port
 *
 * The 'console' is just a UART, but on an MCU with multiple UARTs, it is the 
 * one that is used for debugging, etc.  Other UART functions likely require 
 * passing a handle or something to identify which UART; the console functions
 * hard-code the UART handle for convenience and speed.
 */

int _write(int fd, char *ptr, int len)
{
    if (fd != STDOUT_FILENO && fd != STDERR_FILENO) {
        errno = EBADF;
        return -1;
    }

    for (int i = 0; i < len; i++) {
        if (!bdl_string_put_nb(&monitor_tx, ptr[i])) {
            break;   // queue full — drop remainder, don't block
        }
    }

    return len;   // report full length regardless of drops
}

void USART2_IRQHandler(void)
{
    uint32_t c;

    PB7_OUT(1);
    // check for UART received data
    while ( (USART2)->ISR & (USART_ISR_RXNE_RXFNE_Msk) ) {
        bdl_put_rx_byte(&monitor_rx, (uint8_t)((USART2)->RDR));
    }
    // check for UART ready to send data
    while ( (USART2)->ISR & (USART_ISR_TXE_TXFNF_Msk) ) {
        PA15_OUT(1);
        c = bdl_get_tx_byte(&monitor_tx);
        PA15_OUT(0);
        if ( c <= 255 ) {
            // there is a byte to send, write it to the UART
            (USART2)->TDR = (uint8_t)c;
        } else {
            // no data to send, disable TX FIFO threshold interrupt
            (USART2)->CR3 &= ~(USART_CR3_TXFTIE);
            PB8_OUT(0);
            break;
        }
    }
    PB7_OUT(0);
}

void USART2_start_tx(void)
{
    // enable TX FIFO threshold interrupt to kick things off
    PB8_OUT(1);
    (USART2)->CR3 |= USART_CR3_TXFTIE;
}


/* returns non-zero if transmitter can accept a character */
int cons_tx_ready(void)
{
    return bdl_string_can_put(&monitor_tx);
}

/* transmits a character without waiting; data can be lost if transmitter is not ready */
void cons_tx(char c)
{
    bdl_string_put_nb(&monitor_tx, c);
}

/* waits until transmitter is ready, then transmits character */
void cons_tx_wait(char c)
{
    PB6_OUT(1);
    bdl_string_put_bl(&monitor_tx, c);
    PB6_OUT(0);
}

/* returns non-zero if transmitter is idle (all characters sent) */
int cons_tx_idle(void)
{
    return ( USART2->ISR & USART_ISR_TC_Msk );
}

/* returns non-zero if reciever has a character available */
int cons_rx_ready(void)
{
    return bdl_string_can_get(&monitor_rx);
}

/* gets a character without waiting, returns zero if no data available */
uint32_t cons_rx(void)
{
    return bdl_string_get_nb(&monitor_rx);
}

/* waits until receiver is ready, then gets character */
char cons_rx_wait(void)
{
    return bdl_string_get_bl(&monitor_rx);
}

/* time stamp counter
 *
 * used for high-resolution time measurements
 * counts at the MCU clock rate, or as close as possible
 * 
 */

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
    return (tsc_counts+85)/170;
}

