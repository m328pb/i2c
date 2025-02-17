#pragma once
#include <avr/io.h>

// SPI pins
#define SCK PB5
#define MISO PB4
#define MOSI PB3
#define CS PB2

#define CS_HIGH PORTB |= (1 << CS) // disable
#define CS_LOW PORTB &= ~(1 << CS) // enable


#ifdef __cplusplus
extern "C" {
#endif

void SPI_init(void);
uint8_t SPI_transmit(uint8_t cData);

#ifdef __cplusplus
}
#endif  

#ifdef __cplusplus
void SPI_init(uint8_t clock_div);
#endif