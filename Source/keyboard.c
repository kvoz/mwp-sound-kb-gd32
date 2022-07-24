#include "keyboard.h"

/* Include software timers and queues resources */
#include "timers.h"
#include "queue.h"

#include <stddef.h>
#include <stdint.h>

/* Keyboard setup */
#define KEYBOARD_NUM_ROWS    (4)
#define KEYBOARD_NUM_COLS    (5)

#define KEYBOARD_COL4 {'F', 'A', '6', '1'}
#define KEYBOARD_COL3 {'G', 'B', '7', '2'}
#define KEYBOARD_COL2 {'X', 'C', '8', '3'}
#define KEYBOARD_COL1 {'Y', 'D', '9', '4'}
#define KEYBOARD_COL0 {'Z', 'E', '0', '5'}

const char symbol[KEYBOARD_NUM_COLS][KEYBOARD_NUM_ROWS] = {KEYBOARD_COL0, KEYBOARD_COL1, KEYBOARD_COL2, KEYBOARD_COL3, KEYBOARD_COL4};

/* Timeout for keyboard debounce in ms. */
#define KEYBOARD_DEBOUNCE        (20)

/* Defines of keyboard FSM states. */
typedef enum {
    KB_FSM_STATE_CHK_KEY_PRESS,
    KB_FSM_STATE_PRESS_DEBOUNCE,
    KB_FSM_STATE_CHK_KEY_RELEASE,
    KB_FSM_STATE_RELEASE_DEBOUNCE,

    KB_FSM_STATE_NUM
} KeyboardFsmStates;

/* Defines of keyboard FSM internal events. */
typedef enum {
    KB_FSM_EVENT_NONE,
    KB_FSM_EVENT_KEY_PRESSED,
    KB_FSM_EVENT_DEBOUNCE_TIMEOUT,

    KB_FSM_INT_EVENT_NUM
} KeyboardFsmEvents;

/* Structure for keyboard system instance. */
typedef struct KeyboardInstance_t {
  char key_pressed;

  // Callback functions.
  struct {
    void (*col_select)(uint8_t col);
    void (*col_unselect)(uint8_t col);
    uint8_t (*row_read)(uint8_t row);
  } callback;

  // FSM behavior struct.
  struct {
    void (*transitions[KB_FSM_STATE_NUM][KB_FSM_INT_EVENT_NUM])(void);
    KeyboardFsmStates state;
    KeyboardFsmEvents event;
  } fsm;

} KeyboardInstance;

/* Object-handler of keyboard system behavior. */
KeyboardInstance kb;

static void key_pressed_callback(void) {
  /* Sync with outer FSM sound for playing melody. */
  queueRecSound record;
  record.msg = SOUND_MSG_PLAY;
  record.val = kb.key_pressed - '0';
  if (record.val > 9) record.val = 9;
  queue_put(QUEUE_SOUND, &record);
}

void check_key_press(void) {
  kb.fsm.state = KB_FSM_STATE_CHK_KEY_PRESS;

  for (uint8_t col = 0; col < KEYBOARD_NUM_COLS; col++) {

    kb.callback.col_select(col);

    for (uint8_t row = 0; row < KEYBOARD_NUM_ROWS; row++) {
      uint8_t status = kb.callback.row_read(row);

      if (status == 0) {
        kb.callback.col_unselect(col);
        kb.key_pressed = symbol[KEYBOARD_NUM_COLS - col - 1][KEYBOARD_NUM_ROWS - row - 1];
        // Reset software timer for debounce.
        swtimer_reset(TIM_KEYBOARD);
        kb.fsm.event = KB_FSM_EVENT_KEY_PRESSED;

        /* Call function to manage behavior of pressed key. */
        key_pressed_callback();
        return;
      }
    }

    kb.callback.col_unselect(col);
  }

  kb.fsm.event = KB_FSM_EVENT_NONE;
}

void press_debounce(void) {
  kb.fsm.state = KB_FSM_STATE_PRESS_DEBOUNCE;

  if (swtimer_get(TIM_KEYBOARD) >= KEYBOARD_DEBOUNCE) {
    kb.fsm.event = KB_FSM_EVENT_DEBOUNCE_TIMEOUT;
    swtimer_stop(TIM_KEYBOARD);
  } else {
    kb.fsm.event = KB_FSM_EVENT_NONE;
  }
}

