#include "platform.h"

//#include "emblocs_api.h"
//#include "printing.h"
//#include "serial.h"
#include <assert.h>
#include <string.h>
//#include "tmp_gpio.h"
//#include "watch.h"


#if 0
void __assert_func (const char * file, int line, const char * funct, const char *expr)
{
    print_string("assert(");
    print_string(expr);
    print_string(") at ");
    print_string(file);
    print_string(":");
    print_int_dec(line, '\0');
    print_string(" in function ");
    print_string(funct);
    print_string("()\n");
    // loop forever
    do {} while (1);
}
#endif

// Quick and dirty delay
void delay (unsigned int time) {
    for (unsigned int i = 0; i < time; i++)
        for (volatile unsigned int j = 0; j < 20000; j++);
}

void delay_us (unsigned int us) {
    unsigned int elapsed;
    unsigned int start = tsc_read();
    unsigned int target = us * CLK_MHZ;
    do {
        elapsed = tsc_read() - start;
    } while (elapsed < target);
}


int main (void) {
    platform_init();

    while(1) {
        PB6_OUT(1);
        delay_us(500);
        PB6_OUT(0);
        delay_us(500);
    }
    return 0;
}

