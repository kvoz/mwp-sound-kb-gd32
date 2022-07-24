#include <string.h>
#include "queue.h"

// TODO: head and tail of queue is uint16_t
// TODO: record number of queue is uint8_t, so limit is 256
// TODO: there is no rewrite on overflow of queue records

// Sound queue instances.
#define SOUND_EVENT_QUEUE_RECNUM    (10)
#define SOUND_EVENT_QUEUE_RECSIZE   (2)   // in bytes
uint8_t sound_q[SOUND_EVENT_QUEUE_RECNUM * SOUND_EVENT_QUEUE_RECSIZE] = {0};

// Common queue structure.
typedef struct Queue_t{
  void * const q;

  uint16_t in;
  uint16_t out;
  uint16_t cnt;
  const uint8_t recNum;
  const uint8_t recSize;
} Queue;

// Declaration of array of pointers to queues.
Queue queues[QUEUE_NUM] = {
    [QUEUE_SOUND] = { .q = sound_q, .in = 0, .out = 0, .recNum = SOUND_EVENT_QUEUE_RECNUM, .recSize = SOUND_EVENT_QUEUE_RECSIZE},
};

static inline uint8_t queue_isFull(const queueId id) {
  return (queues[id].cnt == queues[id].recNum) ? 1 : 0;
}

static inline uint8_t queue_isEmpty(const queueId id) {
  return (queues[id].cnt == 0) ? 1 : 0;
}

/* Library behavior definitions. */
void queue_flush(const queueId id) {
  queues[id].in = 0;
  queues[id].out = 0;
  queues[id].cnt = 0;
}

void queue_put(const queueId id, void * element) {
  if (queue_isFull(id) == 1) return;

  uint8_t * const pStart = queues[id].q + (queues[id].recSize * queues[id].in);
  memcpy(pStart, element, queues[id].recSize);

  // Increment index for input records.
  if (queues[id].in < queues[id].recNum - 1) {
    queues[id].in++;
  } else {
    queues[id].in = 0;
  }

  // Increment record counter value.
  if (queue_isFull(id) == 0) {
    queues[id].cnt++;
  }
}

uint8_t queue_get(const queueId id, void * element) {
  if (queue_isEmpty(id) == 1) return 0;

  uint8_t * const pStart = queues[id].q + (queues[id].recSize * queues[id].out);
  memcpy(element, pStart, queues[id].recSize);

  // Increment index for output records.
  if (queues[id].out < queues[id].recNum - 1) {
    queues[id].out++;
  } else {
    queues[id].out = 0;
  }

  queues[id].cnt--;
  return 1;
}
