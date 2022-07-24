#ifndef QUEUE_H_
#define QUEUE_H_

#include <stdint.h>

// Define queue names (ID).
typedef enum {
  QUEUE_SOUND,

  QUEUE_NUM
} queueId;


// Define queue sound elements.
enum {
  SOUND_MSG_SET_VOL = 1,
  SOUND_MSG_PLAY,
};

typedef struct {
  uint8_t msg;
  uint8_t val;
} queueRecSound;



void queue_flush(const queueId id);
void queue_put(const queueId id, void * element);
uint8_t queue_get(const queueId id, void * element);

#endif /* QUEUE_H_ */
