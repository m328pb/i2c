#include <avr/io.h>
#include "uart.h"
#include <util/setbaud.h>


void initUSART(void)
{                       /* requires BAUD */
  UBRR0H = UBRRH_VALUE; /* defined in setbaud.h */
  UBRR0L = UBRRL_VALUE;
  UCSR0A &= ~(1 << U2X0);
  /* Enable USART transmitter/receiver */
  UCSR0B = (1 << TXEN0) | (1 << RXEN0);
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); /* 8 data bits, 1 stop bit */
}

void printByte(char data)
{
  /* Wait for empty transmit buffer */
  loop_until_bit_is_set(UCSR0A, UDRE0);
  UDR0 = data; /* send data */
}
