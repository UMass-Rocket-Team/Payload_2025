#pragma once
#include <stdint.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// --- Constants and Definitions ---
#define BNO055_I2C_ADDRESS  0x28
#define I2C_PORT            i2c0
#define SCL_PIN             21  // Adjust based on wiring
#define SDA_PIN             20  // Adjust based on wiring

// BNO055 Register Map
#define BNO_CHIP_ID         0x00
#define BNO_ACCEL_DATA      0x08
#define BNO_QUATERNION      0x20
#define BNO_OPR_MODE        0x3D

// Operation Modes
typedef enum {
    BNO_MODE_CONFIG = 0x00,
    BNO_MODE_IMU = 0x08,
    BNO_MODE_NDOF = 0x0C, // Sensor Fusion
    BNO_MODE_AMG = 0x07
} bno_operation_mode_t;

// --- Function Prototypes ---
int bno_init(bno_operation_mode_t mode);
int bno_set_mode(bno_operation_mode_t mode);
int bno_read_accel(float *x, float *y, float *z);
int bno_read_gyro(float *x, float *y, float *z);
int bno_read_quat(float *w, float *x, float *y, float *z);
