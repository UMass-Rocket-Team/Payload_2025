#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "pwm_generic.hpp"
#include <string.h>
#include <math.h>

extern "C" {
    #include "morse.h"
}

int main()
{
    stdio_init_all();

    // wait for connection and serial input
    getchar();

    sleep_ms(50);
    printf("Hello, world!\n");

    // initialize PWM 
    PWM pwm;

    if (!pwm.init(0, 16000)) {
        printf("PWM initialization failed\n");
        return -1;
    }

    // enable PWM
    pwm.set_enabled(true);
    printf("PWM enabled\n");
    
    char msgbuf[100];
    char morsebuf[1024];

    while (1) {

        // prompt the user for a message
        printf("Enter a message to convert to morse code: \n>>");

        char c;
        int i = 0;

        while ( (msgbuf[i++] = getchar()) != '\r' ) { if ( i >= sizeof(msgbuf)-1 ) break; }

        // add null terminator
        msgbuf[i] = '\0';

        printf("message length: %d\n", i);
        printf("Translating message: %s\n", msgbuf);

        // convert the message to morse code
        if ( str2morse(msgbuf, morsebuf, 1024) == -1 ) {
            printf("Error converting message to morse code\n");
            continue;
        } 

        // initialize the morse code
        morse_input_t settings;

        settings.code = morsebuf;
        settings.carrier_freq = 400;
        settings.time_interval = 1.f/16.f;
        settings.wave_type = morse_wave_sine;

        // output the morse code over pwm
        uint32_t t_start = time_us_32();
        uint32_t last_t = 0;
        while ( true ) {

            uint32_t t = time_us_32() - t_start;

            float t_sec = (float)t / 1000000.f;

            float val = generate_morse_signal(&settings, t_sec);
            pwm.set_level(val);

            if ( (settings.time_interval * get_morse_duration(&settings)) < t_sec ) {
                break;
            }

            last_t = t;

        }

        printf("Morse code output complete\n");

    }

}
