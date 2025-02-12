#pragma once
#include <cstring>
#include <hardware/i2c.h>
#include "pico/stdlib.h"
#include <stdio.h>
#include "pico/stdio.h"

// --- Constants and Definitions ---
#define BNO055_I2C_ADDRESS  0x28
#define I2C_PORT            i2c1
#define SDA_PIN             9  // Adjust based on wiring
#define SCL_PIN             10  // Adjust based on wiring

// Registers
typedef enum {
    BNO_PWR_MODE = 0x00,
    BNO_OPR_MODE = 0x10,
    BNO_YAW = 0x1A,
    BNO_PITCH = 0x1E,
    BNO_ROLL = 0x1C,
    BNO_OMEGA_X = 0x14,
    BNO_OMEGA_Y = 0x16,
    BNO_OMEGA_Z = 0x18,
    BNO_ACCEL_X = 0x28,
    BNO_ACCEL_Y = 0x2A,
    BNO_ACCEL_Z = 0x2C,
    BNO_QUAT_W = 0x20,
    BNO_QUAT_X = 0x22,
    BNO_QUAT_Y = 0x24,
    BNO_QUAT_Z = 0x26,
} bno055_register_t;

// Operation Modes
typedef enum {
    BNO_MODE_CONFIG = 0x00,
    BNO_MODE_IMU = 0x08,
    BNO_MODE_AMG = 0x07
} bno055_operation_mode_t;

typedef struct bno055_handle_s
{
    uint8_t iic_addr = BNO055_I2C_ADDRESS;
    uint8_t operation_mode = BNO_MODE_AMG;
} bno055_handle_t;

// // --- Function Prototypes ---
// int bno_init(bno_operation_mode_t mode);
// int bno_set_mode(bno_operation_mode_t mode);
// int bno_read_accel(float *x, float *y, float *z);
// int bno_read_gyro(float *x, float *y, float *z);
// int bno_read_quat(float *w, float *x, float *y, float *z);
