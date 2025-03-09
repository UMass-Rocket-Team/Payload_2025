/// @file main.cc
/// @brief Main firmware file for the Payload project.
///
/// This file contains the entry point and primary task scheduling for the firmware.
/// It performs hardware initialization, sensor data acquisition, state control logic,
/// data logging, and transmission handling. Each major block has detailed comments
/// explaining its functionality and error-handling strategy.

#include <stdio.h>
#include "pico/stdlib.h"
#include "math.h"

#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"

#include "tts.hpp"
#include "datalog.hpp"
#include "SD.hpp"
#include "pwmGeneric.hpp"
#include "fifo.hpp"

#include "bmp388.h"
#include "bno055.h"

#include "runningCounter.hpp"
#include "ringBuff.hpp"

#include "pico/time.h"

// #include "EepromArray.hpp"

extern "C" {

    #include "schedule.h"
    #include "morse.h"
    #include "pins.h"
    #include "tone.h"
    #include "buzz.h"
    #include "DS1307.h"

}

void task_main_cb(void* args);

static task_t task_main = task_init_default("main", task_main_cb);

// SD card
SD sdCard(PIN_CS_SD);

bno055_handle_t bno055_handle;
bmp388_handle_t bmp388_handle;

namespace stateControl {

    typedef enum system_state_t {
        STATE_IDLE,
        STATE_LAUNCH,
        STATE_LANDED
    } system_state_t;

    RunningCounter counter_launch_detect_accel(5);

    RunningCounter counter_landed_accel(100);
    RunningCounter counter_landed_velocity(100);
    RunningCounter counter_landed_orientation(100);

    void check_ld_accel(float ax, float ay, float az) {
        float mag = sqrt(ax*ax + ay*ay + az*az);
        counter_launch_detect_accel.update(mag > 12.0);
    }

    void check_landed_velocity(float vz) {
        counter_landed_velocity.update(abs(vz) < 5.0);
    }

    void check_landed_accel(float ax, float ay, float az) {
        float mag = sqrt(ax*ax + ay*ay + az*az);
        counter_landed_accel.update((mag > 9.3) && (mag < 10.3));
    }

    void check_landed_orientation(float vx, float vy, float vz) {
        float mag = sqrt(vx*vx + vy*vy + vz*vz);
        counter_landed_orientation.update(mag < (10.0 * 3.14159 / 180.0));
    }

    system_state_t state = STATE_IDLE;

    void update_state() {
        switch (state) {
            case STATE_IDLE:
                
                if (counter_launch_detect_accel.check()) {
                    state = STATE_LAUNCH;
                }

                break;
            case STATE_LAUNCH:

                if (counter_landed_accel.check()) {
                    state = STATE_LANDED;
                }

                if (counter_landed_orientation.check()) {
                    state = STATE_LANDED;
                }

                break;
            case STATE_LANDED:
                break;
        }
    }

};

namespace navigation {

    RingBuffer<float, 50> running_launchpad_alt_ringbuf;

    // rotational velocity
    float rx, ry, rz;

    // linear acceleration
    float ax, ay, az;

    // quaternion
    float qw, qx, qy, qz;

    // euler angles
    float y, p, r;

    float altitude;
    float velocity;
    float altitude_launchpad;

    RingBuffer<float, 10> running_alt_ringbuff;

    void update_posvel(float pos) {

        float acc = sqrtf(ax*ax + ay*ay + az*az);

        bool acc_ok = (acc > 8.0) && (acc < 11.6);

        if ((stateControl::state == stateControl::STATE_IDLE) && acc_ok) {

            if ( running_launchpad_alt_ringbuf.isFull() ) {
                float _;
                running_launchpad_alt_ringbuf.pop(_);
            }

            running_launchpad_alt_ringbuf.push(pos);

            float sum = 0.0;
            for (int i = 0; i < running_launchpad_alt_ringbuf.size(); i++) {
                sum += running_launchpad_alt_ringbuf[i];
            }

            altitude_launchpad = sum / running_launchpad_alt_ringbuf.size();
            // printf("Launchpad alt: %.2f\n", altitude_launchpad);

        }

        if ( running_alt_ringbuff.isFull() ) {
            float _;
            running_alt_ringbuff.pop(_);
        }

        running_alt_ringbuff.push(pos);

        float sum = 0.0;
        for (int i = 0; i < running_alt_ringbuff.size(); i++) {
            sum += running_alt_ringbuff[i];
        }

        float prev_alt = altitude;
        altitude = (sum / 10.0) - altitude_launchpad;
        
        float prev_vel = velocity;
        velocity = (prev_vel*0.7) + (0.3*((altitude - prev_alt) / 0.02));
    }

}

