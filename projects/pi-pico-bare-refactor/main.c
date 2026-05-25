#include "pico/stdlib.h"
#include "blink.h"

int main(void) {
    stdio_init_all();

    // run the init thread once to initialize hardware
    blink_init(1);

    // run the blink thread periodically at 500ms
    while (1) {
        blink_blink(500000000);
        sleep_ms(250);
    }

    return 0;
}
