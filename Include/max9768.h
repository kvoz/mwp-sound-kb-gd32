#ifndef MAX9768_H_
#define MAX9768_H_

#include <stdint.h>

void max9768_reg_i2c_func(void (*i2c_w) (uint8_t slave_addr, uint8_t data));
void max9768_write(uint8_t data);

#endif /* MAX9768_H_ */