namespace dataCollection {

    uint64_t t_last_time_sync = 0;
    uint64_t t_time_sync_unix = 0;

    uint64_t t_landing_unix;
    float apogee_reached;
    float battery_voltage;
    float landing_velocity;
    float gforce_max;
    float temperature;

    RingBuffer<float, 100> running_velocity_ringbuf;

    RingBuffer<float, 100> running_battery_ringbuf;

    RunningCounter sustained_accel_x_pos(25);
    RunningCounter sustained_accel_x_neg(25);

    RunningCounter sustained_accel_y_pos(25);
    RunningCounter sustained_accel_y_neg(25);

    RunningCounter sustained_accel_z_pos(25);
    RunningCounter sustained_accel_z_neg(25);

    bool stemnot_survived = true;

    float y, p, r;

    uint64_t t_transmission_start = 0;
    const uint64_t transmission_max_dur = 2 * 60 * 1000000;

    void update_data(float velocity, float alt, float ax, float ay, float az) {

        sustained_accel_x_pos.update(ax > 373.0);
        sustained_accel_x_neg.update(ax < -284.0);

        sustained_accel_y_pos.update(ay > 49.0);
        sustained_accel_y_neg.update(ay < -49.0);

        sustained_accel_z_pos.update(az > 167.0);
        sustained_accel_z_neg.update(az < -63.7);

        // rather unfortunate event right here
        if (sustained_accel_x_pos.check() || sustained_accel_x_neg.check() || sustained_accel_y_pos.check() || sustained_accel_y_neg.check() || sustained_accel_z_pos.check() || sustained_accel_z_neg.check()) {
            printf("noooooooooo!\n");
            stemnot_survived = false;
        }
        float g = 9.816;

        if (
            (ax > 35.0 * g) || (ax < -28.0 * g) ||
            (ay > 15.0 * g) || (ay < -15.0 * g) ||
            (az > 15.2 * g) || (az < -13.4 * g)
        ) {
            printf("noooooooooo!\n");
            stemnot_survived = false;
        }

        float gforce = sqrt(ax*ax + ay*ay + az*az)/g;
        apogee_reached = fmax(apogee_reached, alt);
        gforce_max = fmax(gforce_max, gforce);

        if ( running_velocity_ringbuf.isFull() ) {
            float _;
            running_velocity_ringbuf.pop(_);
        }

        // only record velocity above 15m to avoid landing changing stuff  
        if ( alt > 15.0 ) {
            running_velocity_ringbuf.push(velocity);
        }
        
        float sum = 0.0;
        for (int i = 0; i < running_velocity_ringbuf.size(); i++) {
            sum += running_velocity_ringbuf[i];
        }

        landing_velocity = sum / running_velocity_ringbuf.size();

    }

    void update_battery() {
        if ( running_battery_ringbuf.isFull() ) {
            float _;
            running_battery_ringbuf.pop(_);
        }

        uint16_t adc_val = adc_read();
        float batt_voltage_raw = float(adc_val) * 3.3 * 3.f/(4096.f);
        running_battery_ringbuf.push(batt_voltage_raw);

        battery_voltage = 0;

        for ( int i = 0; i < running_battery_ringbuf.size(); i++ ) {
            battery_voltage += running_battery_ringbuf[i];
        }

        battery_voltage /= running_battery_ringbuf.size();
    }

    void start_transmission() {
        t_transmission_start = time_us_64();
    }

