#pragma once

#include "pins.h"
#include "tone.h"
#include "pico/stdlib.h"

#include "schedule.h"

void task_buzz_cb(void* args);

task_t task_buzz = task_init_default("buzzer", task_buzz_cb);

#define BUZZER_ERR_FREQ 1000
#define BUZZER_OK_FREQ 500

static int buzzer_error = 0;
static int buzzer_en = 0;

void buzzer_set_error() {
    buzzer_error = 1;
}

void buzzer_on() {
    buzzer_en = 1;
}

void buzzer_off() {
    buzzer_en = 0;
}

void buzzer_beep_error() {
    if (!buzzer_en) { return; }
    buzzer_tone(PIN_BUZZER, BUZZER_ERR_FREQ);
    sleep_ms(500);
    buzzer_disable(PIN_BUZZER);
    sleep_ms(500);
}

void buzzer_beep_ok() {
    if (!buzzer_en) { return; }
    for ( int i = 0; i < 3; i++ ) {
        buzzer_tone(PIN_BUZZER, BUZZER_OK_FREQ);
        sleep_ms(250);
        buzzer_disable(PIN_BUZZER);
        sleep_ms(250);    
    }
}

void task_buzz_cb(void* args) {

    if (!buzzer_en) {
        task_buzz.next_run = time_us_64() + 1000000;
        return;
    }

    if (buzzer_error) {
        while(1) { buzzer_beep_error(); }
    } else {
        buzzer_beep_ok();
        task_buzz.next_run = time_us_64() + 30000000;
    }

}