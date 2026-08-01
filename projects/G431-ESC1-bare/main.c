#include "platform.h"
#include "blink.h"
#include <stdio.h>

void delay_us(unsigned int us) {
    unsigned int elapsed;
    unsigned int start = tsc_read();
    unsigned int target = us * CLK_MHZ;
    do {
        elapsed = tsc_read() - start;
    } while (elapsed < target);
}

void print_string(const char *str)
{
    const char *cp = str;
    while (*cp != '\0' ) {
        bdl_string_put_bl(&monitor_tx, *cp++);
    }
}


int main(void) {
    platform_init();
    printf("hello, world\n");


    blink_init(1);
    while (1) {
        blink_blink(500000000);
        delay_us(500000);
    }
    return 0;
}