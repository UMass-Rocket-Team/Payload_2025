#include <cstring>
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include "pico/stdio.h"

typedef enum {
	BMP338_REG_CMD           = 0x7E,
	BMP338_REG_CHIP_ID       = 0x00,
	BMP338_REG_ERR           = 0x02,
	BMP338_REG_STATUS        = 0x03,
	BMP338_REG_DATA_0        = 0x04,
	BMP338_REG_DATA_1        = 0x05,
	BMP338_REG_DATA_2        = 0x06,
	BMP338_REG_DATA_3        = 0x07,
	BMP338_REG_DATA_4        = 0x08,
	BMP338_REG_DATA_5        = 0x09,
	BMP338_REG_SENSORTIME_0  = 0x0C,
	BMP338_REG_SENSORTIME_1  = 0x0D,
	BMP338_REG_SENSORTIME_2  = 0x0E,
	BMP338_REG_EVENT         = 0x10,
	BMP338_REG_INT_STATUS    = 0x11,
	BMP338_REG_FIFO_LENGTH_0 = 0x12,
	BMP338_REG_FIFO_LENGTH_1 = 0x13,
	BMP338_REG_FIFO_DATA     = 0x14,
	BMP338_REG_FIFO_WTM_0    = 0x15,
	BMP338_REG_FIFO_WTM_1    = 0x16,
	BMP338_REG_FIFO_CONFIG_1 = 0x17,
	BMP338_REG_FIFO_CONFIG_2 = 0x18,
	BMP338_REG_INT_CTRL      = 0x19,
	BMP338_REG_IF_CONF       = 0x1A,
	BMP338_REG_PWR_CTRL      = 0x1B,
	BMP338_REG_OSR           = 0x1C,
	BMP338_REG_ODR           = 0x1D,
	BMP338_REG_CONFIG        = 0x1F,
	BMP388_REG_NVM_PAR_T1_L  = 0x31,
	BMP388_REG_NVM_PAR_T1_H  = 0x32,
	BMP388_REG_NVM_PAR_T2_L  = 0x33,
	BMP388_REG_NVM_PAR_T2_H  = 0x34,
	BMP388_REG_NVM_PAR_T3    = 0x35,
	BMP388_REG_NVM_PAR_P1_L  = 0x36,
	BMP388_REG_NVM_PAR_P1_H  = 0x37,
	BMP388_REG_NVM_PAR_P2_L  = 0x38,
	BMP388_REG_NVM_PAR_P2_H  = 0x39,
	BMP388_REG_NVM_PAR_P3    = 0x3A,
	BMP388_REG_NVM_PAR_P4    = 0x3B,
	BMP388_REG_NVM_PAR_P5_L  = 0x3C,
	BMP388_REG_NVM_PAR_P5_H  = 0x3D,
	BMP388_REG_NVM_PAR_P6_L  = 0x3E,
	BMP388_REG_NVM_PAR_P6_H  = 0x3F,
	BMP388_REG_NVM_PAR_P7    = 0x40,
	BMP388_REG_NVM_PAR_P8    = 0x41,
	BMP388_REG_NVM_PAR_P9_L  = 0x42,
	BMP388_REG_NVM_PAR_P9_H  = 0x43,
	BMP388_REG_NVM_PAR_P10   = 0x44,
	BMP388_REG_NVM_PAR_P11   = 0x45

} bmp338_register_t;

/**
 * @brief bmp388 handle structure definition
 */
typedef struct bmp388_handle_s
{
    uint8_t iic_addr;                                                                   /**< iic device address */
    uint8_t inited;                                                                     /**< inited flag */
    uint8_t iic_spi;                                                                    /**< iic spi interface */
    uint16_t t1;                                                                        /**< t1 register */
    uint16_t t2;                                                                        /**< t2 register */
    int8_t t3;                                                                          /**< t3 register */
    int16_t p1;                                                                         /**< p1 register */
    int16_t p2;                                                                         /**< p2 register */
    int8_t p3;                                                                          /**< p3 register */
    int8_t p4;                                                                          /**< p4 register */
    uint16_t p5;                                                                        /**< p5 register */
    uint16_t p6;                                                                        /**< p6 register */
    int8_t p7;                                                                          /**< p7 register */
    int8_t p8;                                                                          /**< p8 register */
    int16_t p9;                                                                         /**< p9 register */
    int8_t p10;                                                                         /**< p10 register */
    int8_t p11;                                                                         /**< p11 register */
    int64_t t_lin;                                                                      /**< t_lin register */
} bmp388_handle_t;