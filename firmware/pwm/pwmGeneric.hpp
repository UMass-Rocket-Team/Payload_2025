#pragma once

#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "hardware/clocks.h"

int get_best_pwm_config(uint freq, uint16_t* wrapval, float* clkdiv) {
    
    if ( wrapval == NULL || clkdiv == NULL ) {
        return -1;
    }

    float clkdiv_tmp = (float)clock_get_hz(clk_sys)/((float)freq * UINT16_MAX);
    
    if ( clkdiv_tmp >= 1 && clkdiv_tmp < 256 ) {
        *clkdiv = clkdiv_tmp;
        *wrapval = UINT16_MAX;
        return 0;
    } else if ( clkdiv_tmp < 1 ) {
        uint16_t tmp_wrap = (float)clock_get_hz(clk_sys)/(float)freq;
        *clkdiv = 1.f;
        *wrapval = tmp_wrap;
        return 0;
    }

    return -1;

}

class PWM {

private:

    uint slice_num;
    uint gpio;
    uint wrap;
    pwm_config config;

public:

    PWM() = default;
    ~PWM() = default;

    bool init(uint gpio, uint freq) {

        this->gpio = gpio;

        // Initialize GPIO pin and enable PWM
        gpio_init(gpio);
        gpio_set_function(gpio, GPIO_FUNC_PWM);

        // Initialize PWM slice
        slice_num = pwm_gpio_to_slice_num(gpio);
        config = pwm_get_default_config();

        uint16_t wrapval;
        float clkdiv;

        int ret = get_best_pwm_config(freq, &wrapval, &clkdiv);

        if ( ret == -1 ) { return false; }

        wrap = wrapval;

        pwm_config_set_clkdiv(&config, clkdiv);
        pwm_config_set_wrap(&config, wrapval);
        pwm_config_set_clkdiv_mode(&config, PWM_DIV_FREE_RUNNING);
        pwm_config_set_phase_correct(&config, false);
        pwm_init(slice_num, &config, false);

        return true;
    }

    void set_enabled(bool enabled) {
        pwm_set_enabled(slice_num, enabled);
    }

    void set_level(uint level) {
        pwm_set_gpio_level(gpio, level);
    }

    void set_level(float level) {
        if ( wrap != 0 ) {
            pwm_set_gpio_level(gpio, (uint16_t)((float)wrap * level));
        }
    }

    void set_irq_enabled(bool enabled) {
        pwm_set_irq_enabled(slice_num, enabled);
    }

};