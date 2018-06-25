#include "glib_queue.h"

static glib_queue_t* glib_queue_init()
{
    printf("%s:%d\n", __func__, __LINE__);
    glib_queue_t *q = malloc(sizeof (glib_queue_t));
    q->_queue = g_queue_new();
    q->waiting_threads = 0;
    pthread_mutex_init(&(q->mutex), NULL);
    pthread_cond_init(&(q->cond), NULL);
    return q;
}

static int glib_queue_push(glib_queue_t* q, void *data)
{
    pthread_mutex_lock(&(q->mutex));
    g_queue_push_tail(q->_queue, data);
    if (q->waiting_threads > 0)
    {
        pthread_cond_signal(&(q->cond));
    }
    pthread_mutex_unlock(&(q->mutex));
    return 0;
}

static int glib_queue_pop(glib_queue_t* q, void **data)
{
    int ret = 0;

    pthread_mutex_lock(&(q->mutex));

    if (!g_queue_is_empty(q->_queue))
        *data = g_queue_pop_head(q->_queue);
    else
        ret = -1;

    pthread_mutex_unlock(&(q->mutex));
    return ret;
}

static int glib_queue_update(glib_queue_t* q)
{
}

struct queue_ops glib_queue_ops = {
    .init = glib_queue_init,
    .push = glib_queue_push,
    .pop = glib_queue_pop,
    .update = glib_queue_update,
};
