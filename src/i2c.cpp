#include "i2c.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>

I2C::I2C(uint8_t addr) { this->addr = addr; }

#if defined(ENABLE_I2C_SCAN)
I2C::I2C() { scan(); }

void I2C::scan() {
  force = false;
  for (uint8_t i = 0x20; i <= 0x27; i++) {
    init();
    start();
    send_addr(i);
    if (err == NO_ERR) {
      this->addr = i;
      stop();
      return;
    }
  };
}
#endif

void I2C::off() {
  _PRR |= 1 << _PRTWI;  // turn off TWI
  _TWCR |= backup_TWIE; // restore interrupts
}

void I2C::init() {
  _PRR &= ~(1 << _PRTWI); // turn on TWI
  backup_TWIE = _TWCR & (1 << TWIE);
  _TWCR &= ~(1 << TWIE); // turn off interrupts

  /* twi bit rate formula from atmega328p manual p.180
  SCL Frequency = CPU Clock Frequency / (16 + (2 * TWBR))
  SCL frequency up to 400kHz on atmega328p
  note: TWBR should be 10 or higher for master mode*/
  _TWSR &= ~(1 << TWPS0); // initialize twi prescaler and bit rate
  _TWSR &= ~(1 << TWPS1); // set prescaler to 1
  _TWBR = ((F_CPU / I2C_FREQ) - 16) / 2; // 100kHz

  // activate internal pullups for twi.
  PORTC |= (1 << PC4); // pullup for sda
  PORTC |= (1 << PC5); // pullup for scl
}

/**
 * @brief Send data to the slave device over I2C
 * @param data 8 bit data to send
 * @param len number of bytes to send
 * @return error code (see error struct, 0 on success)
 * @details
 * This function sends data to the slave device over I2C.
 * It sends a start condition, the slave device address (with W bit set),
 * the data, and a stop condition. It returns error struct.
 */

uint8_t I2C::TWSR_code() {
  // return TRUE when ACK recived or FALSE
  // I2C status is in TWSR register (p.200 ATmega328P datasheet)
  // Always return TRUE if forced.
  // before you start refactoring: using direct casting to enum type,
  // like return (I2C_status)(TWSR & TWSR_mask) is not safe,
  // unless all possible values will be defined in enum variable
  if (force)
    return 1;
  uint8_t code = _TWSR & TWSR_mask;
  switch (code) {
  case 0x08: // TW_START
    return 1;
  case 0x18: // TW_ACK
    return 1;
  case 0x28: // TW_DATA_ACK
    return 1;
  default:
    return 0;
  }
}

void I2C::start() {
  // Master Transmitter Mode (atmega328p, p.185)
  // send start condition
  err = NO_ERR;
  _TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
  while (!(_TWCR & (1 << TWINT)))
    ;
  if (!TWSR_code()) // A START condition has been transmitted?
  {
    stop();
    err = DEV_ERR;
  };
}

void I2C::send_addr(uint8_t addr) {
  // build sla+w, slave device address + w bit (0 write to slave)
  _TWDR = addr << 1;
  // transmit address
  _TWCR = (1 << TWINT) | (1 << TWEN);
  while (!(_TWCR & (1 << TWINT)))
    ;
  if (!TWSR_code()) // SLA+W transmitted, ACK received?
  {
    stop();
    err = DEV_ERR;
  };
}

void I2C::write(uint8_t *data, uint8_t len)
// must be 8 bit data
{
  start();
  if (err != NO_ERR)
    return;

  send_addr(this->addr);
  if (err != NO_ERR)
    return;

  // send data
  while (len--) {
    _TWDR = *data++;
    _TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(_TWCR & (1 << TWINT)))
      ;
    if (!TWSR_code()) // data byte transmitted, ACK received?
    {
      stop();
      err = COM_ERR;
      return;
    };
  };
  // send stop condition
  stop();
}

void I2C::stop() {
  _TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
  // wait for STOP flag cleared: STOP transmition completed
  // otherway can be interpreted as REPEATED START
  while (_TWCR & (1 << TWSTO))
    ;
}
