#pragma once
#include "i2c.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>

I2C::I2C(uint8_t addr) { this->addr = addr; }

void I2C::init() {
  PRR0 &= ~(1 << PRTWI0); // turn on TWI
  TWCR0 &= ~(1 << TWIE);  // turn off interrupts

  /* twi bit rate formula from atmega328p manual pg 180
  SCL Frequency = CPU Clock Frequency / (16 + (2 * TWBR))
  SCL frequency up to 400kHz on atmega328p
  note: TWBR should be 10 or higher for master mode*/
  TWSR0 &= ~(1 << TWPS0); // initialize twi prescaler and bit rate
  TWSR0 &= ~(1 << TWPS1); // set prescaler to 1
  TWBR0 = ((F_CPU / I2C_FREQ) - 16) / 2; // 100kHz

  // activate internal pullups for twi.
  PORTC |= (1 << PC4); // pullup for sda
  PORTC |= (1 << PC5); // pullup for scl
}

/**
 * @brief Send data to the slave device over I2C
 * @param data 8 bit data to send
 * @param len number of bytes to send
 * @return 1 on success, 0 on error
 * @details
 * This function sends data to the slave device over I2C.
 * It sends a start condition, the slave device address (with W bit set),
 * the data, and a stop condition. It returns error struct.
 */

I2C::error I2C::write(uint8_t *data, uint8_t len)
// must be 8 bit data
{
  status = TW_NO_INFO;
  // Master Transmitter Mode (atmega328p, pg 185)
  // send start condition
  TWCR0 = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
  while (!(TWCR0 & (1 << TWINT)))
    ;
  status = TWSR_code;
  if (status != TW_START) // A START condition has been transmitted
  {
    stop();
    return  DEV_ERR;
  };

  // build sla+w, slave device address + w bit (0 write to slave)
  TWDR0 = addr << 1;
  // transmit address
  TWCR0 = (1 << TWINT) | (1 << TWEN);
  while (!(TWCR0 & (1 << TWINT)))
    ;
  status = TWSR_code;
  if (status != TW_ACK) // SLA+W transmitted, ACK received
  {
    stop();
    return DEV_ERR;
  };

  // send data
  while (len--) {
    TWDR0 = *data++;
    TWCR0 = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR0 & (1 << TWINT)))
      ;
    status = TWSR_code;
    if (status != TW_DATA_ACK) // data byte transmitted, ACK received
    {
      stop();
      return COM_ERR;
    };
  };
  // send stop condition
	stop();
  return NO_ERR;
}

void I2C::stop() { TWCR0 = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN); }
