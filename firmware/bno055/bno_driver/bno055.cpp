#include "bno055.h"

static uint8_t i2c_read(uint8_t device_addr, uint8_t reg, uint8_t* data, uint16_t len) {
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

uint8_t bno055_set_mode(bno055_handle_t *handle, bno055_operation_mode_t mode) {
    uint8_t buf = mode;
    if(i2c_write(handle->iic_addr, BNO_OPR_MODE, &buf, 1)) {
        printf("BNO: Write operation mode failed.\n");
        return 1;
    };
    if(i2c_read(handle->iic_addr, BNO_OPR_MODE, &(handle->operation_mode), 1)) {
        printf("BNO: Verify operation mode failed.\n");
        return 1;
    }
    if (handle->operation_mode != mode) {
        printf("BNO: Mode updated incorrectly.\n");
        return 1;
    }
    return 0;
}

uint8_t bno055_read_acceleration(bno055_handle_t *handle, float *x, float *y, float *z) {
    uint8_t buf[2];
    if (i2c_read(handle->iic_addr, BNO_ACCEL_X, (uint8_t *)buf, 2)) {
        printf("BNO: X-axis acceleration read failed!");
        return 1;
    }
    *x = (int16_t)(buf[1] << 8 | buf[0]) / 100.0;

    if (i2c_read(handle->iic_addr, BNO_ACCEL_Y, (uint8_t *)buf, 2)) {
        printf("BNO: Y-axis acceleration read failed!");
        return 1;
    }
    *y = (int16_t)(buf[1] << 8 | buf[0]) / 100.0;

    if (i2c_read(handle->iic_addr, BNO_ACCEL_Z, (uint8_t *)buf, 2)) {
        printf("BNO: Z-axis acceleration read failed!");
        return 1;
    }
    *z = (int16_t)(buf[1] << 8 | buf[0]) / 100.0;
    
    return 0;
}

uint8_t bno055_read_omega(bno055_handle_t *handle, float *x, float *y, float *z) {
    uint8_t buf[2];
    if (i2c_read(handle->iic_addr, BNO_OMEGA_X, (uint8_t *)buf, 2)) {
        printf("BNO: X-axis acceleration read failed!");
        return 1;
    }
    *x = (int16_t)(buf[1] << 8 | buf[0]) / 16.0;

    if (i2c_read(handle->iic_addr, BNO_OMEGA_Y, (uint8_t *)buf, 2)) {
        printf("BNO: Y-axis acceleration read failed!");
        return 1;
    }
    *y = (int16_t)(buf[1] << 8 | buf[0]) / 16.0;   

    if (i2c_read(handle->iic_addr, BNO_OMEGA_Z, (uint8_t *)buf, 2)) {
        printf("BNO: Z-axis acceleration read failed!");
        return 1;
    }
    *z = (int16_t)(buf[1] << 8 | buf[0]) / 16.0;
    
    return 0;
}

uint8_t bno055_read_ypr(bno055_handle_t *handle, float *yaw, float *pitch, float *roll) {

    // uint8_t u;
    // i2c_read(handle->iic_addr, BNO_UNIT_SEL, &u, 1);
    // printf("Unit: %u \n", u);

    uint8_t buf[2];
    if (i2c_read(handle->iic_addr, BNO_YAW, (uint8_t *)buf, 2)) {
        printf("BNO: Yaw read failed!");
        return 1;
    }
    *yaw = (int16_t)(buf[1] << 8 | buf[0]) / 16.0;

    if (i2c_read(handle->iic_addr, BNO_PITCH, (uint8_t *)buf, 2)) {
        printf("BNO: Pitch read failed!");
        return 1;
    }
    *pitch = (int16_t)(buf[1] << 8 | buf[0]) / 16.0;

    if (i2c_read(handle->iic_addr, BNO_ROLL, (uint8_t *)buf, 2)) {
        printf("BNO: Roll read failed!");
        return 1;
    }
    *roll = (int16_t)(buf[1] << 8 | buf[0]) / 16.0;

    return 0;
}

uint8_t bno055_read_quat(bno055_handle_t *handle, float *w, float *x, float *y, float *z) {
    uint8_t buf[2];
    if (i2c_read(handle->iic_addr, BNO_QUAT_W, (uint8_t *)buf, 2)) {
        printf("BNO: Quat W read failed!");
        return 1;
    }
    *w = (int16_t)(buf[1] << 8 | buf[0]);

    if (i2c_read(handle->iic_addr, BNO_QUAT_X, (uint8_t *)buf, 2)) {
        printf("BNO: Quat X read failed!");
        return 1;
    }
    *x = (int16_t)(buf[1] << 8 | buf[0]);

    if (i2c_read(handle->iic_addr, BNO_QUAT_Y, (uint8_t *)buf, 2)) {
        printf("BNO: Quat Y read failed!");
        return 1;
    }
    *y = (int16_t)(buf[1] << 8 | buf[0]);

    if (i2c_read(handle->iic_addr, BNO_QUAT_Z, (uint8_t *)buf, 2)) {
        printf("BNO: Quat Z failed!");
        return 1;
    }
    *z = (int16_t)(buf[1] << 8 | buf[0]);

    return 0;
}

uint8_t bno055_init(bno055_handle_t *handle) {
    i2c_init(I2C_PORT, 100 * 1000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    if (handle == NULL) {
        return 1;
    }

    uint8_t pwr_mode = 0x00;
    if (i2c_write(handle->iic_addr, BNO_PWR_MODE, &pwr_mode, 1) != 0) {
        printf("BNO055: Setting power mode failed!\n");
        return 1;
    }

    uint8_t unit_sel = (handle->use_radians) << 2;
    if (i2c_write(handle->iic_addr, BNO_UNIT_SEL, &unit_sel, 1) != 0) {
        printf("BNO055: Setting unit mode failed!\n");
        return 1;
    }

    if (bno055_set_mode(handle, BNO_MODE_IMU)) {
        printf("BNO055: Setting operation mode failed!\n");
        return 1;
    }
    return 0;
}
