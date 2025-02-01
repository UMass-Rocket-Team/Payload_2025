#include "bmp388.h"

i2c_inst_t* I2C_PORT = i2c0;

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

static float bmp388_compensate_temperature(bmp388_handle_t *handle, uint32_t data)
{ 
	float partial_1, partial_2;

    partial_1 = (float)(data - handle->t1);
    partial_2 = (float)(partial_1 * handle->t2);

    float temp = partial_2 + (partial_1 * partial_1) * handle->t3;
	handle->t = temp;
	return temp;
}

static int64_t bmp388_compensate_pressure(bmp388_handle_t *handle, uint32_t pres_uncomp)
{
    float partial_1, partial_2, partial_3, partial_4;
    float partial_out_1, partial_out_2;

    float pres_uncomp_flt = (float)pres_uncomp;
    float pres_uncomp_flt_2 = pres_uncomp_flt * pres_uncomp_flt;

	float temp = handle->t;
    float temp_squared = temp * temp;
    float temp_cubed = temp_squared * temp;

    partial_1 = handle->p6 * temp;
    partial_2 = handle->p7 * temp_squared;
    partial_3 = handle->p8 * temp_cubed;
    partial_out_1 = handle->p5 + partial_1 + partial_2 + partial_3;

    partial_1 = handle->p2 * temp;
    partial_2 = handle->p3 * temp_squared;
    partial_3 = handle->p4 * temp_cubed;

    partial_out_2 = pres_uncomp_flt * (handle->p1 + partial_1 + partial_2 + partial_3);

    partial_1 = pres_uncomp_flt_2;
    partial_2 = handle->p9 + handle->p10 * temp;
    partial_3 = partial_1 * partial_2;

    partial_4 = partial_3 + (pres_uncomp_flt_2 * pres_uncomp_flt) * handle->p11;
    float pres = partial_1 + partial_2 + partial_4;

	return pres;
}

static uint8_t bmp388_get_calibration_data(bmp388_handle_s *handle) {
	uint8_t buf[2];
	if (i2c_read(handle->iic_addr, BMP388_REG_NVM_PAR_T1_L, buf, 2) == 1) {
		printf("BMP388: Failed to read PAR_T1\n");
		return 1;
	}
	handle->t1 = (uint16_t)buf[1] << 8 | buf[0];

	if (i2c_read(handle->iic_addr, BMP388_REG_NVM_PAR_T2_L, buf, 2) == 1) {
		printf("BMP388: Failed to read PAR_T2\n");
		return 1;
	}
	handle->t2 = (uint16_t)buf[1] << 8 | buf[0];

	if (i2c_read(handle->iic_addr, BMP388_REG_NVM_PAR_T3, buf, 1) == 1) {
		printf("BMP388: Failed to read PAR_T3\n");
		return 1;
	}
	handle->t3 = (uint8_t)buf[0];
	
	if (i2c_read(handle->iic_addr, BMP388_REG_NVM_PAR_P1_L, buf, 2) == 1) {
		printf("BMP388: Failed to read PAR_P1\n");
		return 1;
	}
	handle->p1 = (uint16_t)buf[1] << 8 | buf[0];

	if (i2c_read(handle->iic_addr, BMP388_REG_NVM_PAR_P2_L, buf, 2) == 1) {
		printf("BMP388: Failed to read PAR_P2\n");
		return 1;
	}
	handle->p2 = (uint16_t)buf[1] << 8 | buf[0];

	if (i2c_read(handle->iic_addr, BMP388_REG_NVM_PAR_P3, buf, 1) == 1) {
		printf("BMP388: Failed to read PAR_P3\n");
		return 1;
	}
	handle->p3 = (uint8_t)buf[0];
	
	if (i2c_read(handle->iic_addr, BMP388_REG_NVM_PAR_P4, buf, 1) == 1) {
		printf("BMP388: Failed to read PAR_P4\n");
		return 1;
	}
	handle->p4 = (uint8_t)buf[0];

	if (i2c_read(handle->iic_addr, BMP388_REG_NVM_PAR_P5_L, buf, 2) == 1) {
		printf("BMP388: Failed to read PAR_P5\n");
		return 1;
	}
	handle->p5 = (uint16_t)buf[1] << 8 | buf[0];

	if (i2c_read(handle->iic_addr, BMP388_REG_NVM_PAR_P6_L, buf, 2) == 1) {
		printf("BMP388: Failed to read PAR_P6\n");
		return 1;
	}
	handle->p6 = (uint16_t)buf[1] << 8 | buf[0];

	if (i2c_read(handle->iic_addr, BMP388_REG_NVM_PAR_P7, buf, 1) == 1) {
		printf("BMP388: Failed to read PAR_P7\n");
		return 1;
	}
	handle->p7 = (uint8_t)buf[0];
	
	if (i2c_read(handle->iic_addr, BMP388_REG_NVM_PAR_P8, buf, 1) == 1) {
		printf("BMP388: Failed to read PAR_P8\n");
		return 1;
	}
	handle->p8 = (uint8_t)buf[0];

	if (i2c_read(handle->iic_addr, BMP388_REG_NVM_PAR_P9_L, buf, 2) == 1) {
		printf("BMP388: Failed to read PAR_P9\n");
		return 1;
	}
	handle->p9 = (uint16_t)buf[1] << 8 | buf[0];

	if (i2c_read(handle->iic_addr, BMP388_REG_NVM_PAR_P10, buf, 1) == 1) {
		printf("BMP388: Failed to read PAR_P10\n");
		return 1;
	}
	handle->p10 = (uint8_t)buf[0];
	
	if (i2c_read(handle->iic_addr, BMP388_REG_NVM_PAR_P11, buf, 1) == 1) {
		printf("BMP388: Failed to read PAR_P11\n");
		return 1;
	}
	handle->p11 = (uint8_t)buf[0];
	
	return 0;
}

