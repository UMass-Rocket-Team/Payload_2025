#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/clocks.h"


int main()
{
    stdio_init_all();

    while (1) {
        printf("%llu\n", time_us_64());
        sleep_ms(1000);
    }

}
