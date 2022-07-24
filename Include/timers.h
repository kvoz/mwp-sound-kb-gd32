#ifndef TIMERS_H_
#define TIMERS_H_

#include <stdint.h>

typedef enum {
	TIM_SOUND,				// Used for control sound signals duration
	TIM_KEYBOARD,

	NUM_OF_TIMERS
} swTimerId;

void swtimer_reset(uint8_t tim);
void swtimer_pause(uint8_t tim);
void swtimer_stop(uint8_t tim);
void swtimer_release(uint8_t tim);
void swtimer_process(void);
uint16_t swtimer_get(uint8_t tim);

#endif