uint8_t bmp388_read_temperature_pressure(bmp388_handle_s *handle, uint32_t *temp_raw, float *temp_c,
										 uint32_t *pressure_raw, float *pressure_pa) {
	uint8_t res;
	uint8_t prev;
	uint8_t buf[3];

	res = i2c_read(handle->iic_addr, BMP388_REG_STATUS, (uint8_t *)&prev, 1);
	if (res != 0) {
		printf("BMP388: Failed to get status register\n");
		return 1;
	}
	if (prev & (1 << 6) != 0) {
		int64_t output;
		res = i2c_read(handle->iic_addr, BMP388_REG_DATA_3, (uint8_t *)&prev, 3);
		if (res != 0) {
			printf("BMP388: Failed to read temperature data register\n");
			return 1;
		}
		*temp_raw = (uint32_t) buf[2] << 16 | (uint32_t) buf[1] << 8 |(uint32_t) buf[0];
		float comp = bmp388_compensate_temperature(handle, *temp_raw);
		*temp_c = (float)((double)output/100.0);
		
	} else {
		printf("BMP388: Temperature frame not ready\n");
		return 1;
	}
	if (prev & (1 << 5) != 0) {
		int64_t output;
		res = i2c_read(handle->iic_addr, BMP388_REG_DATA_0, (uint8_t *)&prev, 3);
		if (res != 0) {
			printf("BMP388: Failed to read pressure data register\n");
			return 1;
		}
		*pressure_raw = (uint32_t) buf[2] << 16 | (uint32_t) buf[1] << 8 |(uint32_t) buf[0];
		float comp = bmp388_compensate_pressure(handle, *pressure_raw);
		*pressure_pa = (float)((double)output/100.0);
	} else {
		printf("BMP388: pressure frame not ready\n");
		return 1;
	}
	return 0;
}

uint8_t bmp388_init(bmp388_handle_t *handle) {
	if (handle == NULL) {
		return 1;
	}
	if (bmp388_get_calibration_data(handle) != 0) {
		return 1;
	}
	return 0;
}
