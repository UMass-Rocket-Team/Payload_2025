#include "DS1307.h"

const static uint8_t DS1307_ADDR = 0x68;

#define DS1307_CTRL 0x07
#define DS1307_SEC 0x00

static uint8_t bcd_to_decimal(uint8_t bcd) {
    return (bcd >> 4) * 10 + (bcd & 0x0F);
}

static uint8_t decimal_to_bcd(uint8_t decimal) {
    return ((decimal / 10) << 4) | (decimal % 10);
}

static int read(i2c_inst_t *i2c, uint8_t reg, uint8_t *data, size_t len) {
    int ret = i2c_write_blocking(i2c, DS1307_ADDR, &reg, 1, true);
    if (ret < 0) { 
        printf("Failed to write DS1307 register\n");
        return ret; 
    }
    ret = i2c_read_blocking(i2c, DS1307_ADDR, data, len, false);
    if (ret < 0) { 
        printf("Failed to read DS1307 data\n");
    }
    return ret;
}

static int write(i2c_inst_t *i2c, uint8_t reg, uint8_t *data, size_t len) {
    uint8_t buf[len + 1];
    buf[0] = reg;
    for ( int i = 0; i < len; i++ ) { buf[i + 1] = data[i]; }
    return i2c_write_blocking(i2c, DS1307_ADDR, buf, len + 1, false);
}

int ds1307_init(i2c_inst_t *i2c) {
    uint8_t ctrl = 0x00;
    int ret = read(i2c, DS1307_CTRL, &ctrl, 1);
    if (ret < 0) { 
        printf("Failed to read DS1307_CTRL\n");
        printf("ret: %d\n", ret);
        return ret; 
    }
    ctrl &= ~(1 << 7);

    ret = write(i2c, DS1307_CTRL, &ctrl, 1);
    if (ret < 0) { 
        printf("Failed to write DS1307_CTRL\n");
        printf("ret: %d\n", ret);
        return ret; 
    }

    return 0; 
}

int ds1307_get_date(i2c_inst_t *i2c, uint8_t *day, uint8_t *month, uint8_t *year) {
    uint8_t data[3];
    int ret = read(i2c, DS1307_SEC, data, 3);
    if (ret < 0) { return ret; }
    *day = bcd_to_decimal(data[0]);
    *month = bcd_to_decimal(data[1]);
    *year = bcd_to_decimal(data[2]);
    return 0;
}

int ds1307_get_time(i2c_inst_t *i2c, uint8_t *hour, uint8_t *minute, uint8_t *second) {
    uint8_t data[3];
    int ret = read(i2c, DS1307_SEC, data, 3);
    if (ret < 0) { return ret; }
    *hour = bcd_to_decimal(data[0]);
    *minute = bcd_to_decimal(data[1]);
    *second = bcd_to_decimal(data[2]);
    return 0;
}

int ds1307_set_time(i2c_inst_t *i2c, uint8_t hour, uint8_t minute, uint8_t second) {
    uint8_t data[3] = { decimal_to_bcd(hour), decimal_to_bcd(minute), decimal_to_bcd(second) };
    return write(i2c, DS1307_SEC, data, 3);
}

int ds1307_set_date(i2c_inst_t *i2c, uint8_t day, uint8_t month, uint8_t year) {
    uint8_t data[3] = { decimal_to_bcd(day), decimal_to_bcd(month), decimal_to_bcd(year) };
    return write(i2c, DS1307_SEC, data, 3);
}
