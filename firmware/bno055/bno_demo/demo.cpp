#include "pico/stdlib.h"
#include "bno055.h"
#include <stdio.h>
#include "pico/stdio.h"
#include <string>

int main(int argc, char const *argv[])
{
    stdio_init_all();
    bno055_handle_t handle;
    while (bno055_init(&handle) != 0) {
        printf("INIT FAILING\n");
        sleep_ms(500);
    }

    int16_t max_accel = 0;

    while(true) {
        
        float y;
        float p;
        float r;
        bno055_read_ypr(&handle, &y, &p, &r);
        printf("Yaw: %f \n", y);
        printf("Pitch: %f \n", p);
        printf("Roll: %f \n", r);

        float x_accel;
        float y_accel;
        float z_accel;
        bno055_read_acceleration(&handle, &x_accel, &y_accel, &z_accel);
        printf("X Accel: %f \n", x_accel);
        printf("Y Accel: %f \n", y_accel);
        printf("Z Accel: %f \n", z_accel);
        printf("\n\n");

        sleep_ms(100);
    } 
}
