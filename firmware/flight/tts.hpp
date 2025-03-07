#ifndef TTS_HPP
#define TTS_HPP

#include "SD.hpp"
#include "pwmGeneric.hpp"
#include "fifo.hpp"
#include "EepromArray.hpp"
#include "audio_files.h"

extern "C" {
    #include "schedule.h"
}

namespace TTS {

    enum class tts_field_t {
    
        zero = 0,
        one,
        two,
        three,
        four,
        five,
        six,
        seven,
        eight,
        nine,
    
        dot,
    
        pause,
    
        callsign,
        apogee,
        max_alt,
        land_vel,
        g_force,
        orientation,
        temperature,
        batt_voltage,
        stem_surival,
        landing_time,

        stemnot,

        kilo,
        charlie,
        whiskey,
        mps,
        celsius,
        volts,
        g,
        meters,
        second,
        minute,
        hour,

        stem_dead,
        stem_alive,

        yaw,
        pitch,
        roll,

        degree,

        field_max
    
    };

    static FIFO<tts_field_t, 256> tts_fifo;
    
    void init();
    
    void update(void* args);

    int append_data(float val, int decimals, tts_field_t identifier);
    
    int append_field(tts_field_t identifier);
    
    task_t task_tts = task_init_default("tts", update);
    
    int append_field(tts_field_t field) {
    
        if ( tts_fifo.available() < 1 ) {
            return -1;
        }
    
        if (!tts_fifo.push(field)) {
            return -1;
        }
    
        return 0;
    
    }

    int append_data(float val, int decimals, tts_field_t identifier) {
    
        char buffer[32];
        int len = snprintf(buffer, 32, "%.*f", decimals, val);
    
        if (len < 0) {
            return -1;
        }
    
        // would be len - 1, but we also need the callsign and a pause
        int digits_required = len + 1;
    
        if ( tts_fifo.available() < digits_required ) {
            return -1;
        }
    
        if (!tts_fifo.push(identifier)) {
            return -1;
        }
    
        // TODO:
        // add error handling for if a fifo push fails, we should probably clear the fifo of whatever we just added
        for (int i = 0; i < len; i++) {

            tts_field_t field;

            if (buffer[i] == '-') {
                field = tts_field_t::pause;
            } else if (buffer[i] == '.') {
                field = tts_field_t::dot;
            } else {
                field = (tts_field_t)(buffer[i] - '0');
            }

            if (!tts_fifo.push(field)) {

                // clear the fifo of whatever we just added
                for ( int j = 0; j < i; j++ ) {
                    tts_fifo.pop_from_tail();
                }

                return -1;
            }

        }
    
        return 0;
    
    }
    