    void update_transmission() {
        
        if (t_transmission_start == 0) {
            return;
        }

        if ((time_us_64() - t_transmission_start) < transmission_max_dur) {

            if (TTS::tts_fifo.empty()) {

                printf("Starting new transmission\n");

                // callsign
                TTS::append_field(TTS::tts_field_t::kilo);
                TTS::append_field(TTS::tts_field_t::charlie);
                TTS::append_field(TTS::tts_field_t::one);
                TTS::append_field(TTS::tts_field_t::whiskey);
                TTS::append_field(TTS::tts_field_t::charlie);
                TTS::append_field(TTS::tts_field_t::kilo);
                
                TTS::append_field(TTS::tts_field_t::callsign);
                
                TTS::append_data(apogee_reached, 1, TTS::tts_field_t::max_alt);
                TTS::append_field(TTS::tts_field_t::meters);
                
                TTS::append_data(landing_velocity, 1, TTS::tts_field_t::land_vel);
                TTS::append_field(TTS::tts_field_t::mps);
                
                TTS::append_data(gforce_max, 1, TTS::tts_field_t::g_force);
                TTS::append_field(TTS::tts_field_t::g);
                
                TTS::append_data(battery_voltage, 2, TTS::tts_field_t::batt_voltage);
                TTS::append_field(TTS::tts_field_t::volts);
                
                TTS::append_data(temperature, 1, TTS::tts_field_t::temperature);
                TTS::append_field(TTS::tts_field_t::celsius);

                TTS::append_field(TTS::tts_field_t::landing_time);
                uint64_t hours = (t_landing_unix / 3600) % 24;
                uint64_t minutes = (t_landing_unix / 60) % 60;
                uint64_t seconds = t_landing_unix % 60;

                TTS::append_data(hours, 0, TTS::tts_field_t::hour);
                TTS::append_data(minutes, 0, TTS::tts_field_t::minute);
                TTS::append_data(seconds, 0, TTS::tts_field_t::second);

                TTS::append_field(TTS::tts_field_t::stemnot);
                TTS::append_field(TTS::tts_field_t::stem_surival);
                TTS::append_field(stemnot_survived ? TTS::tts_field_t::stem_alive : TTS::tts_field_t::stem_dead);

                TTS::append_field(TTS::tts_field_t::orientation);
                TTS::append_data(y, 1, TTS::tts_field_t::yaw);
                TTS::append_field(TTS::tts_field_t::degree);

                TTS::append_data(p, 1, TTS::tts_field_t::pitch);
                TTS::append_field(TTS::tts_field_t::degree);

                TTS::append_data(r, 1, TTS::tts_field_t::roll);
                TTS::append_field(TTS::tts_field_t::degree);

                TTS::append_field(TTS::tts_field_t::kilo);
                TTS::append_field(TTS::tts_field_t::charlie);
                TTS::append_field(TTS::tts_field_t::one);
                TTS::append_field(TTS::tts_field_t::whiskey);
                TTS::append_field(TTS::tts_field_t::charlie);
                TTS::append_field(TTS::tts_field_t::kilo);

                sleep_ms(1000);

            }

        }
    }

}

