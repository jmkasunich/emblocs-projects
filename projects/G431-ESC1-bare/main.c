#include "platform.h"
#include "blink.h"

void delay_us(unsigned int us) {
    unsigned int elapsed;
    unsigned int start = tsc_read();
    unsigned int target = us * CLK_MHZ;
    do {
        elapsed = tsc_read() - start;
    } while (elapsed < target);
}

int main(void) {
    platform_init();
    blink_init(1);
    while (1) {
        blink_blink(500000000);
        delay_us(500000);
    }
    return 0;
}