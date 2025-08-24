# I2C protocol for ATmega328pb/p

Created primarly for OLED/LCD display and to make it maximum lightweight.

## main features

- minimum implementation to make it lightweight

```bash
AVR Memory Usage
----------------
Device: Unknown

Program:     392 bytes
(.text + .data + .bootloader)

Data:          0 bytes
(.data + .bss + .noinit)
```

- no Arduino libs dependency
- only Master Transmition
- on ATmega328pb uses only TWI0
- do not use interrupts just loop until response available

## usage

Library implemented as class I2C::I2C(uint8_t addr) and provides following methods:

- I2C::init() - initialize chip registers for TWI communication
- I2C::write(uint8_t *data, uint8_t len) - sends data in 8bit chunks `len` times.
Will execute stop() after finished and on any error unless force=true (see below).
- I2C::stop() - sends stop signal
- I2C::off() - turn off TWI and restore back TWI interrupts setting
- I2C::force - when set `true` will not wait for ACK from slave. By default `false`

Class also exposes I2C.err variable of type error:

```cpp
  typedef enum : uint8_t {
    NO_ERR = 0,
    DEV_ERR = 1,
    COM_ERR = 2,
  } error;
```

When library compiled with ENABLE_I2C_SCAN directive, aditional empty
constructor I2C::I2C will be available. In this situation the constructor will scan
through all addresses and set one when recive ACK. This also set I2C::force to `false`.
In most cases not needed and waste memory (aditional 176 bytes!), but can be
usefull in rare cases when device address is not known.

## example

Script sends sample text with `force=true`, so no need for I2C device, just network analyzer.
Compile examples/main.cpp ([env:demo] in platformio.ini).
