#include "bno055.h"

uint8_t i2c_read(uint8_t device_addr, uint8_t reg, uint8_t* data, uint16_t len) {
	int ret = i2c_write_timeout_us(I2C_PORT, device_addr, &reg, 1, true, 1000);
	if (ret < 0) {
		printf("Write failed! \n");
		return 1;
	}

	ret = i2c_read_timeout_us(I2C_PORT, device_addr, data, len, false, 1000);
	if (ret < 0) {
		printf("Read failed! \n");
		return 1;
	}
	// printf("Read %d bytes! \n", ret);
	return 0;
}

static uint8_t i2c_write(uint8_t device_addr, uint8_t reg, uint8_t *buf, uint8_t len) {
	for (int i = 0; i < len; i++) {
		uint8_t buffer[2];
		buffer[0] = reg+i;
		memcpy(&buffer[1], buf, 1);
		if (i2c_write_blocking(I2C_PORT, device_addr, buffer, 2, false) < 0) {
			return 1;
		}
	}
	return 0;
}


int bno_set_mode(bno_operation_mode_t mode) {
    uint8_t data = mode;
    if (i2c_write(BNO_OPR_MODE, &data, 1) < 0) {
        return -1; // Failed to write mode
    }
    sleep_ms(30); // Allow mode to take effect
    bno->mode = mode;
    return 0; // Success
}

int bno_read_accel(float *x, float *y, float *z) {
    uint8_t data[6];
    if (i2c_read(BNO_ACCEL_DATA, data, 6) < 0) {
        return -1; // Read failed
    }
    *x = ((int16_t)((data[1] << 8) | data[0])) / 100.f;
    *y = ((int16_t)((data[3] << 8) | data[2])) / 100.f;
    *z = ((int16_t)((data[5] << 8) | data[4])) / 100.f;
    return 0; // Success
}

int bno_read_quaternion(BNO055 *bno, int16_t *w, int16_t *x, int16_t *y, int16_t *z) {
    uint8_t data[8];
    if (i2c_read(BNO_QUATERNION, data, 8) < 0) {
        return -1; // Read failed
    }
    *w = (int16_t)((data[1] << 8) | data[0]);
    *x = (int16_t)((data[3] << 8) | data[2]);
    *y = (int16_t)((data[5] << 8) | data[4]);
    *z = (int16_t)((data[7] << 8) | data[6]);
    return 0; // Success
}
