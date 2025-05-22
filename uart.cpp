#include "uart.h"
#include <avr/io.h>
#include <stdint.h>
// <setbaud.h> requires BAUD and F_CPU
#include <util/setbaud.h>

void initUART(void) {
  uint8_t sreg = SREG;
  // asynchronous mode
  UBRR0H = UBRRH_VALUE; // defined in setbaud.h based on BAUD & F_CPU
  UBRR0L = UBRRL_VALUE;

#if USE_2X            // double the USART transmission speed?
  UCSR0A |= (1 << 1); // 1 if BAUDRATE is outside tol. Double speed
#else
  UCSR0A &= ~(1 << 1); // 0 if BAUDRATE is within tolerances
#endif
  UCSR0B = (1 << TXEN0) | (1 << RXEN0); // Enable UART transmitter/receiver
  UCSR0B &= ~(1 << UDRIE0);             // disable interrupts
  UCSR0B &= ~(1 << TXCIE0);
  UCSR0B &= ~(1 << RXCIE0);
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8 data bits, 1 stop bit
  SREG = sreg;
}

void print_UART(char data) {
  // Wait for empty transmit buffer
  loop_until_bit_is_set(UCSR0A, UDRE0);
  UDR0 = data; // send data
}

void println_UART(const char *str) {
  while (*str) {
    print_UART(*str++);
  }
  print_UART('\n');
}
