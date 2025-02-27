#ifndef TTS_HPP
#define TTS_HPP

#include "SD.hpp"
#include "pwmGeneric.hpp"
#include "fifo.hpp"

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
    
        dot = 10,
    
        pause = 11,
    
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

        degree
    
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
                strcpy(filename, "pause.wav");
                return true;
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
    
    void init() {}

    void update(void* args) {

        // printf("tts update\n");

        if (tts_fifo.empty()) {
            return;
        }
    
        tts_field_t field = tts_fifo.pop();
    
        char filename[32];
    
        if (!filename_from_field(field, filename)) {
            printf("Error: Could not find filename for field\n");
            return;
        }
    
        // play file
    
        printf("Playing file: %s\n", filename);
    
        PWM pwmChan;
    
        if ( !pwmChan.init(16, 16000) ) {
            printf("Error: Could not init PWM\n");
            return;
        }
    
        pwmChan.set_enabled(true);
    
        if ( field == tts_field_t::pause ) {
            pwmChan.set_level(0.0f);
            sleep_ms(200);
            pwmChan.set_enabled(false);
            printf("Pause\n");
            task_tts.next_run = time_us_64() + 10000; // run at 100Hz
            return;
        }
        
        WaveFile soundFile;

        if ( !soundFile.open(filename) ) {
            printf("Error: Could not open file\n");
            return;
        }
    
        for ( int i = 0; i < soundFile.getNumSamples(); i++ ) {
            
            uint64_t start = time_us_64();

            // copy the sample from the file buffer
            int16_t sample;
            soundFile.readSamples((uint16_t*)&sample, 2, i*2);
    
            // convert to float and scale to [0.0, 1.0]
            float sample_f = ((float(sample) / INT16_MAX) + 1.f) / 2.f;

            // Update PWM slice
            pwmChan.set_level(sample_f);  
    
            // Wait for the next sample time
            while ((time_us_64() - start) < (1000000 / 16000)) {}
    
        }
    
        pwmChan.set_enabled(false);
    
        soundFile.close();
    
        printf("File played\n");

        task_tts.next_run = time_us_64() + 10000; // run at 100Hz
    
    }
    
} // namespace TTS

#endif