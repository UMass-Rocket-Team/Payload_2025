#include "pico/stdlib.h"
#include <stdio.h>
#include "pico/stdio.h"
#include "bmp388.h"
#include <string>

int main(int argc, char const *argv[])
{
    stdio_init_all();
    bmp388_handle_t handle;
    handle.iic_addr = (0x77);
    i2c_inst_t* I2C_PORT = i2c0;

    while (bmp388_init(&handle) != 0) {
        printf("INIT FAILING");
    }

    while(true) {
        uint32_t buf1;
        uint32_t buf2;
        float temp;
        float pressure;
        bmp388_read_temperature_pressure(&handle, (uint32_t*)&buf1, &temp,(uint32_t*) &buf2, &pressure);
        printf("TEMP RAW? \n");
        printf("%u \n", buf1);
        printf("TEMP COMP? \n");
        printf("%f \n", temp);

        printf("PRESS RAW? \n");
        printf("%u \n", buf2);
        printf("PRESS COMP? \n");
        printf("%f \n", pressure);

        sleep_ms(1000);
    } 
}