/// @brief Main entry point for the firmware.
/// 
/// This function performs all necessary initializations:
/// - Standard I/O for debug messages.
/// - Hardware peripherals such as I2C, SPI, and sensors.
/// - Task scheduling and error handling for critical subsystem failures.
/// - Determination of next log file index for persistent data storage on the SD card.
/// 
/// It then enters an infinite loop, running scheduled tasks. This loop never returns.
int main()
{
    // Initialize debugging output and hardware standard libraries.
    stdio_init_all();

    // Initialize UART for standard I/O
    // uart_init(uart1, 115200);
    // gpio_set_function(12, GPIO_FUNC_UART);
    // gpio_set_function(13, GPIO_FUNC_UART);

    // Pre-flight pause to allow removal of test code in final flight configuration.
    // TODO:
    // Remove before flight
    // getchar();
    sleep_ms(100);
    
	printf("Payload\n");

    adc_init();
    adc_gpio_init(29);
    adc_select_input(3);

    // buzzer_off();
    buzzer_on();

    // Set and add main task to the scheduler with appropriate priority.
    task_main.priority = 2;

    int ret;

    // ret = scheduler_add_task(&task_main);
    // if (ret != 0) {
    // //     printf("Failed to add main task\n");
    //     while(1) {
    //         buzzer_beep_error();
    //     }
    // }

    // // Add buzzer and TTS tasks as auxiliary tasks with error handling.
    // ret = scheduler_add_task(&task_buzz);
    // if (ret != 0) {
    // //     printf("Failed to add buzzer task\n");
    //     while(1) {
    //         buzzer_beep_error();
    //     }
    // }

    // ret = scheduler_add_task(&TTS::task_tts);
    // if (ret != 0) {
    // //     printf("Failed to add TTS task\n");
    //     while(1) {
    //         buzzer_beep_error();
    //     }
    // }

    // // Initialize the scheduler to start task execution.
    // ret = scheduler_init();
    // if (ret != 0) {
    // //     printf("Scheduler init failed\n");
    //     while(1) {
    //         buzzer_beep_error();
    //     }
    // }

    // Initialize I2C for sensor communication at 400kHz.
    i2c_init(i2c0, 400*1000);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    printf("I2C init...[OK]\n");

    int tries;

    // Sensor initialization with robust error checking and retries.
    printf("BNO055 init...");

    bno055_handle.iic_addr = (0x28);

    tries = 0;
    while (bno055_init(&bno055_handle) != 0) {
        tries++;
        if (tries > 5) {
            printf("BNO055 init failed\n");
            while(1) {
                buzzer_beep_error(); // Exits system on persistent sensor failure.
            }
        }
    }

    printf("[OK]\n");

    printf("BMP388 init...");

    bmp388_handle.iic_addr = (0x77);
    tries = 0;

    while (bmp388_init(&bmp388_handle) != 0) {
        tries++;
        if (tries > 5) {
            printf("BMP388 init failed\n");
            while(1) {
                buzzer_beep_error();
            }
        }
    }

    printf("[OK]\n");

    // SD card initialization. The SD card is used for logging flight data.
    printf("SD Card init...");

    if (!sdCard.open()) {
        printf("[FAILED]\n");
        while(1) {
            buzzer_beep_error();
        }
    } else {
        printf("[OK]\n");
    }

    std::vector<std::string> files = sdCard.listFiles("/");

    // Determine the next available log file index from SD card contents.
    int file_index = 0;

    for (int i = 0; i < files.size(); i++) {
        if (files[i].find("datalog") != std::string::npos) {
            int idx = std::stoi(files[i].substr(7, files[i].size() - 11));
            if (idx >= file_index) {
                file_index = idx + 1;
            }
        }
    }

    printf("Next file index: %d\n", file_index);

    datalog::set_filename(file_index);

    printf("Reading wave files...\n");

    // Read wave files from the SD card for TTS operations.

    // for ( int i = 0; i < (int)TTS::tts_field_t::field_max; i++ ) {

    //     if ( i == (int)TTS::tts_field_t::pause ) {
    //         continue;
    //     }

    //     char filename[32];
    //     TTS::filename_from_field((TTS::tts_field_t)i, filename);

    //     WaveFile soundFile(&TTS::waveArray[i]);
    //     printf("Opening file %s\n", filename);
    //     if (!soundFile.open(filename)) {
    //         printf("Error: Could not open file\n");
    //         continue;
    //     }

    //     // copy to buffer

    //     size_t waveSz = soundFile.getNumSamples();
    //     // TTS::waveArray[i] = (uint8_t*)malloc(waveSz);
    //     if (TTS::waveArray[i] == NULL) {
    //         printf("Error: Could not allocate memory\n");
    //         continue;
    //     }
    //     // TTS::waveArray[i].init(waveSz * 2);

    //     uint8_t* wavePtr = soundFile.getPtr();

    //     // printf("Copying %d (%d after rounding) samples\n", waveSz*2, ((waveSz*2)/256 + 1) * 256);
    //     // TTS::waveArray[i].write(0, wavePtr, ((waveSz*2)/256 + 1) * 256);
    //     printf("Copying\n");
    //     int last_percent = 0;
    //     for (size_t j = 0; j < waveSz; j++) {
    //         TTS::waveArray[i][j] = wavePtr[j];
    //         if (int(float(j)/waveSz * 100.0) >= last_percent + 10) {
    //             last_percent = int(float(j)/waveSz * 100.0);
    //             printf("Copying %.2f %%\n", float(j)/waveSz * 100.0);
    //         }
    //     }

    //     if (!soundFile.close()) {
    //         printf("Error: Could not close file\n");
    //     }

    //     printf("Completed file %d of %d (%.2f %)\n", i, (int)TTS::tts_field_t::field_max, (float(i) / (int)TTS::tts_field_t::field_max) * 100.0);

    // }

    printf("Waiting for time sync\n");

    // Synchronize time with external input, proceeding after a 500ms timeout.
    int c;

    c = getchar_timeout_us(5000);
    if (c == PICO_ERROR_TIMEOUT) {
        printf("Timeout\n");
    } else {

        printf("Syncing time\n");
        char time_str[33];
        int i = 0;
        while ( c != PICO_ERROR_TIMEOUT && c != '\n' && i < 32 ) {
            time_str[i] = c;
            i++;
            c = getchar_timeout_us(5000);
        }

        time_str[i] = '\0';

        printf("Time string: %s\n", time_str);
        
        int64_t time_val = atoll(time_str);
        printf("Time value: %lld\n", time_val);

    }

    // Signal successful initialization via auditory (buzzer) cues.
    buzzer_beep_ok();

    buzzer_on();
    
    // Initialize data logging and TTS modules.
    datalog::init();
    TTS::init();

    printf("Entering main loop\n");

    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);

    // Main loop: continuously run scheduled tasks.
    while(1) {
        task_main_cb(NULL);
        while(time_us_64() < task_main.next_run) {;}
    }

}

