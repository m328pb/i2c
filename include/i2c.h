#pragma once
#include <avr/io.h>

#define SSD1306_I2C_ADDRESS 0x3C
#define I2C_FREQ 100000L
#define TWSR_mask 0xF8 // mask for TWSR register (7..3 bits)
#define TWSR_code (I2C_status)(TWSR0 & TWSR_mask)

class I2C {
private:
  uint8_t addr;

public:
  I2C(uint8_t addr);
  // I2C protocol status codes (basically TWSR register bits)
  // see atmega328p datasheet, page 186, tab.21-3
  typedef enum : uint8_t {
    TW_NO_INFO = 0x00,
    TW_START = 0x08,
    TW_REP_START = 0x10,
    TW_ACK = 0x18,
    TW_NACK = 0x20,
    TW_DATA_ACK = 0x28,
    TW_DATA_NACK = 0x30,
    TW_ARBIT_LOST = 0x38
  } I2C_status;

  typedef enum : uint8_t {
    NO_ERR = 0,
    DEV_ERR = 1,
    COM_ERR = 2,
  } error;

  I2C_status status;
  void init();
  error write(uint8_t *data, uint8_t len);
  void stop();
};
