#include "timers.h"

#include <stdint.h>

typedef enum {
  STOP = 0,
  RUN,
  PAUSE
} timerState;

typedef struct {
  timerState state;
  uint16_t counter;
} SwTimer;

// Array of software timers.
SwTimer timers[NUM_OF_TIMERS];

void swtimer_reset(uint8_t tim) {
  timers[tim].state = RUN;
  timers[tim].counter = 0;
}

void swtimer_pause(uint8_t tim) {
  timers[tim].state = PAUSE;
}

void swtimer_stop(uint8_t tim) {
  timers[tim].state = STOP;
  timers[tim].counter = 0;
}

void swtimer_release(uint8_t tim) {
  timers[tim].state = RUN;
}

void swtimer_process(void) {
	for(uint8_t i = 0; i < NUM_OF_TIMERS; i++) {
		if(timers[i].state == RUN) {
			timers[i].counter++;
		}
	}
}

uint16_t swtimer_get(uint8_t tim) {
	return timers[tim].counter;
}

