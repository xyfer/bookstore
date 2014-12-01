#ifndef QUEUE_H
#define QUEUE_H

#include "node.h"
#include <pthread.h>


struct queue 
{
    nodePTR head;
    pthread_mutex_t mutex;
    pthread_cond_t NOTempty;
};

typedef struct queue* queuePTR;

queuePTR createQueue();
const void* queuepeek(queuePTR);
const void* dequeue(queuePTR);

#endif
