#pragma once

#ifndef BAUD
#define BAUD 9600
#endif

#ifndef F_CPU
#define F_CPU 16000000L
#endif

void initUART(void);
void print_UART(char data);
void println_UART(const char *str);
