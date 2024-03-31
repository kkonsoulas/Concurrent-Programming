// Our  binary semaphore implementation
#ifndef MY_SEMAPHORE_H
#define MY_SEMAPHORE_H
#include <sys/ipc.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include "../monitor.h"

struct mysem {
    monitor_t monitor;
    int value;
    char initialized;
};
typedef struct mysem mysem_t;

//Inits semaphore (s) to value (n), returns:
// 1 for success
// 0 if already initialized
// -1 if n != 1,0
int mysem_init(mysem_t *s, int n);

//decrease semaphore by one 
// 1 for success
//-1 if semaphore is not initialized
int mysem_down(mysem_t *s);

//Increases semaphore by one returns:
//1 for success
//0 if already 1
//-1 if not initialized
int mysem_up(mysem_t *s);

// Destroys semaphore, returns:
// 1 for success
//-1 if semaphore is not initialized/already destroyed
int mysem_destroy(mysem_t *s);


#endif