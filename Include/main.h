#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>

void max9768_write_callback(uint8_t slave_addr, uint8_t data);

void sound_set_tone_callback(uint16_t tone);
void sound_turn_on_callback(void);
void sound_turn_off_callback(void);

void keyboard_col_sel_callback(uint8_t col);
void keyboard_col_unsel_callback(uint8_t col);
uint8_t keyboard_row_read_callback(uint8_t row);

#endif /* MAIN_H */