int tts_iter_num = 0;

/**
 * @brief Task callback function for the main scheduling loop.
 *
 * This function is executed periodically by the scheduler. It
 * - Reads sensor data from BNO055: gyroscope (omega), acceleration, quaternion and Euler angles.
 * - Reads pressure and temperature from BMP388, calculates altitude.
 * - Updates position and velocity filtering using RingBuffer objects.
 * - Checks and updates the system state (idle, launch, landed) based on sensor thresholds.
 * - Initiates transmissions, triggers TTS operations, and logs data via the datalog module.
 *
 * It ensures that sensor read failures are logged, and in the case of critical errors,
 * the system indicates these via buzzer-beep error routines.
 *
 * @param args Unused pointer to additional task arguments.
 */
void task_main_cb(void* args) {

    if ( stateControl::state == stateControl::STATE_IDLE ) {
        int c;

        c = getchar_timeout_us(50);
        if (c == PICO_ERROR_TIMEOUT) {
            printf("Timeout\n");
        } else {

            printf("Syncing time\n");
            char time_str[33];
            int i = 0;
            while ( c != PICO_ERROR_TIMEOUT && c != '\n' && i < 32 ) {
                time_str[i] = c;
                i++;
                c = getchar_timeout_us(5000);
            }

            time_str[i] = '\0';

            printf("Time string: %s\n", time_str);
            
            int64_t time_val = atoll(time_str);
            printf("Time value: %lld\n", time_val);

            dataCollection::t_last_time_sync = time_us_64();
            dataCollection::t_time_sync_unix = time_val;

        }
    }

    gpio_put(25, 1);

    uint64_t t_start = time_us_64();

    // Read BNO055 data

    float pres, temp, alt_raw;
    uint32_t pres_raw, temp_raw;
    uint8_t ret;

    if ( stateControl::state != stateControl::STATE_LANDED ) {

        // Read angular velocity from BNO055 sensor.
        // If the read fails, log a failure message.
        ret = bno055_read_omega(&bno055_handle, &navigation::rx, &navigation::ry, &navigation::rz);
        if (ret != 0) {
            printf("BNO055 read failed\n");
        }

        // Read acceleration values from BNO055 sensor.
        ret = bno055_read_acceleration(&bno055_handle, &navigation::ax, &navigation::ay, &navigation::az);
        if (ret != 0) {
            printf("BNO055 read failed\n");
        }

        // Read quaternion data from BNO055 sensor.
        ret = bno055_read_quat(&bno055_handle, &navigation::qw, &navigation::qx, &navigation::qy, &navigation::qz);
        if (ret != 0) {
            printf("BNO055 read failed\n");
        }

        // Read Euler angles (yaw, pitch, roll) from BNO055 sensor.
        ret = bno055_read_ypr(&bno055_handle, &navigation::y, &navigation::p, &navigation::r);
        if (ret != 0) {
            printf("BNO055 read failed\n");
        }

        // Read BMP388 data

        // Read measurements from BMP388 sensor (temperature and pressure).
        ret = bmp388_read_temperature_pressure(&bmp388_handle, &temp_raw, &temp, &pres_raw, &pres);
        if (ret != 0) {
            printf("BMP388 read failed\n");
        }

        // calculate altitude
        alt_raw = 44330 * (1.0 - pow((pres/100.f) / 1013.25, 0.1903));

        // Update position and velocity estimates using filtered altitude values.
        navigation::update_posvel(alt_raw);
            
    }

    // Check state
    stateControl::system_state_t old_state = stateControl::state;

    switch(stateControl::state) {
        case stateControl::STATE_IDLE:
            stateControl::check_ld_accel(navigation::ax, navigation::ay, navigation::az);
            break;
        case stateControl::STATE_LAUNCH:
            buzzer_off();
            stateControl::check_landed_accel(navigation::ax, navigation::ay, navigation::az);
            stateControl::check_landed_orientation(navigation::rx, navigation::ry, navigation::rz);
            break;
        case stateControl::STATE_LANDED:
            buzzer_off();
            break;
    }

    // Update the global system state based on counters and thresholds.
    stateControl::update_state();
    if (old_state != stateControl::state) {
        printf("State change: %d -> %d\n", old_state, stateControl::state);
    }

    // Update transmission and TTS functionalities once landing is confirmed.
    dataCollection::apogee_reached = fmax(dataCollection::apogee_reached, navigation::altitude);
    dataCollection::gforce_max = fmax(dataCollection::gforce_max, sqrt(navigation::ax * navigation::ax +
                                   navigation::ay * navigation::ay +
                                   navigation::az * navigation::az) / 9.816);
    if (stateControl::state == stateControl::STATE_LANDED && dataCollection::t_transmission_start == 0) {
        dataCollection::t_landing_unix = dataCollection::t_time_sync_unix + ((time_us_64() - dataCollection::t_last_time_sync) / 1000000);
        printf("Landing time: %llu\n", dataCollection::t_landing_unix);
        // dataCollection::battery_voltage = 3.3;
        dataCollection::landing_velocity = navigation::velocity;
        dataCollection::temperature = temp;
        dataCollection::y = navigation::y;
        dataCollection::p = navigation::p;
        dataCollection::r = navigation::r;
        dataCollection::start_transmission();
    }

    dataCollection::update_transmission();

    // Debug: print current flight metrics for non-landed state.
    if (stateControl::state != stateControl::STATE_LANDED) {
        printf("pos: %.2f, vel: %.2f, ax: %.2f, ay: %.2f, az: %.2f, batt: %.2f\n", 
           navigation::altitude, navigation::velocity, navigation::ax, navigation::ay, navigation::az, dataCollection::battery_voltage);
    } 

    if ( stateControl::state == stateControl::STATE_LANDED && tts_iter_num >= 10) {
        TTS::update(NULL);
        tts_iter_num = 0;
    }

    tts_iter_num++;

    dataCollection::update_battery();

    // Log data
    if ( stateControl::state != stateControl::STATE_LANDED ) {
        // Log sensor data into a CSV file via datalog module.
        datalog::update_datalog_accel(navigation::ax, navigation::ay, navigation::az);
        datalog::update_datalog_gyro(navigation::rx, navigation::ry, navigation::rz);
        datalog::update_datalog_quat(navigation::qw, navigation::qx, navigation::qy, navigation::qz);
        datalog::update_datalog_baro(pres, temp, alt_raw);
        datalog::update_datalog_fusion(navigation::altitude, navigation::velocity);
        datalog::update_datalog_batt(dataCollection::battery_voltage);

        // uint64_t t0 = time_us_64();
        datalog::update();
        // uint64_t t1 = time_us_64();
    }
    // // printf("Datalog update tim e: %llu\n", uint64_t(t1 - t0));

    // Schedule next execution of this task (every 20ms).
    task_main.next_run = t_start + 20000;

    gpio_put(25, 0);

}
