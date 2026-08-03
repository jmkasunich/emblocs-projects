#include "platform.h"
#include <pico/stdlib.h>
#include <hardware/timer.h>
#include <hardware/irq.h>
#include "blink.h"
#include <stdio.h>

static uint32_t alarm_target = 0;
static uint32_t slow_thread_irq = 0;

#define THREAD_ALARM_NUM   0
#define THREAD_ALARM_IRQ   TIMER_IRQ_0
#define THREAD_PERIOD_US   500   // 2 kHz
#define THREAD_PERIOD_NS   (THREAD_PERIOD_US*1000)

void slow_handler(void)
{
    blink_blink(500000000);
    printf("slow");
}

void alarm_handler(void)
{
    static int n = 0;

    // clear previous interrupt
    hw_clear_bits(&timer_hw->intr, 1u << THREAD_ALARM_NUM);
    // set next interrupt time
    alarm_target += THREAD_PERIOD_US;
    timer_hw->alarm[THREAD_ALARM_NUM] = alarm_target;
    blink_toggle(THREAD_PERIOD_NS);
    if ( n++ >= 1000 ) {
        irq_set_pending(slow_thread_irq);
        n = 0;
    }
}


int main(void)
{
    platform_init();

    printf("hello, world\n");


    // run the init thread once to initialize hardware
    blink_init(1);
    // set up realtime thread triggers
    // slow thread first - triggered by fast thread divide by N
    slow_thread_irq = user_irq_claim_unused(true);
    irq_set_exclusive_handler(slow_thread_irq, slow_handler);
    irq_set_priority(slow_thread_irq, SECOND_THREAD_PRIORITY);
    irq_set_enabled(slow_thread_irq, true);
    // now fast thread - triggered by system alarm timer
    hardware_alarm_claim(THREAD_ALARM_NUM);
    irq_set_exclusive_handler(THREAD_ALARM_IRQ, alarm_handler);
    irq_set_priority(THREAD_ALARM_IRQ, HIGHEST_THREAD_PRIORITY);
    hw_clear_bits(&timer_hw->intr, 1u << THREAD_ALARM_NUM);
    hw_set_bits(&timer_hw->inte, 1u << THREAD_ALARM_NUM);
    irq_set_enabled(THREAD_ALARM_IRQ, true);
    // read alarm timer
    alarm_target = timer_hw->timerawl;
    alarm_target += THREAD_PERIOD_US;
    timer_hw->alarm[THREAD_ALARM_NUM] = alarm_target;
    // run the background thread
    while (1) {
        sleep_ms(500);
        printf(".");
    }
    return 0;
}
