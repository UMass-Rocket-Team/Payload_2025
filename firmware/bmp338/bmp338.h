#include "pico/stdlib.h"
#include "hardware/i2c.h"

typedef enum {
	BMP338_REG_CMD           = 0x7E,
	BMP338_REG_CHIP_ID       = 0x00,
	BMP338_REG_ERR           = 0x02,
	BMP338_REG_STATUS        = 0x03,
	BMP338_REG_DATA_0        = 0x04,
	BMP338_REG_DATA_1        = 0x05,
	BMP338_REG_DATA_2        = 0x06,
	BMP338_REG_DATA_3        = 0x07,
	BMP338_REG_DATA_4        = 0X08,
	BMP338_REG_DATA_5        = 0X09,
	BMP338_REG_SENSORTIME_0  = 0X0C,
	BMP338_REG_SENSORTIME_1  = 0X0D,
	BMP338_REG_SENSORTIME_2  = 0X0E,
	BMP338_REG_EVENT         = 0X10,
	BMP338_REG_INT_STATUS    = 0X11,
	BMP338_REG_FIFO_LENGTH_0 = 0X12,
	BMP338_REG_FIFO_LENGTH_1 = 0X13,
	BMP338_REG_FIFO_DATA     = 0X14,
	BMP338_REG_FIFO_WTM_0    = 0X15,
	BMP338_REG_FIFO_WTM_1    = 0X16,
	BMP338_REG_FIFO_CONFIG_1 = 0X17,
	BMP338_REG_FIFO_CONFIG_2 = 0X18,
	BMP338_REG_INT_CTRL      = 0X19,
	BMP338_REG_IF_CONF       = 0X1A,
	BMP338_REG_PWR_CTRL      = 0X1B,
	BMP338_REG_OSR           = 0X1C,
	BMP338_REG_ODR           = 0X1D,
	BMP338_REG_CONFIG        = 0X1F

} bmp338_register_t;