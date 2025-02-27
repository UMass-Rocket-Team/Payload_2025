#pragma once

#include "hardware/i2c.h"

int ds1307_init(i2c_inst_t *i2c);

int ds1307_get_date(i2c_inst_t *i2c, uint8_t *day, uint8_t *month, uint8_t *year);
int ds1307_set_date(i2c_inst_t *i2c, uint8_t day, uint8_t month, uint8_t year);

int ds1307_get_time(i2c_inst_t *i2c, uint8_t *hour, uint8_t *minute, uint8_t *second);
int ds1307_set_time(i2c_inst_t *i2c, uint8_t hour, uint8_t minute, uint8_t second);
