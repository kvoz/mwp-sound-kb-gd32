#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>

typedef struct {
  uint8_t * buffer;
  uint8_t buffer_size;
  uint8_t slave_addr;
} I2CPacket;

void i2c_write(uint32_t periph, I2CPacket *packet);
void i2c_read(uint32_t periph, I2CPacket *packet);

#endif /* I2C_H_ */