    static bool filename_from_field(tts_field_t field, char* filename) {
    
        switch (field) {
            case tts_field_t::zero:
                strcpy(filename, "0.wav");
                return true;
            case tts_field_t::one:
                strcpy(filename, "1.wav");
                return true;
            case tts_field_t::two:
                strcpy(filename, "2.wav");
                return true;
            case tts_field_t::three:
                strcpy(filename, "3.wav");
                return true;
            case tts_field_t::four:
                strcpy(filename, "4.wav");
                return true;
            case tts_field_t::five:
                strcpy(filename, "5.wav");
                return true;
            case tts_field_t::six:
                strcpy(filename, "6.wav");
                return true;
            case tts_field_t::seven:
                strcpy(filename, "7.wav");
                return true;
            case tts_field_t::eight:
                strcpy(filename, "8.wav");
                return true;
            case tts_field_t::nine:
                strcpy(filename, "9.wav");
                return true;
            case tts_field_t::dot:
                strcpy(filename, "dot.wav");
                return true;
            case tts_field_t::pause:
                return false;
            case tts_field_t::callsign:
                strcpy(filename, "callsign.wav");
                return true;
            case tts_field_t::landing_time:
                strcpy(filename, "t_land.wav");
                return true;
            case tts_field_t::apogee:
                strcpy(filename, "apogee.wav");
                return true;
            case tts_field_t::max_alt:
                strcpy(filename, "max_alt.wav");
                return true;
            case tts_field_t::land_vel:
                strcpy(filename, "land_vel.wav");
                return true;
            case tts_field_t::g_force:
                strcpy(filename, "gforce.wav");
                return true;
            case tts_field_t::orientation:
                strcpy(filename, "orientation.wav");
                return true;
            case tts_field_t::temperature:
                strcpy(filename, "temp.wav");        
                return true;
            case tts_field_t::batt_voltage:
                strcpy(filename, "batt.wav");        
                return true;
            case tts_field_t::stem_surival:
                strcpy(filename, "survival.wav");        
                return true;
            case tts_field_t::kilo:
                strcpy(filename, "kilo.wav");
                return true;
            case tts_field_t::charlie:
                strcpy(filename, "charlie.wav");
                return true;
            case tts_field_t::whiskey:
                strcpy(filename, "whiskey.wav");
                return true;
            case tts_field_t::mps:
                strcpy(filename, "mps.wav");
                return true;
            case tts_field_t::celsius:
                strcpy(filename, "celsius.wav");
                return true;
            case tts_field_t::volts:
                strcpy(filename, "volts.wav");
                return true;
            case tts_field_t::g:
                strcpy(filename, "g.wav");
                return true;
            case tts_field_t::meters:
                strcpy(filename, "meters.wav");
                return true;
            case tts_field_t::second:
                strcpy(filename, "second.wav");
                return true;
            case tts_field_t::minute:
                strcpy(filename, "minute.wav");
                return true;
            case tts_field_t::hour:
                strcpy(filename, "hour.wav");
                return true;
            case tts_field_t::stem_dead:
                strcpy(filename, "stem_dead.wav");
                return true;
            case tts_field_t::stem_alive:
                strcpy(filename, "stem_alive.wav");
                return true;
            case tts_field_t::yaw:
                strcpy(filename, "yaw.wav");
                return true;
            case tts_field_t::pitch:
                strcpy(filename, "pitch.wav");
                return true;
            case tts_field_t::roll:
                strcpy(filename, "roll.wav");
                return true;   
            case tts_field_t::degree:
                strcpy(filename, "degree.wav");
                return true;     
            default:
                return false;

        }
        
    }
    
