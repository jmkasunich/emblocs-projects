#include "platform.h"
#include "blink.h"
#include <stdio.h>

#include "stm32g4xx_ll_bus.h"
#include "stm32g4xx_ll_tim.h"


void delay_us(unsigned int us) {
    unsigned int elapsed;
    unsigned int start = tsc_read();
    unsigned int target = us * CLK_MHZ;
    do {
        elapsed = tsc_read() - start;
    } while (elapsed < target);
}

#if 0
void print_string(const char *str)
{
    const char *cp = str;
    while (*cp != '\0' ) {
        bdl_string_put_bl(&monitor_tx, *cp++);
    }
}
#endif

#define THREAD_TIMER         TIM7
#define THREAD_ISRn          TIM7_IRQn
#define THREAD_IRQ_HANDLER   TIM7_IRQHandler

#define THREAD1_ISRn         COMP4_5_6_IRQn
#define THREAD1_IRQ_HANDLER  COMP4_5_6_IRQHandler


void THREAD_IRQ_HANDLER(void)
{
    static int n = 0;
    LL_TIM_ClearFlag_UPDATE(THREAD_TIMER);   // clear the UIF the update event just set
    blink_toggle(50000);
    if ( n++ >= 1000) {
        n = 0;
        NVIC_SetPendingIRQ(THREAD1_ISRn);
    }
}

void THREAD1_IRQ_HANDLER (void)
{
    blink_blink(500000000);
    printf("slow");
}


int main(void) {
    platform_init();
    printf("hello, world\n");

    // run init thread once
    blink_init(1);

    NVIC_SetPriority(THREAD1_ISRn, 5);
    NVIC_EnableIRQ(THREAD1_ISRn);

    // Enable TIM7 peripheral clock
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM7);
    // Configure prescaler and auto-reload
    LL_TIM_SetPrescaler(THREAD_TIMER, 169);  // divide by 170, result is 1MHz
    LL_TIM_SetAutoReload(THREAD_TIMER, 499);  // divide by 500, result 2000Hz
    // Force the prescaler/ARR to load immediately
    LL_TIM_GenerateEvent_UPDATE(THREAD_TIMER);
    LL_TIM_ClearFlag_UPDATE(THREAD_TIMER);   // clear the UIF the update event just set
    // Enable the update interrupt and NVIC line
    LL_TIM_EnableIT_UPDATE(THREAD_TIMER);
    NVIC_SetPriority(THREAD_ISRn, 4);
    NVIC_EnableIRQ(THREAD_ISRn);
    // Start counting
    LL_TIM_EnableCounter(TIM7);
    printf("starting");

    while (1) {
        delay_us(500000);  // one-half second
        printf(".");       // just to show we're alive
    }
    return 0;
}