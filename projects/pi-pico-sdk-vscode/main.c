#include <stdio.h>
#include "pico/stdlib.h"
#include "blink.h"


int main()
{
    stdio_init_all();

    blink_init(1);

    while (true) {
        printf("Hello, world!\n");
        blink_blink(500000000);
        sleep_ms(500);
    }
}
