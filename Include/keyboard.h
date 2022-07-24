#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include <stdint.h>

/* Function to initialize FSM object. */
void keyboard_init(void);
/* Function to call FSM transitions. */
void keyboard_fsm_call(void);

/* Functions to register hardware dependency callbacks. */
void keyboard_reg_func(void (*col_select)(uint8_t col),
                            void (*col_unselect)(uint8_t col),
                            uint8_t (*row_read)(uint8_t row));

#endif /* KEYBOARD_H_ */
