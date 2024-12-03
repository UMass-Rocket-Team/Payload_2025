#include "bmp388.h"

i2c_inst_t* I2C_PORT = i2c0;
uint SDA_PIN = 4;
uint SCL_PIN = 5;
uint I2C_BAUDRATE = 100000;

// Initialize
// Compensation values
// Pressure data

// will add spi function later
static uint8_t i2c_spi_read(uint8_t device_addr, uint8_t reg, uint8_t* data, uint16_t len) {
	if (i2c_write_blocking(I2C_PORT, device_addr, &reg, 1, true) < 0) {
		return 1;
	}

	if (i2c_read_blocking(I2C_PORT, device_addr, data, len, false) < 0) {
		return 1;
	}
	return 0;
}

// will add spi function later
static uint8_t i2c_spi_write(uint8_t device_addr, uint8_t reg, uint8_t *buf, uint8_t len) {
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

static uint8_t get_calibration_data(bmp388_handle_s *handle) {
	uint8_t buf[2];
	if (i2c_spi_read(handle->iic_addr, BMP388_REG_NVM_PAR_T1_L, buf, 2) == 1) {
		printf("BMP388: Failed to read PAR_T1\n");
		return 1;
	}
	handle->t1 = (uint16_t)buf[1] << 8 | buf[0];

	if (i2c_spi_read(handle->iic_addr, BMP388_REG_NVM_PAR_T2_L, buf, 2) == 1) {
		printf("BMP388: Failed to read PAR_T2\n");
		return 1;
	}
	handle->t2 = (uint16_t)buf[1] << 8 | buf[0];

	if (i2c_spi_read(handle->iic_addr, BMP388_REG_NVM_PAR_T3, buf, 1) == 1) {
		printf("BMP388: Failed to read PAR_T3\n");
		return 1;
	}
	handle->t3 = (uint8_t)buf[0];
	
	if (i2c_spi_read(handle->iic_addr, BMP388_REG_NVM_PAR_P1_L, buf, 2) == 1) {
		printf("BMP388: Failed to read PAR_P1\n");
		return 1;
	}
	handle->p1 = (uint16_t)buf[1] << 8 | buf[0];

	if (i2c_spi_read(handle->iic_addr, BMP388_REG_NVM_PAR_P2_L, buf, 2) == 1) {
		printf("BMP388: Failed to read PAR_P2\n");
		return 1;
	}
	handle->p2 = (uint16_t)buf[1] << 8 | buf[0];

	if (i2c_spi_read(handle->iic_addr, BMP388_REG_NVM_PAR_P3, buf, 1) == 1) {
		printf("BMP388: Failed to read PAR_P3\n");
		return 1;
	}
	handle->p3 = (uint8_t)buf[0];
	
	if (i2c_spi_read(handle->iic_addr, BMP388_REG_NVM_PAR_P4, buf, 1) == 1) {
		printf("BMP388: Failed to read PAR_P4\n");
		return 1;
	}
	handle->p4 = (uint8_t)buf[0];

	if (i2c_spi_read(handle->iic_addr, BMP388_REG_NVM_PAR_P5_L, buf, 2) == 1) {
		printf("BMP388: Failed to read PAR_P5\n");
		return 1;
	}
	handle->p5 = (uint16_t)buf[1] << 8 | buf[0];

	if (i2c_spi_read(handle->iic_addr, BMP388_REG_NVM_PAR_P6_L, buf, 2) == 1) {
		printf("BMP388: Failed to read PAR_P6\n");
		return 1;
	}
	handle->p6 = (uint16_t)buf[1] << 8 | buf[0];

	if (i2c_spi_read(handle->iic_addr, BMP388_REG_NVM_PAR_P7, buf, 1) == 1) {
		printf("BMP388: Failed to read PAR_P7\n");
		return 1;
	}
	handle->p7 = (uint8_t)buf[0];
	
	if (i2c_spi_read(handle->iic_addr, BMP388_REG_NVM_PAR_P8, buf, 1) == 1) {
		printf("BMP388: Failed to read PAR_P8\n");
		return 1;
	}
	handle->p8 = (uint8_t)buf[0];

	if (i2c_spi_read(handle->iic_addr, BMP388_REG_NVM_PAR_P9_L, buf, 2) == 1) {
		printf("BMP388: Failed to read PAR_P9\n");
		return 1;
	}
	handle->p9 = (uint16_t)buf[1] << 8 | buf[0];

	if (i2c_spi_read(handle->iic_addr, BMP388_REG_NVM_PAR_P10, buf, 1) == 1) {
		printf("BMP388: Failed to read PAR_P10\n");
		return 1;
	}
	handle->p10 = (uint8_t)buf[0];
	
	if (i2c_spi_read(handle->iic_addr, BMP388_REG_NVM_PAR_P11, buf, 1) == 1) {
		printf("BMP388: Failed to read PAR_P11\n");
		return 1;
	}
	handle->p11 = (uint8_t)buf[0];
	
	return 0;
}

static uint8_t read_temperature_pressure(bmp388_handle_s *handle, uint32_t *temp_raw, float *temp_c,
										 uint32_t *pressure_raw, float *pressure_pa) {
	uint8_t res;
	uint8_t prev;
	uint8_t buf;

	res = i2c_spi_read(handle->iic_addr, BMP388_REG_STATUS, (uint8_t *)&prev, 1);
	if (res != 0) {
		printf("BMP388: Failed to get status register\n");
		return 1;
	}
	if (prev & (1 << 6) != 0) {
		int64_t output;
		res = i2c_spi_read(handle->iic_addr, BMP388_REG_DATA_3, (uint8_t *)&prev, 3);
		if (res != 0) {
			printf("BMP388: Failed to read temperature data register\n");
			return 1;
		}
	} else {
		printf("BMP388: Temperature frame not ready\n");
		return 1;
	}
	if (prev & (1 << 5) != 0) {
		int64_t output;
		res = i2c_spi_read(handle->iic_addr, BMP388_REG_DATA_0, (uint8_t *)&prev, 3);
		if (res != 0) {
			printf("BMP388: Failed to read pressure data register\n");
			return 1;
		}
	} else {
		printf("BMP388: pressure frame not ready\n");
		return 1;
	}
}

int main(int argc, char const *argv[])
{
	stdio_init_all();
	return 0;
}