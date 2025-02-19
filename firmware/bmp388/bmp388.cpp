#include "bmp388.h"

i2c_inst_t* I2C_PORT = i2c0;

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

static float bmp388_compensate_temperature(bmp388_handle_t *handle, uint32_t data)
{ 
    uint64_t partial_data1;
    uint64_t partial_data2;
    uint64_t partial_data3;
    int64_t partial_data4;
    int64_t partial_data5;
    int64_t partial_data6;
    int64_t comp_temp;

    /* calculate compensate temperature */
    partial_data1 = (uint64_t)(data - (256 * (uint64_t)(handle->t1)));
    partial_data2 = (uint64_t)(handle->t2 * partial_data1);
    partial_data3 = (uint64_t)(partial_data1 * partial_data1);
    partial_data4 = (int64_t)(((int64_t)partial_data3) * ((int64_t)handle->t3));
    partial_data5 = ((int64_t)(((int64_t)partial_data2) * 262144) + (int64_t)partial_data4);
    partial_data6 = (int64_t)(((int64_t)partial_data5) / 4294967296U);
    handle->t = partial_data6;
    comp_temp = (int64_t)((partial_data6 * 25)  / 16384);
    
    return comp_temp;
}

static int64_t bmp388_compensate_pressure(bmp388_handle_t *handle, uint32_t data){
    int64_t partial_data1;
    int64_t partial_data2;
    int64_t partial_data3;
    int64_t partial_data4;
    int64_t partial_data5;
    int64_t partial_data6;
    int64_t offset;
    int64_t sensitivity;
    uint64_t comp_press;

    /* calculate compensate pressure */
    partial_data1 = handle->t * handle->t;
    partial_data2 = partial_data1 / 64;
    partial_data3 = (partial_data2 * handle->t) / 256;
    partial_data4 = (handle->p8 * partial_data3) / 32;
    partial_data5 = (handle->p7 * partial_data1) * 16;
    partial_data6 = (handle->p6 * handle->t) * 4194304;
    offset = (int64_t)((int64_t)(handle->p5) * (int64_t)140737488355328U) + partial_data4 + partial_data5 + partial_data6;
    partial_data2 = (((int64_t)handle->p4) * partial_data3) / 32;
    partial_data4 = (handle->p3 * partial_data1) * 4;
    partial_data5 = ((int64_t)(handle->p2) - 16384) * ((int64_t)handle->t) * 2097152;
    sensitivity = (((int64_t)(handle->p1) - 16384) * (int64_t)70368744177664U) + partial_data2 + partial_data4 + partial_data5;
    partial_data1 = (sensitivity / 16777216) * data;
    partial_data2 = (int64_t)(handle->p10) * (int64_t)(handle->t);
    partial_data3 = partial_data2 + (65536 * (int64_t)(handle->p9));
    partial_data4 = (partial_data3 * data) / 8192;
    partial_data5 = (partial_data4 * data) / 512;
    partial_data6 = (int64_t)((uint64_t)data * (uint64_t)data);
    partial_data2 = ((int64_t)(handle->p11) * (int64_t)(partial_data6)) / 65536;
    partial_data3 = (partial_data2 * data) / 128;
    partial_data4 = (offset / 4) + partial_data1 + partial_data5 + partial_data3;
    comp_press = (((uint64_t)partial_data4 * 25) / (uint64_t)1099511627776U);
    
    return comp_press;
}

static uint8_t bmp388_get_calibration_data(bmp388_handle_s *handle) {
	uint8_t buf[2];
	if (i2c_read(0x77, BMP388_REG_NVM_PAR_T1_L, buf, 2) == 1) {
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

	int64_t output;
	res = i2c_read(handle->iic_addr, BMP388_REG_DATA_3, (uint8_t *)buf, 3);
	if (res != 0) {
		printf("BMP388: Failed to read temperature data register\n");
		return 1;
	}
	*temp_raw = (uint32_t) buf[2] << 16 | (uint32_t) buf[1] << 8 |(uint32_t) buf[0];
	output = bmp388_compensate_temperature(handle, *temp_raw);
	*temp_c = (float)((double)output/100.0);
	
	res = i2c_read(handle->iic_addr, BMP388_REG_DATA_0, (uint8_t *)buf, 3);
	if (res != 0) {
		printf("BMP388: Failed to read pressure data register\n");
		return 1;
	}
	*pressure_raw = (uint32_t) buf[2] << 16 | (uint32_t) buf[1] << 8 |(uint32_t) buf[0];
	output = bmp388_compensate_pressure(handle, *pressure_raw);
	*pressure_pa = (float)((double)output/100.0);

	return 0;
}

uint8_t bmp388_init(bmp388_handle_t *handle) {
	i2c_init(I2C_PORT, 100 * 1000);
  	gpio_set_function(4, GPIO_FUNC_I2C);
    gpio_set_function(5, GPIO_FUNC_I2C);
    gpio_pull_up(4);
    gpio_pull_up(5);

	if (handle == NULL) {
		return 1;
	}
	uint8_t data = 255;
	int ret = i2c_write(handle->iic_addr, BMP388_REG_PWR_CTRL, &(data), 1);
	while(ret != 0) {
		printf("BMP388: Enabling sensors failed, retrying... \n");
	}
	if (bmp388_get_calibration_data(handle) != 0) {
		return 1;
	}
	return 0;
}
