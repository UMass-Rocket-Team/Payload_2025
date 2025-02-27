#include "datalog.hpp"
#include "SD.hpp"

namespace datalog {

static char datalog_fname[32];

const char* header = "timestamp,baro_pressure,baro_temperature,baro_altitude,"
                             "accel_x,accel_y,accel_z,"
                             "gyro_x,gyro_y,gyro_z,"
                             "mag_x,mag_y,mag_z,"
                             "quat_w,quat_x,quat_y,quat_z,"
                             "fusion_altitude,fusion_velocity\n";

static datalog_frame_t datalog_frame;

static char datalog_strbuf[4096];

void update_datalog_accel(const float x, const float y, const float z) {
    datalog_frame.accel_x = x;
    datalog_frame.accel_y = y;
    datalog_frame.accel_z = z;
}

void update_datalog_gyro(const float x, const float y, const float z) {
    datalog_frame.gyro_x = x;
    datalog_frame.gyro_y = y;
    datalog_frame.gyro_z = z;
}

void update_datalog_mag(const float x, const float y, const float z) {
    datalog_frame.mag_x = x;
    datalog_frame.mag_y = y;
    datalog_frame.mag_z = z;
}

void update_datalog_baro(const float pressure, const float temperature, const float altitude) {
    datalog_frame.baro_pressure = pressure;
    datalog_frame.baro_temperature = temperature;
    datalog_frame.baro_altitude = altitude;
}

void update_datalog_quat(const float w, const float x, const float y, const float z) {
    datalog_frame.quat_w = w;
    datalog_frame.quat_x = x;
    datalog_frame.quat_y = y;
    datalog_frame.quat_z = z;
}

void update_datalog_fusion(const float altitude, const float velocity) {
    datalog_frame.fusion_altitude = altitude;
    datalog_frame.fusion_velocity = velocity;
}

void init() {

    TextFile datalog_file;

    if (!datalog_file.open(datalog_fname, FA_OPEN_APPEND | FA_WRITE | FA_CREATE_NEW)) {
        printf("Failed to open datalog file\n");
        return;
    } else {
        printf("Opened datalog file\n");
    }

    if (!datalog_file.writeLine(header)) {
        printf("Failed to write header to datalog file\n");
    }

    if (!datalog_file.close()) {
        printf("Failed to close datalog file\n");
    }

}

void set_filename(int idx) {
    sprintf(datalog_fname, "datalog%d.csv", idx);
}

void update() {

    datalog_frame.timestamp = to_us_since_boot(get_absolute_time());

    int bytes_enc = sprintf(datalog_strbuf, "%lu,%.6f,%.6f,%.6f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f\n",
            datalog_frame.timestamp,
            datalog_frame.baro_pressure,
            datalog_frame.baro_temperature,
            datalog_frame.baro_altitude,
            datalog_frame.accel_x,
            datalog_frame.accel_y,
            datalog_frame.accel_z,
            datalog_frame.gyro_x,
            datalog_frame.gyro_y,
            datalog_frame.gyro_z,
            datalog_frame.mag_x,
            datalog_frame.mag_y,
            datalog_frame.mag_z,
            datalog_frame.quat_w,
            datalog_frame.quat_x,
            datalog_frame.quat_y,
            datalog_frame.quat_z,
            datalog_frame.fusion_altitude,
            datalog_frame.fusion_velocity);

    // printf("encoded %d bytes\n", bytes_enc);

    TextFile datalog_file;

    if (!datalog_file.open(datalog_fname, FA_OPEN_APPEND | FA_WRITE | FA_OPEN_ALWAYS)) {
        printf("Failed to open %s\n", datalog_fname);
        return;
    }

    if (!datalog_file.writeLine(datalog_strbuf)) {
        printf("Failed to write to %s\n", datalog_fname);
    }

    if (!datalog_file.close()) {
        printf("Failed to close %s\n", datalog_fname);
    }

}

} // namespace datalog