void check_key_release(void) {
  kb.fsm.state = KB_FSM_STATE_CHK_KEY_RELEASE;

  for (uint8_t col = 0; col < KEYBOARD_NUM_COLS; col++) {
    kb.callback.col_select(col);

    for (uint8_t row = 0; row < KEYBOARD_NUM_ROWS; row++) {
      uint8_t status = kb.callback.row_read(row);

      if (status == 0) {
        kb.callback.col_unselect(col);
        kb.fsm.event = KB_FSM_EVENT_KEY_PRESSED;
        return;
      }
    }

    kb.callback.col_unselect(col);
  }

  /* If all keys was released. */
  kb.fsm.event = KB_FSM_EVENT_NONE;
  // Reset software timer for debounce.
  swtimer_reset(TIM_KEYBOARD);
}


void release_debounce(void) {
  kb.fsm.state = KB_FSM_STATE_RELEASE_DEBOUNCE;

  if (swtimer_get(TIM_KEYBOARD) >= KEYBOARD_DEBOUNCE) {
    kb.fsm.event = KB_FSM_EVENT_DEBOUNCE_TIMEOUT;
    swtimer_stop(TIM_KEYBOARD);
  } else {
    kb.fsm.event = KB_FSM_EVENT_NONE;
  }
}

void _err(void) {
  // Handle error.
  while(1);
}

void keyboard_init(void) {
  // Apply initial configuration
  kb.key_pressed = '\0';

  // Start position for FSM (state/event pair).
  kb.fsm.state = KB_FSM_STATE_CHK_KEY_PRESS;
  kb.fsm.event = KB_FSM_EVENT_NONE;

  // Register full FSM transition table.
  kb.fsm.transitions[KB_FSM_STATE_CHK_KEY_PRESS][KB_FSM_EVENT_NONE] = check_key_press;
  kb.fsm.transitions[KB_FSM_STATE_CHK_KEY_PRESS][KB_FSM_EVENT_KEY_PRESSED] = press_debounce;
  kb.fsm.transitions[KB_FSM_STATE_CHK_KEY_PRESS][KB_FSM_EVENT_DEBOUNCE_TIMEOUT] = _err;

  kb.fsm.transitions[KB_FSM_STATE_PRESS_DEBOUNCE][KB_FSM_EVENT_NONE] = press_debounce;
  kb.fsm.transitions[KB_FSM_STATE_PRESS_DEBOUNCE][KB_FSM_EVENT_KEY_PRESSED] = _err;
  kb.fsm.transitions[KB_FSM_STATE_PRESS_DEBOUNCE][KB_FSM_EVENT_DEBOUNCE_TIMEOUT] = check_key_release;

  kb.fsm.transitions[KB_FSM_STATE_CHK_KEY_RELEASE][KB_FSM_EVENT_NONE] = release_debounce;
  kb.fsm.transitions[KB_FSM_STATE_CHK_KEY_RELEASE][KB_FSM_EVENT_KEY_PRESSED] = check_key_release;
  kb.fsm.transitions[KB_FSM_STATE_CHK_KEY_RELEASE][KB_FSM_EVENT_DEBOUNCE_TIMEOUT] = _err;

  kb.fsm.transitions[KB_FSM_STATE_RELEASE_DEBOUNCE][KB_FSM_EVENT_NONE] = release_debounce;
  kb.fsm.transitions[KB_FSM_STATE_RELEASE_DEBOUNCE][KB_FSM_EVENT_KEY_PRESSED] = _err;
  kb.fsm.transitions[KB_FSM_STATE_RELEASE_DEBOUNCE][KB_FSM_EVENT_DEBOUNCE_TIMEOUT] = check_key_press;
}

void keyboard_fsm_call(void) {
  kb.fsm.transitions[kb.fsm.state][kb.fsm.event]();
}

static void _kb_col_sel(uint8_t col) { (void)(col); };
static void _kb_col_unsel(uint8_t col) { (void)(col); };
static uint8_t _kb_row_read(uint8_t row) { (void)(row); return 0; };

void keyboard_reg_func(void (*col_select)(uint8_t col),
                            void (*col_unselect)(uint8_t col),
                            uint8_t (*row_read)(uint8_t row)) {

  if (col_select == NULL || col_unselect == NULL || row_read == NULL) {
    // Dummy pointers to safe functions if there is error in callback registration.
    kb.callback.col_select = _kb_col_sel;
    kb.callback.col_unselect = _kb_col_unsel;
    kb.callback.row_read = _kb_row_read;
  } else {
    kb.callback.col_select = col_select;
    kb.callback.col_unselect = col_unselect;
    kb.callback.row_read = row_read;
  }
}
