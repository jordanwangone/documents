#include <mqueue.h>
#include <sched.h>
#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <event2/event.h>
#include <event2/event_struct.h>
#include <event2/util.h>
#include <string.h>
#include "glib_queue.h"

#define QUEUE_NAME "/mymq"

typedef struct {
    int len;
    struct timeval lasttime;
    char data[0];
} posix_mq_msg;

void print_elapse(struct timeval lasttime, glib_queue_t* queue)
{
    static int first_enter = 0;
    struct timeval newtime, difference;
    static int max, min;
    static long long count = 0, result;

    evutil_gettimeofday(&newtime, NULL);
    evutil_timersub(&newtime, &lasttime, &difference);

    int elapse =  difference.tv_sec * 1000000 + difference.tv_usec;

    if (first_enter < 1000) {
        first_enter++;
//        max = min = elapse;
    } else {
        result += elapse;
        count++;
        if (elapse > max)
            max = elapse;
//        if (elapse < min)
//            min = elapse;
        if (!(count % 1000000))
        printf("max:%d, min:%d, average: %llu, count:%d, elapse: %d\n", max, min, result/count, count, elapse);
    }

//    printf("elapse: %d\n", elapse);
//    glib_queue_ops.push(queue, &elapse);
//    while (!glib_queue_ops.pop(queue, &tmp)) {
}

int get_msg_len(int len)
{
    return sizeof(struct timeval) + sizeof(int) + len;
}

posix_mq_msg* mk_msg(int len, posix_mq_msg* s_msg)
{
    int size = get_msg_len(len);
    evutil_gettimeofday(&s_msg->lasttime, NULL);
    s_msg->len = size;

    return s_msg;
}

int posix_rmq_create()
{
    mqd_t queue = mq_open(QUEUE_NAME, O_RDONLY);

    if (-1 == queue) {
        perror("mq_open error:");
        return -1;
    }
    return queue;
}

int posix_wmq_create()
{
    mqd_t queue = mq_open(QUEUE_NAME, O_WRONLY);

    if (-1 == queue) {
        perror("mq_open error:");
        return -1;
    }
    return queue;
}


int posix_mq_unlink(void)
{
    int ret = mq_unlink(QUEUE_NAME);
    if (0 != ret) {
        perror("mq_unlink error:");
        return -1;
    }
    return 0;
}

int posix_mq_send(mqd_t queue, int len, posix_mq_msg* msg)
{
//    usleep(0.1);
    posix_mq_msg* s_msg = mk_msg(len, msg);

    int ret = mq_send(queue, s_msg, s_msg->len, 0);
    if (0 != ret) {
        perror("mq_send error:");
        printf("ret:%d, %s\n", ret, strerror(errno));
        return -1;
    }
//    free(s_msg);

    return 0;
}

int posix_mq_recv(mqd_t queue, int len, glib_queue_t* time_q, posix_mq_msg* buffer)
{
    posix_mq_msg* msg = buffer;
    int msg_len = get_msg_len(len);
    int ret = mq_receive(queue, msg, msg_len, 0);
    if (ret != msg_len) {
        perror("mq_recv error:");
        printf("ret:%d, %d, %d\n", ret, errno, msg_len);
        return -1;
    } else {
        print_elapse(msg->lasttime, time_q);
    }
//    free(msg);

    return 0;
}
glib_queue_t* queue;

void print_info(int sig)
{

    exit(1);
}
int main(int argc, void* argv[])
{
    int len = 4076;
    int pid = fork();
    if (pid == 0) {
#if 0
    int maxpri = sched_get_priority_max(SCHED_FIFO);

    struct sched_param sp;
    sp.sched_priority = maxpri;
//    sched_setscheduler(0, SCHED_FIFO, &sp);
          if (sched_setscheduler(getpid(), SCHED_FIFO, &sp) == -1) //设置优先级

     {
            perror("sched_setscheduler() failed");
            exit(1);
     } 
#endif
        int msg_len = get_msg_len(len);

        posix_mq_msg* msg = malloc(msg_len);
            mqd_t mq= mq_open(QUEUE_NAME, O_WRONLY);
//        for (int i = 0; i < 10000000; i++)
            while(1)
            {
//                usleep(0.1);
                posix_mq_send(mq, len, msg);
            }


    } else {
#if 0
    int maxpri = sched_get_priority_max(SCHED_FIFO);

    struct sched_param sp;
    sp.sched_priority = maxpri;
              if (sched_setscheduler(getpid(), SCHED_FIFO, &sp) == -1) //设置优先级

     {
            perror("sched_setscheduler() failed");
            exit(1);
     } 
#endif
//        queue = glib_queue_ops.init();
        int msg_len = get_msg_len(len);

        posix_mq_msg* msg = malloc(msg_len);
        mqd_t mq= mq_open(QUEUE_NAME, O_RDONLY);
//        struct sigaction handler;
////        handler.sa_handler = print_info;
//        sigemptyset(&handler.sa_mask);
//        handler.sa_flags = 0;
//        sigaction(SIGINT, &handler, NULL);
//        for (int i = 0; i < 10000000; i++)
        while(1)
            posix_mq_recv(mq, len, queue, msg);
    }

    waitpid(pid);
    posix_mq_unlink();
}
