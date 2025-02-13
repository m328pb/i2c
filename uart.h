#pragma once
#include "diskio.h"
#include "pff.h" // include if using FAT for printsUART(FRESULT result)

#define BAUD 9600



void initUART(void);
void print_UART(char data);
void println_UART(const char *str);
#ifdef PF_DEFINED
void println_UART(FRESULT result);
void println_UART(R1_RESP result);
void println_UART(uint16_t bits);
#endif