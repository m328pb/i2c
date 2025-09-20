/*
 simply routine to check and demonstarte the library
  - plug in network analyzer
  - check recived text with network analyzer
 */
#include "i2c.h"

int main() {
  I2C i2c;
  i2c.test = true;
  const char *txt_def = "test I2C: 100kHz"; // default
  const char *txt_50 = "test I2C: 50kHz";
  const char *txt_200 = "test I2C: 200kHz";
  i2c.init();
  i2c.send_ln((uint8_t *)txt_def, 17);
  i2c.init(0x3c,50);
  i2c.send_ln((uint8_t *)txt_50, 16);
  i2c.init(0x3c,200);
  i2c.send_ln((uint8_t *)txt_200, 17);
  i2c.off();

  uint8_t addr = i2c.scan();
  i2c.init();
  i2c.send(addr);
  i2c.off();
}
