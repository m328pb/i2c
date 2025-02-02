#include <avr/io.h>
#include <avr/interrupt.h>
#include "i2c.h"

void mw_init()
{
    PRR &= ~(1 << PRTWI); // turn on TWI

    /* twi bit rate formula from atmega328p manual pg 180
    SCL Frequency = CPU Clock Frequency / (16 + (2 * TWBR))
    SCL frequency up to 400kHz on atmega328p
    note: TWBR should be 10 or higher for master mode*/
    TWSR &= ~(1 << TWPS0);                // initialize twi prescaler and bit rate
    TWSR &= ~(1 << TWPS1);                // set prescaler to 1
    TWBR = ((F_CPU / I2C_FREQ) - 16) / 2; // 100kHz

    // activate internal pullups for twi.
    PORTC |= (1 << PC4); // pullup for sda
    PORTC |= (1 << PC5); // pullup for scl
};

/**
 * @brief Send data to the slave device over I2C
 * @param data 8 bit data to send
 * @param len number of bytes to send
 * @return 1 on success, 0 on error
 * @details
 * This function sends data to the slave device over I2C.
 * It sends a start condition, the slave device address (with W bit set),
 * the data, and a stop condition. It returns 1 on success, and 0 on error.
 */
bool mw_write(uint8_t *data, uint8_t len)
// must be 8 bit data
{
    // Master Transmitter Mode (atmega328p, pg 185)
    // send start condition
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)))
        ;
    if ((TWSR & 0xf8) != 0x08) // A START condition has been transmitted
        return false;

    // build sla+w, slave device address + w bit (0 write to slave)
    TWDR = SSD1306_I2C_ADDRESS << 1;
    // transmit address
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)))
        ;
    if ((TWSR & 0xf8) != 0x18) // SLA+W transmitted, ACK received
        return false;

    // send data
    while (len--)
    {
        TWDR = *data++;
        TWCR = (1 << TWINT) | (1 << TWEN);
        while (!(TWCR & (1 << TWINT)))
            ;
        if ((TWSR & 0xf8) != 0x28) // data byte transmitted, ACK received
            return false;
    }

    // send stop condition
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
    return true;
}
