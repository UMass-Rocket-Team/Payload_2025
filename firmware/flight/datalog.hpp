#pragma once

#include "SD.hpp"
#include "pins.h"

namespace datalog {

typedef struct {

    uint64_t timestamp;

    float baro_pressure;
    float baro_temperature;
    float baro_altitude;

    float accel_x;
    float accel_y;
    float accel_z;

    float gyro_x;
    float gyro_y;
    float gyro_z;

    float mag_x;
    float mag_y;
    float mag_z;

    float quat_w;
    float quat_x;
    float quat_y;
    float quat_z;

    float fusion_altitude;
    float fusion_velocity;

} datalog_frame_t;

const static size_t datalog_frame_sz = sizeof(datalog_frame_t);

void init();
void update();

void set_filename(int idx);
void update_datalog_accel(const float x, const float y, const float z);
void update_datalog_gyro(const float x, const float y, const float z);
void update_datalog_mag(const float x, const float y, const float z);
void update_datalog_baro(const float pressure, const float temperature, const float altitude);
void update_datalog_quat(const float w, const float x, const float y, const float z);
void update_datalog_fusion(const float altitude, const float velocity);

} // namespace datalog
