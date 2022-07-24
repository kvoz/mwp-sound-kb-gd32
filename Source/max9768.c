#include "max9768.h"

#include <stdint.h>
#include <stddef.h>

// TODO: now supports only single chip on I2C bus, improve this.

/* Predefined ADDRx bits for single chip on I2C bus. */
#define MAX9768_ADDR2       (0)
#define MAX9768_ADDR1       (1)
#define MAX9768_I2C_ADDR    (0b10010000 | (MAX9768_ADDR2 << 2) | (MAX9768_ADDR1 << 1))

/* Dummy function for safe use, registered as default callbacks. */
static void _write(uint8_t slave_addr, uint8_t data) { (void)(slave_addr); (void)(data);}

/* MAX9768 chip handler struct. */
typedef struct {
  uint8_t slaveAddr;
  void (*_write_byte) (uint8_t addr, uint8_t data);
} Max9768Chip;

/* Chip instance with initialization. */
Max9768Chip max9768 = {
    .slaveAddr = MAX9768_I2C_ADDR,
};

/* Public function for register interface (i2c) callback. */
void max9768_reg_i2c_func(void (*i2c_w) (uint8_t slave_addr, uint8_t data)) {
  if(i2c_w == NULL) {
    max9768._write_byte = _write;
  } else {
    max9768._write_byte = i2c_w;
  }
}

/* Public function for chip write. */
void max9768_write(uint8_t data) {
  /* In slave address bit R/W is set to 0 for write operation. */
  max9768._write_byte(max9768.slaveAddr | (0 << 0), data);
}


