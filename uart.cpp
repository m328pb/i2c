#include <avr/io.h>
#include "uart.h"
#include <util/setbaud.h>

void initUART(void)
{                       /* requires BAUD */
  UBRR0H = UBRRH_VALUE; /* defined in setbaud.h */
  UBRR0L = UBRRL_VALUE;
  UCSR0A &= ~(1 << U2X0);
  /* Enable UART transmitter/receiver */
  UCSR0B = (1 << TXEN0) | (1 << RXEN0);
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); /* 8 data bits, 1 stop bit */
}

void print_UART(char data)
{
  /* Wait for empty transmit buffer */
  loop_until_bit_is_set(UCSR0A, UDRE0);
  UDR0 = data; /* send data */
}

void println_UART(const char *str)
{
  while (*str)
  {
    print_UART(*str++);
  }
  print_UART('\n');
}

#ifdef PF_DEFINED
void println_UART(FRESULT result)
{
  switch (result)
  {
  case FR_OK:
    println_UART("OK\n");
    break;
  case FR_DISK_ERR:
    println_UART("Disk error\n");
    break;
  case FR_NOT_READY:
    println_UART("Not ready\n");
    break;
  case FR_NO_FILE:
    println_UART("No file\n");
    break;
  case FR_NOT_OPENED:
    println_UART("Not opened\n");
    break;
  case FR_NOT_ENABLED:
    println_UART("Not enabled\n");
    break;
  case FR_NO_FILESYSTEM:
    println_UART("No filesystem\n");
    break;
  }
}

void println_UART(R1_RESP result)
{
  switch (result)
  {
  case INITIALIZED:
    println_UART("INITIALIZED/n");
    break;
  case IDLE_STATE:
    println_UART("IDLE_STATE/n");
    break;
  case DATA_START_TOKEN:
    println_UART("DATA_START_TOKEN/n");
    break;
  case NO_RESPONSE:
    println_UART("NO_RESPONSE/n");
    break;
  case ERROR:
    println_UART("Error/n");
    break;
  }
}

void println_UART(uint16_t bits)
{
  if (bits & CARD_IS_LOCKED)
    println_UART("CARD_IS_LOCKED/n");
  if (bits & WP_ERASE_SKIP)
    println_UART("WP_ERASE_SKIP/n");
  if (bits & GENERAL_ERROR)
    println_UART("GENERAL_ERROR/n");
  if (bits & CC_ERRROR)
    println_UART("CC_ERRROR/n");
  if (bits & CARD_ECC_FAILED)
    println_UART("CARD_ECC_FAILED/n");
  if (bits & WP_VIOLATION)
    println_UART("WP_VIOLATION/n");
  if (bits & ERASE_PARAM)
    println_UART("ERASE_PARAM/n");
  if (bits & OUT_OF_RANGE)
    println_UART("OUT_OF_RANGE/n");
  if (bits & IN_IDLE_STATE)
    println_UART("IN_IDLE_STATE/n");
  if (bits & ERASE_RESET)
    println_UART("ERASE_RESET/n");
  if (bits & ILLEGAL_CMD)
    println_UART("ILLEGAL_CMD/n");
  if (bits & COM_CRC_ERROR)
    println_UART("COM_CRC_ERROR/n");
  if (bits & ERASE_SEQ_ERR)
    println_UART("ERASE_SEQUENCE_ERROR/n");
  if (bits & ADDR_ERROR)
    println_UART("ADDRESS_ERROR/n");
  if (bits & PARAMETER_ERROR)
    println_UART("PARAM_ERROR/n");
}
#endif