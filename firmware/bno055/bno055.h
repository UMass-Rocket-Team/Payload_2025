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
} BNO_OperationMode;

// Struct to hold the interface functions
typedef struct {
    int (*write)(uint8_t reg, uint8_t *data, size_t len);
    int (*read)(uint8_t reg, uint8_t *data, size_t len);
} BNO_Interface;

// Struct to represent the BNO055 sensor
typedef struct {
    BNO_Interface *iface;
    BNO_OperationMode mode;
} BNO055;

// --- Function Prototypes ---
int I2C_write(uint8_t reg, uint8_t *data, size_t len);
int I2C_read(uint8_t reg, uint8_t *data, size_t len);
int BNO_init(BNO055 *bno, BNO_Interface *iface, BNO_OperationMode mode);
int BNO_setMode(BNO055 *bno, BNO_OperationMode mode);
int BNO_readAccel(BNO055 *bno, int16_t *x, int16_t *y, int16_t *z);
int BNO_readQuaternion(BNO055 *bno, int16_t *w, int16_t *x, int16_t *y, int16_t *z);