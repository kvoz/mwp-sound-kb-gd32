#include "gd32f10x_i2c.h"
#include "i2c.h"

#include <stdint.h>

void i2c_write(uint32_t periph, I2CPacket *packet) {
  /* wait until I2C bus is idle */
  while(i2c_flag_get(periph, I2C_FLAG_I2CBSY));
  /* send a start condition to I2C bus */
  i2c_start_on_bus(periph);
  /* wait until SBSEND bit is set */
  while(!i2c_flag_get(periph, I2C_FLAG_SBSEND));
  /* send slave address to I2C bus */
  i2c_master_addressing(periph, (uint32_t)packet->slave_addr, I2C_TRANSMITTER);
  /* wait until ADDSEND bit is set */
  while(!i2c_flag_get(periph, I2C_FLAG_ADDSEND));
  /* clear ADDSEND bit */
  i2c_flag_clear(periph, I2C_FLAG_ADDSEND);
  /* wait until the transmit data buffer is empty */
  while(!i2c_flag_get(periph, I2C_FLAG_TBE));

  for(uint8_t i = 0; i < packet->buffer_size; i++){
    /* data transmission */
    i2c_data_transmit(periph, packet->buffer[i]);
    /* wait until the TBE bit is set */
    while(!i2c_flag_get(periph, I2C_FLAG_TBE));
  }
  /* send a stop condition to I2C bus */
  i2c_stop_on_bus(periph);
  while(I2C_CTL0(periph)&0x0200);
}

void i2c_read(uint32_t periph, I2CPacket *packet) {
  /* not implemented yet */
}
