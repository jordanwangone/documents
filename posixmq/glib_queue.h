#ifndef GLIB_QUEUE_H
#define GLIB_QUEUE_H

#include <stdlib.h>
#include <glib.h>

typedef struct glib_queue
{
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int waiting_threads;
    GQueue* _queue;
} glib_queue_t;

struct queue_ops {
    glib_queue_t*  (*init)();
    int (*push) (glib_queue_t* q, void *data);
    int (*pop) (glib_queue_t* q, void **data);
    int (*update) (glib_queue_t* q);
};


extern struct queue_ops glib_queue_ops;

#endif // GLIB_QUEUE_H
