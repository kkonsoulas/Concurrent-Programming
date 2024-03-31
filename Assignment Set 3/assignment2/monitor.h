#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef __MONITOR_H__
#define __MONITOR_H__

typedef struct monitor{
    pthread_mutex_t monitorLock;
    pthread_cond_t *condLock;
    pthread_mutex_t idLock;
    unsigned condNum;
    unsigned lastId;
    unsigned currentId;
} monitor_t;

void monitor_init(monitor_t *monitor, unsigned condNum);

void monitor_destroy(monitor_t* monitor);

void monitor_lock(monitor_t* monitor);

void monitor_unlock(monitor_t* monitor);

void monitor_wait(monitor_t* monitor,unsigned condIdx);

void monitor_notify(monitor_t* monitor,unsigned condIdx);

#endif