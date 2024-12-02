#include "bmp338.h"

const i2c_inst_t* I2C_PORT = i2c0;
const uint SDA_PIN = 4;
const uint SCL_PIN = 5;
const uint I2C_BAUDRATE = 100000;

// Initialize
// Compensation values
// Pressure data

static uint8_t i2c_read(uint8_t device_addr, uint8_t reg, uint8_t* data, uint16_t len) {
	if (i2c_write_blocking(I2C_PORT, device_addr, &reg, 1, true) < 0) {
		return 1;
	}

	if (i2c_read_blocking(I2C_PORT, device_addr, data, len, false) < 0) {
		return 1;
	}
	return 0;
}

static uint8_t i2c_write(uint8_t device_addr, uint8_t reg, uint8_t *buf, uint8_t len) {
	for (i = 0; i < len; i++) {
		uint8_t buffer[2];
		buffer[0] = reg + i;
		buffer[1] = buf + i;
		if (i2c_write_blocking(I2C_PORT, device_addr, buffer, 2) < 0) {
			return 1;
		}
	}
	return 0;
}

int main(int argc, char const *argv[])
{
	stdio_init_all();
	i2c_init()
	return 0;
}