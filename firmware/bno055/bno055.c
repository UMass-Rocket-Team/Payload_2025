#include "bno055.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"


// --- I2C Communication Functions ---
int I2C_write(uint8_t reg, uint8_t *data, size_t len) {
    uint8_t buffer[len + 1];
    buffer[0] = reg;
    memcpy(&buffer[1], data, len);
    int result = i2c_write_blocking(I2C_PORT, BNO055_I2C_ADDRESS, buffer, len + 1, false);
    return result < 0 ? -1 : 0;
}

int I2C_read(uint8_t reg, uint8_t *data, size_t len) {  // Might not work
    uint8_t tmp = reg;
    if (i2c_write_blocking(I2C_PORT, BNO055_I2C_ADDRESS, &tmp, 1, true) < 0) {
        return -1;
    }
    int result = i2c_read_blocking(I2C_PORT, BNO055_I2C_ADDRESS, data, len, false);
    return result < 0 ? -1 : 0;
}

// --- BNO055 Driver Functions ---
int BNO_init(BNO055 *bno, BNO_Interface *iface, BNO_OperationMode mode) {
    
    if ( bno == NULL ) { return -1; }
    if ( iface == NULL ) { return -1; }
    if ( iface->read == NULL || iface->write == NULL) { return -1; }

    bno->iface = iface;

    // Verify chip ID
    uint8_t chip_id = 0;
    if (bno->iface->read(BNO_CHIP_ID, &chip_id, 1) < 0 || chip_id != 0xA0) {
        return -1; // Chip ID mismatch
    }

    // Switch to CONFIG mode
    if (BNO_setMode(bno, BNO_MODE_CONFIG) < 0) {
        return -1;
    }

    sleep_ms(30); // Allow mode switch

    // Switch to desired mode
    if (BNO_setMode(bno, mode) < 0) {
        return -1;
    }

    return 0; // Success
}

int BNO_setMode(BNO055 *bno, BNO_OperationMode mode) {
    uint8_t data = mode;
    if (bno->iface->write(BNO_OPR_MODE, &data, 1) < 0) {
        return -1; // Failed to write mode
    }
    sleep_ms(30); // Allow mode to take effect
    bno->mode = mode;
    return 0; // Success
}

int BNO_readAccel(BNO055 *bno, int16_t *x, int16_t *y, int16_t *z) {
    uint8_t data[6];
    if (bno->iface->read(BNO_ACCEL_DATA, data, 6) < 0) {
        return -1; // Read failed
    }
    *x = (int16_t)((data[1] << 8) | data[0]);
    *y = (int16_t)((data[3] << 8) | data[2]);
    *z = (int16_t)((data[5] << 8) | data[4]);
    return 0; // Success
}

int BNO_readQuaternion(BNO055 *bno, int16_t *w, int16_t *x, int16_t *y, int16_t *z) {
    uint8_t data[8];
    if (bno->iface->read(BNO_QUATERNION, data, 8) < 0) {
        return -1; // Read failed
    }
    *w = (int16_t)((data[1] << 8) | data[0]);
    *x = (int16_t)((data[3] << 8) | data[2]);
    *y = (int16_t)((data[5] << 8) | data[4]);
    *z = (int16_t)((data[7] << 8) | data[6]);
    return 0; // Success
}

// --- Main Function ---
int main() {
    stdio_init_all();

    // Initialize I2C
    i2c_init(I2C_PORT, 400 * 1000); // 400 kHz
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SCL_PIN);
    gpio_pull_up(SDA_PIN);

    // Initialize BNO055
    BNO_Interface i2c_iface = { .write = I2C_write, .read = I2C_read };
    BNO055 bno;

    if (BNO_init(&bno, &i2c_iface, BNO_MODE_NDOF) != 0) {
        printf("Failed to initialize BNO055.\n");
        return -1;
    }
    printf("BNO055 initialized successfully.\n");

    // Example: Read accelerometer data
    int16_t x, y, z;
    if (BNO_readAccel(&bno, &x, &y, &z) == 0) {
        printf("Accel X: %d, Y: %d, Z: %d\n", x, y, z);
    } else {
        printf("Failed to read accelerometer data.\n");
    }

    // Example: Read quaternion data
    int16_t w, qx, qy, qz;
    if (BNO_readQuaternion(&bno, &w, &qx, &qy, &qz) == 0) {
        printf("Quaternion W: %d, X: %d, Y: %d, Z: %d\n", w, qx, qy, qz);
    } else {
        printf("Failed to read quaternion data.\n");
    }

    return 0;
}