    const uint8_t* wave_pointer_from_field(tts_field_t field, size_t* size) {
    
        switch(field) {

            case tts_field_t::zero:
                *size = sizeof(f_0_wav);
                return f_0_wav;
            case tts_field_t::one:
                *size = sizeof(f_1_wav);
                return f_1_wav;
            case tts_field_t::two:
                *size = sizeof(f_2_wav);
                return f_2_wav;
            case tts_field_t::three:
                *size = sizeof(f_3_wav);
                return f_3_wav;
            case tts_field_t::four:
                *size = sizeof(f_4_wav);
                return f_4_wav;
            case tts_field_t::five:
                *size = sizeof(f_5_wav);
                return f_5_wav;
            case tts_field_t::six:
                *size = sizeof(f_6_wav);
                return f_6_wav;
            case tts_field_t::seven:
                *size = sizeof(f_7_wav);
                return f_7_wav;
            case tts_field_t::eight:
                *size = sizeof(f_8_wav);
                return f_8_wav;
            case tts_field_t::nine:
                *size = sizeof(f_9_wav);
                return f_9_wav;
            case tts_field_t::dot:
                *size = sizeof(f_dot_wav);
                return f_dot_wav;
            case tts_field_t::callsign:
                *size = sizeof(f_callsign_wav);
                return f_callsign_wav;
            case tts_field_t::landing_time:
                *size = sizeof(f_t_land_wav);
                return f_t_land_wav;
            case tts_field_t::apogee:
                *size = sizeof(f_apogee_wav);
                return f_apogee_wav;
            case tts_field_t::max_alt:
                *size = sizeof(f_apogee_wav);
                return f_apogee_wav;
            case tts_field_t::land_vel:
                *size = sizeof(f_land_vel_wav);
                return f_land_vel_wav;
            case tts_field_t::g_force:
                *size = sizeof(f_gforce_wav);
                return f_gforce_wav;
            case tts_field_t::orientation:
                *size = sizeof(f_orient_wav);
                return f_orient_wav;
            case tts_field_t::temperature:
                *size = sizeof(f_temp_wav);
                return f_temp_wav;
            case tts_field_t::batt_voltage:
                *size = sizeof(f_batt_wav);
                return f_batt_wav;
            case tts_field_t::stem_surival:
                *size = sizeof(f_survival_wav);
                return f_survival_wav;
            case tts_field_t::kilo:
                *size = sizeof(f_kilo_wav);
                return f_kilo_wav;
            case tts_field_t::charlie:
                *size = sizeof(f_charlie_wav);
                return f_charlie_wav;
            case tts_field_t::whiskey:
                *size = sizeof(f_whiskey_wav);
                return f_whiskey_wav;
            case tts_field_t::mps:
                *size = sizeof(f_mps_wav);
                return f_mps_wav;
            case tts_field_t::celsius:
                *size = sizeof(f_celsius_wav);
                return f_celsius_wav;
            case tts_field_t::volts:
                *size = sizeof(f_volts_wav);
                return f_volts_wav;
            case tts_field_t::g:
                *size = sizeof(f_g_wav);
                return f_g_wav;
            case tts_field_t::meters:
                *size = sizeof(f_meters_wav);
                return f_meters_wav;
            case tts_field_t::second:
                *size = sizeof(f_second_wav);
                return f_second_wav;
            case tts_field_t::minute:
                *size = sizeof(f_minute_wav);
                return f_minute_wav;
            case tts_field_t::hour:
                *size = sizeof(f_hour_wav);
                return f_hour_wav;
            case tts_field_t::stem_dead:
                *size = sizeof(f_stem_dead_wav);
                return f_stem_dead_wav;
            case tts_field_t::stem_alive:
                *size = sizeof(f_stem_alive_wav);
                return f_stem_alive_wav;
            case tts_field_t::yaw:
                *size = sizeof(f_yaw_wav);
                return f_yaw_wav;
            case tts_field_t::pitch:
                *size = sizeof(f_pitch_wav);
                return f_pitch_wav;
            case tts_field_t::roll:
                *size = sizeof(f_roll_wav);
                return f_roll_wav;
            case tts_field_t::degree:
                *size = sizeof(f_degree_wav);
                return f_degree_wav;
            default:
                return nullptr;

        }
    
    }

    void init() {}

    void update(void* args) {

        // printf("tts update\n");

        if (tts_fifo.empty()) {
            return;
        }
    
        tts_field_t field = tts_fifo.pop();
    
        size_t data_size;
        const uint8_t* data_ptr;

        if ( field == tts_field_t::pause ) {
            data_ptr = nullptr;
        } else {
            data_ptr = wave_pointer_from_field(field, &data_size);
        
            if (data_ptr == nullptr) {
                printf("Error: Could not find data for field\n");
                return;
            }
        }

        char filename[32];
    
        if (!filename_from_field(field, filename)) {
            printf("Error: Could not find filename for field\n");
            return;
        }
    
        // play file
    
        printf("Playing file: %s...", filename);
    
        PWM pwmChan;
    
        if ( !pwmChan.init(16, 8000) ) {
            printf("[FAILED]\n");
            printf("Error: Could not init PWM\n");
            return;
        }
    
        pwmChan.set_enabled(true);
    
        if ( field == tts_field_t::pause ) {
            pwmChan.set_level(0.0f);
            sleep_ms(200);
            pwmChan.set_enabled(false);
            printf("[OK]\n");
            printf("Pause\n");
            task_tts.next_run = time_us_64() + 10000; // run at 100Hz
            return;
        }
    
        for ( int i = 0; i < data_size; i++ ) {
            
            uint64_t start = time_us_64();
    
            // convert to float and scale to [0.0, 1.0]
            float sample_f = ((float(data_ptr[i]) / INT8_MAX) + 1.f) / 2.f;

            // Update PWM slice
            pwmChan.set_level(sample_f);  
    
            // Wait for the next sample time
            while ((time_us_64() - start) < (1000000 / 8000)) {}
    
        }
    
        pwmChan.set_level(0.0f);
        // pwmChan.set_enabled(false);
    
        printf("[OK]\n");

        task_tts.next_run = time_us_64() + 10000; // run at 100Hz
    
    }
    
} // namespace TTS

#endif