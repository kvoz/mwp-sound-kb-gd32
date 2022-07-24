#ifndef SOUND_H_
#define SOUND_H_

#include <stdint.h>

/* Function to initialize FSM object. */
void sound_init(void);

/* Functions to register hardware dependency callbacks. */
void sound_reg_func(void (*set_vol) (uint8_t slave_addr, uint8_t data),
                    void (*turn_on)(void),
                    void (*turn_off)(void),
                    void (*set_tone) (uint16_t tone));

/* Function to call FSM transitions. */
void sound_fsm_call(void);

#endif /* SOUND_H_ */
