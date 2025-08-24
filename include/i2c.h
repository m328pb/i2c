#pragma once
#include <avr/io.h>

// m328p and m328pb define registers slightly different
// becouse m328pb has two TWI units
#if defined(__AVR_ATmega328PB__)
#define _PRR PRR0
#define _PRTWI PRTWI0
#define _TWCR TWCR0
#define _TWSR TWSR0
#define _TWBR TWBR0
#define _TWDR TWDR0
#elif defined(__AVR_ATmega328P__)
#define _PRR PRR
#define _PRTWI PRTWI
#define _TWCR TWCR
#define _TWSR TWSR
#define _TWBR TWBR
#define _TWDR TWDR
#else
#error "Unsupported MCU"
#endif

#define SSD1306_I2C_ADDRESS 0x3C
#define PCF8574_I2C_ADDRESS 0x27
#define I2C_FREQ 100000L
#define TWSR_mask 0xF8 // mask for TWSR register (7..3 bits)

class I2C {
private:
  // I2C protocol status codes (basically TWSR register bits)
  // see atmega328p datasheet, page 186, tab.21-3
  uint8_t addr;
  uint8_t backup_TWIE; // to restore interrupt status

#if defined(ENABLE_I2C_SCAN)
  void scan();
#endif

  void start();
  void send_addr(uint8_t addr);
  uint8_t TWSR_code();

public:
  I2C(uint8_t addr);
#if defined(ENABLE_I2C_SCAN)
  I2C(); // will try to guess the address (scan all possibilities)
#endif

  typedef enum : uint8_t {
    NO_ERR = 0,
    DEV_ERR = 1,
    COM_ERR = 2,
  } error;
  error err;
  uint8_t force = false;

  void init();
  void write(uint8_t *data, uint8_t len);
  void stop();
  void off();
};
