#include "melodies.h"

#include <stdint.h>

/* Melody scores. */
const MelodyNote melody_start[] = {
    { .note = NOTE_D, .duration = 450 },
    { .note = NOTE_REST, .duration = 100 },
    { .note = NOTE_B, .duration = 300 },
    { .note = NOTE_REST, .duration = 50 },
    { .note = NOTE_G, .duration = 150 },
    { .note = NOTE_REST, .duration = 50 },
    { .note = NOTE_A, .duration = 150 },
    { .note = NOTE_REST, .duration = 50 },
    { .note = NOTE_C, .duration = 450 }
};

const MelodyNote melody_test[] = {
    { .note = NOTE_C, .duration = 499 },
    { .note = NOTE_E, .duration = 333 },
    { .note = NOTE_Fd_Gb, .duration = 333 },
    { .note = NOTE_A, .duration = 166 },

    { .note = NOTE_G, .duration = 499 },
    { .note = NOTE_E, .duration = 166 },
    { .note = NOTE_REST, .duration = 166 },
    { .note = NOTE_C, .duration = 166 },
    { .note = NOTE_REST, .duration = 166 },
    { .note = NOTE_A, .duration = 166 },

    { .note = NOTE_Fd_Gb, .duration = 166 },
    { .note = NOTE_F, .duration = 166 },
    { .note = NOTE_F, .duration = 166 },
    { .note = NOTE_G, .duration = 666 },
    { .note = NOTE_REST, .duration = 166 }
};

/* Array of pointers to melodies. */
const Score melodies[MELODY_NUM] = {
    [MELODY_STARTUP] = { .id = MELODY_STARTUP, .len = 9, .notes = melody_start},
    [MELODY_TEST] = { .id = MELODY_TEST, .len = 15, .notes = melody_test},
};

const Score * melody_get(melodyId id) {
  if (id < MELODY_NUM) {
    return &melodies[id];
  } else {
    // If there is overflow of selected ID, then return last melody in array.
    return &melodies[MELODY_NUM - 1];
  }
}
