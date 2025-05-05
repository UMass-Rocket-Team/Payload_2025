#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

int main()
{
    stdio_init_all();

    adc_init();
    adc_gpio_init(29);
    adc_select_input(3);

    while (true) {
        uint16_t result = adc_read();
        float voltage = (float)result * 3.3*3.f/(4096.f);
        printf("VSYS Voltage: %.2f V\n", voltage);
        sleep_ms(500);
    }
}
