#ifndef _THREAD_H
#define _THREAD_H
#include "list.h"
#include "mycoroutines.h"


struct semaphore {
    int semid;
    int value;
    char initialized;
};
typedef struct semaphore sem_t;

typedef struct blockerStruct{
    unsigned blockerId;
    sem_t *sem;
    char state;
    node_t* threadNode;
} blocker_t;

//Starting Thread System (Scheduler Init)
int mythreads_init();

int mythreads_create(thr_t *thr, void (body)(void *), void *arg); 

int mythreads_yield(); 

int mythreads_join(thr_t *thr);

int mythreads_destroy(thr_t *thr);

int mythreads_sem_create(sem_t *s, int val); 

int mythreads_sem_down(sem_t *s);

int mythreads_sem_up(sem_t *s); 

int mythreads_sem_destroy(sem_t *s);

unsigned mythreads_self();

int clear_everything();

#endif