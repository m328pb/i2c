#include <avr/io.h>
#include "spi.h"

void SPI_init(void)
{
    // The PRSPI bit must be written to zero to enable SPI module
    PRR &= ~(1 << PRSPI);

    // Set SCK, MOSI and CS as output
    DDRB = (1 << SCK) | (1 << MOSI) | (1 << CS); // SCK, MOSI, CS output
    CS_HIGH;                                     // SS pullup
    PORTB |= (1 << MISO);                        // MISO pullup

    SPCR = ((0 & ~(1 << SPIE)) |            // disable interrupts SPIE=0
           (1 << SPE) |                     // SPI Enable =1,
           (1 << MSTR) |                    // set Master mode MSTR=1,
           (1 << SPR0) | (1 << SPR1)) &     // set clock rate fck/16 SPR0=1, SPR1=0
          ~(1 << DORD) &                    // Data order:  MSB first when DORD=0
          ~(1 << CPOL) &                    // SPI mode: CPOL=0: lead edge - Rising; trailing edge: Falling
          ~(1 << CPHA);                     // SPI mode: CPHA=0: lead edge - Sample; trailing edge: Setup
}
uint8_t SPI_transmit(uint8_t cData)
{
    /* Start transmission */
    SPDR = cData;
    /* Credits to Arduino SPI lib:
     * The following NOP introduces a small delay that can prevent the wait
     * loop form iterating when running at the maximum speed. This gives
     * about 10% more speed, even if it seems counter-intuitive. At lower
     * speeds it is unnoticed.
     */
    // asm volatile("nop");
    /* Wait for transmission complete */
    while (!(SPSR & (1 << SPIF)))
        ;
    return SPDR;
}
