#ifndef SYSTICK_H
#define SYSTICK_H

#include <stdint.h>

void systick_config(void);
void systick_delay_1us(uint32_t delay);
void systick_delay_1ms(uint32_t delay);

#endif /* SYSTICK_H */
