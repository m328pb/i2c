#include <avr/io.h>
#include "spi.h"

void SPI_init()
{
    SPI_init(16);
}
void SPI_init(uint8_t clock_div)
{

    // The PRSPI bit must be written to zero to enable SPI module
    PRR &= ~(1 << PRSPI);

    // Set SCK, MOSI and CS as output
    DDRB = (1 << SCK) | (1 << MOSI) | (1 << CS); // SCK, MOSI, CS output
    CS_HIGH;                                     // SS pullup
    PORTB |= (1 << MISO);                        // MISO pullup

    SPCR = ((0 & ~(1 << SPIE)) | // disable interrupts SPIE=0
            (1 << SPE) |         // SPI Enable =1,
            (1 << MSTR)) &       // set Master mode MSTR=1,
           ~(1 << DORD) &        // Data order:  MSB first when DORD=0
           ~(1 << CPOL) &        // SPI mode: CPOL=0: lead edge - Rising; trailing edge: Falling
           ~(1 << CPHA);         // SPI mode: CPHA=0: lead edge - Sample; trailing edge: Setup

    // Set clock rate
    uint8_t div;
    switch (clock_div) //       SPI2X    SPR1    SPR0
    {
    case 4:
        div = 0; // 0        0       0
        break;
    case 16:
        div = 1; // 0        0       1
        break;
    case 64:
        div = 2; // 0        1       0
        break;
    case 128:
        div = 3; // 0        1       1
        break;
    case 2:
        div = 4; // 1        0       0
        break;
    case 8:
        div = 5; // 1        0       1
        break;
    case 32:
        div = 6; // 1        1       0
        break;
    default: //64
        div = 2;
    }
    SPSR = (SPSR & ~(1 << SPI2X)) | (div & 0b100);
    SPCR = (SPCR & ~(1 << SPR1)) | (div & 0b010);
    SPCR = (SPCR & ~(1 << SPR0)) | (div & 0b001);
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
