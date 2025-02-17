#pragma once

#define BAUD 9600

void initUART(void);
void print_UART(char data);
void println_UART(const char *str);
