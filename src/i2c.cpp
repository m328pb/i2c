#include "i2c.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>

I2C::I2C() {}

#if defined(ENABLE_I2C_SCAN)
/*
 * @brief scan for device address
 * @return adress if found or zero if failed
 * @details
 * when ACK recived set the address. Do not forget to init() after.
 *
 */
uint8_t I2C::scan() {
  for (uint8_t i = 0x20; i <= 0x27; i++) {
    init();
    start();
    send_addr(i);
    if (err == NO_ERR) {
      this->addr = i;
      stop();
      return i;
    }
  }
  return 0;
}
#endif

/*
* @brief initialize with given parameters
* @params addr device address
* @params speed communication speed in !!kHz!!
* @details
* set device address and speed. Max speed is 400kHz theoretically
* but do not exceed 200kHz
*/
void I2C::init(uint8_t addr, uint16_t speed) {
  this->addr = addr;
  if (speed != this->speed) {
    this->speed = speed;
    off();
    init();
  }
}

/*
* @brief initialize with default parameters
* @details
* initialize with defaults parameters or previously set
*/
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
  _TWBR = ((F_CPU / (speed * 1000)) - 16) / 2;

  // activate internal pullups for twi.
  PORTC |= (1 << PC4); // pullup for sda
  PORTC |= (1 << PC5); // pullup for scl
}

/*
* @brief return TRUE when no error or FALSE
* @details
* I2C status is in TWSR register (p.200 ATmega328P datasheet)
* before you start refactoring: using direct casting to enum type,
* like return (I2C_status)(TWSR & TWSR_mask) is not safe,
* unless all possible values will be defined in enum variable
*/
uint8_t I2C::TWSR_status() {
  if (test)
    return true;
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

/**
 * @brief Send data to the slave device over I2C
 * @param data 8 bit data to send
 * @param len number of bytes to send
 * @details
 * This function sends data to the slave device over I2C.
 * It sends a start condition, the slave device address (with W bit set),
 * the data, and a stop condition.
 */
void I2C::send_ln(uint8_t *data, uint8_t len)
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
    send_byte(*data++);
    if (err != NO_ERR)
      return;
  };
  stop();
}

/*
 * @brief Send single byte to the slave device over I2C
 * @param data 8 bit data to send
 * @details
 * This function sends single byte data to the slave device over I2C.
 * It sends a start condition, the slave device address (with W bit set),
 * the data, and a stop condition.
 */
void I2C::send(uint8_t data) { send_ln(&data, 1); }

/*
 * @brief turn off TWI and restore interrupts status
*/
void I2C::off() {
  _PRR |= 1 << _PRTWI;  // turn off TWI
  _TWCR |= backup_TWIE; // restore interrupts
}

// private functions
// ****************
void I2C::send_byte(uint8_t data) {
  _TWDR = data;
  _TWCR = (1 << TWINT) | (1 << TWEN);
  do {
  } while (!(_TWCR & (1 << TWINT)));
  if (!TWSR_status()) // data byte transmitted, ACK received?
  {
    stop();
    err = COM_ERR;
  };
}
void I2C::start() {
  // Master Transmitter Mode (atmega328p, p.185)
  // send start condition
  err = NO_ERR;
  _TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
  while (!(_TWCR & (1 << TWINT)))
    ;
  if (!TWSR_status()) // A START condition has been transmitted?
  {
    stop();
    off();
    err = DEV_ERR;
  };
}

void I2C::send_addr(uint8_t addr) {
  // build sla+w, slave device address + w bit (0 write to slave)
  _TWDR = addr << 1;
  // transmit address
  _TWCR = (1 << TWINT) | (1 << TWEN);
  do {
  } while (!(_TWCR & (1 << TWINT)));
  if (!TWSR_status()) // SLA+W transmitted, ACK received?
  {
    stop();
    off();
    err = DEV_ERR;
  };
}
void I2C::stop() {
  _TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
  // wait for STOP flag cleared: STOP transmition completed
  // otherway can be interpreted as REPEATED START
  while (_TWCR & (1 << TWSTO))
    ;
}
