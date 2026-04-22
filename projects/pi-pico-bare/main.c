#include "pico/stdlib.h"
#include "hardware/timer.h"

// The Pico W uses a different LED pin via the wifi chip;
// this is correct for the standard Pico (non-W)
#define LED_PIN 25

// Busy-wait delay in microseconds using the RP2040 hardware timer
void delay_us(uint32_t us)
{
    uint32_t start = time_us_32();
    while ((time_us_32() - start) < us) {}
}

int main(void)
{
    // Initialize the Pico SDK
    stdio_init_all();

    // Initialize the LED pin as an output
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    while (1) {
        gpio_put(LED_PIN, 1);
        delay_us(500000);
        gpio_put(LED_PIN, 0);
        delay_us(500000);
    }

    return 0;
}
