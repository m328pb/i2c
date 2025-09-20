/*
 Used to measure memory usage: env:mem_usage in platformio.ini
 then do avr-size on firmware.elf
 */
#include "i2c.h"

int main() {
  uint8_t data[] = {0};
  I2C i2c;
#if defined(ENABLE_I2C_SCAN)
  i2c.scan();
#endif
  i2c.init();
  i2c.send(0);
  i2c.send_ln(data, 1);
  i2c.off();
  return 0;
}
