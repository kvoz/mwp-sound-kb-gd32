#include "sound.h"
#include "melodies.h"
#include "max9768.h"

/* Include software timers and queues resources */
#include "timers.h"
#include "queue.h"

#include <stddef.h>
#include <stdint.h>

/* Define of sound FSM states. */
typedef enum {
    SOUND_FSM_STATE_IDLE,
    SOUND_FSM_STATE_PLAY_NOTE,
    SOUND_FSM_STATE_WAIT,

    SOUND_FSM_STATE_NUM
} SoundFsmStates;

/* Define of sound FSM events. */
typedef enum {
    SOUND_FSM_EVENT_NONE,
    SOUND_FSM_EVENT_MELODY_START,
    SOUND_FSM_EVENT_NOTE_NEXT,
    SOUND_FSM_EVENT_MELODY_STOP,

    SOUND_FSM_EVENT_NUM
} SoundFsmEvents;

/* Structure of sound system instance.
 * Holds information about playing melody, system volume and hardware callbacks.
 * */
typedef struct SoundInstance_t {
  uint8_t volume;

  // Melody player handler.
  struct {
    const Score *melody;
    uint8_t currentNote;
  } player;

  // Callback functions.
  struct {
    void (*turn_on)(void);
    void (*turn_off)(void);
    void (*set_tone) (uint16_t tone);
  } callback;

  // FSM behavior struct.
  struct {
    void (*transitions[SOUND_FSM_STATE_NUM][SOUND_FSM_EVENT_NUM])();
    SoundFsmStates state;
    SoundFsmEvents event;
  } fsm;

} SoundInstance;

/* Dummy callback functions for safe usage. */
static void _turn_on(void) {};
static void _turn_off(void) {};
static void _set_tone(uint16_t tone) { (void)(tone); };

/* Object-handler of sound system behavior. */
SoundInstance sound;

static void idle(void) {
  sound.fsm.state = SOUND_FSM_STATE_IDLE;

  // Check if there is external events in event queue.
  queueRecSound record;
  if (queue_get(QUEUE_SOUND, &record) == 1) {

    switch (record.msg) {
      case SOUND_MSG_SET_VOL:
        // Set new volume.
        max9768_write(record.val);
        // Update FSM state/event.
        sound.fsm.event = SOUND_FSM_EVENT_NONE;
        break;

      case SOUND_MSG_PLAY:
        // Get melody pointer.
        sound.player.melody = melody_get(record.val);
        sound.player.currentNote = 0;
        sound.callback.turn_on();
        // Update FSM state/event.
        sound.fsm.event = SOUND_FSM_EVENT_MELODY_START;
        break;

      default:
        break;
    }
  }

}

static void note_play(void) {
  sound.fsm.state = SOUND_FSM_STATE_PLAY_NOTE;

  sound.callback.set_tone(sound.player.melody->notes[sound.player.currentNote].note);
  /* Restart software timer for counting delay of selected note */
  swtimer_reset(TIM_SOUND);

  sound.fsm.event = SOUND_FSM_EVENT_NONE;
}

static void note_wait(void) {
  sound.fsm.state = SOUND_FSM_STATE_WAIT;

  if (swtimer_get(TIM_SOUND) >= sound.player.melody->notes[sound.player.currentNote].duration) {
    /* Stop software timer for counting delays of notes. */
    swtimer_stop(TIM_SOUND);

    // Checkout next note.
    sound.player.currentNote++;
    if (sound.player.currentNote < sound.player.melody->len) {
      // If next note exist: update it.
      sound.fsm.event = SOUND_FSM_EVENT_NOTE_NEXT;
    } else {
      // Else stop output, and return to idle.
      sound.callback.turn_off();
      sound.fsm.event = SOUND_FSM_EVENT_MELODY_STOP;
    }

  } else {
    // Wait if note not played fully.
    sound.fsm.event = SOUND_FSM_EVENT_NONE;
  }
}

static void _err(void) {
  // Handle error.
  while(1);
}


/* Initialization function of sound instance. */
void sound_init(void) {
  // Set initial values.
  sound.volume = 0;

  // Flush melody pointer.
  sound.player.melody = (void*)0;
  sound.player.currentNote = 0;

  // Start position for FSM (state/event pair).
  sound.fsm.state = SOUND_FSM_STATE_IDLE;
  sound.fsm.event = SOUND_FSM_EVENT_NONE;

  // Register full FSM transition table.
  sound.fsm.transitions[SOUND_FSM_STATE_IDLE][SOUND_FSM_EVENT_NONE] = idle;
  sound.fsm.transitions[SOUND_FSM_STATE_IDLE][SOUND_FSM_EVENT_MELODY_START] = note_play;
  sound.fsm.transitions[SOUND_FSM_STATE_IDLE][SOUND_FSM_EVENT_NOTE_NEXT] = _err;
  sound.fsm.transitions[SOUND_FSM_STATE_IDLE][SOUND_FSM_EVENT_MELODY_STOP] = _err;

  sound.fsm.transitions[SOUND_FSM_STATE_PLAY_NOTE][SOUND_FSM_EVENT_NONE] = note_wait;
  sound.fsm.transitions[SOUND_FSM_STATE_PLAY_NOTE][SOUND_FSM_EVENT_MELODY_START] = _err;
  sound.fsm.transitions[SOUND_FSM_STATE_PLAY_NOTE][SOUND_FSM_EVENT_NOTE_NEXT] = _err;
  sound.fsm.transitions[SOUND_FSM_STATE_PLAY_NOTE][SOUND_FSM_EVENT_MELODY_STOP] = _err;

  sound.fsm.transitions[SOUND_FSM_STATE_WAIT][SOUND_FSM_EVENT_NONE] = note_wait;
  sound.fsm.transitions[SOUND_FSM_STATE_WAIT][SOUND_FSM_EVENT_MELODY_START] = _err;
  sound.fsm.transitions[SOUND_FSM_STATE_WAIT][SOUND_FSM_EVENT_NOTE_NEXT] = note_play;
  sound.fsm.transitions[SOUND_FSM_STATE_WAIT][SOUND_FSM_EVENT_MELODY_STOP] = idle;
}

void sound_reg_func(void (*set_vol) (uint8_t slave_addr, uint8_t data),
                    void (*turn_on)(void),
                    void (*turn_off)(void),
                    void (*set_tone) (uint16_t tone)) {
  // Pass through HW callback to volume driver.
  max9768_reg_i2c_func(set_vol);

  if (turn_on == NULL || turn_off == NULL || set_tone == NULL) {
    // Register dummy functions for safe.
    sound.callback.set_tone = _set_tone;
    sound.callback.turn_on = _turn_on;
    sound.callback.turn_off = _turn_off;
  } else {
    // Register callback from used HW for output.
    sound.callback.set_tone = set_tone;
    sound.callback.turn_on = turn_on;
    sound.callback.turn_off = turn_off;
  }
}

void sound_fsm_call(void) {
  sound.fsm.transitions[sound.fsm.state][sound.fsm.event]();
}
