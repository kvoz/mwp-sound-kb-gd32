#ifndef MELODIES_H_
#define MELODIES_H_

#include <stdint.h>

// TODO: there is limitation on melody length (256 notes).

typedef enum {
  MELODY_STARTUP,
  MELODY_TEST,

  MELODY_NUM
} melodyId;

/* tones in C major */
typedef enum {
  NOTE_REST    = 0,
  NOTE_C       = 262,
  NOTE_Cd_Db   = 277,
  NOTE_D       = 293,
  NOTE_Dd_Eb   = 311,
  NOTE_E       = 330,
  NOTE_F       = 349,
  NOTE_Fd_Gb   = 370,
  NOTE_G       = 392,
  NOTE_Gd_Ab   = 415,
  NOTE_A       = 440,
  NOTE_Ad_Bb   = 466,
  NOTE_B       = 494,
  NOTE_HC      = 523,
} melodyNoteName;

typedef struct {
  melodyNoteName note;
  uint16_t duration;
} MelodyNote;

typedef struct {
  melodyId id;
  uint8_t len;
  const MelodyNote *notes;
} Score;

const Score * melody_get(melodyId id);

#endif /* MELODIES_H_ */
