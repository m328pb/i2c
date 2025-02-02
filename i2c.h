#pragma once
#include <avr/io.h>

#define SSD1306_I2C_ADDRESS 0x3C
#define I2C_FREQ 100000L

void mw_init(void);
bool mw_write(uint8_t *data, uint8_t len);


