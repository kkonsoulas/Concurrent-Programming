#include "mySemLib.h"
extern int errno;


int mysem_init(mysem_t *s, int n) {

    //check if valid value
    if(n != 0 && n != 1) {
        return 0;
    }

    if (s->initialized == 'T') {
        return -1; //semaphore already exists
    }

    monitor_init(&s->monitor);
    s->value = n;
    s->initialized = 'T';

    return 1;    
}

int mysem_down(mysem_t *s) {
    monitor_lock(&s->monitor);

    if (s->initialized != 'T') {
        monitor_unlock(&s->monitor);
        return -1; //semaphore not initialized
    }

    while(1){
        if(s->value == 1){
            s->value = 0;
            break;
        }
        monitor_wait(&s->monitor);
    }


    monitor_unlock(&s->monitor);
    return 1;
}

int mysem_up(mysem_t *s){
    monitor_lock(&s->monitor);

    if (s->initialized != 'T') {
        monitor_unlock(&s->monitor);
        return -1; //semaphore not initialized
    }

    if (s->value == 1) {
        monitor_unlock(&s->monitor);
        return 0;
    }
    
    
    s->value = 1;
    monitor_notify(&s->monitor);

    monitor_unlock(&s->monitor);
    return 1;
}

int mysem_destroy(mysem_t *s) {

    if (s->initialized == 'F') {
        return -1;
    }

    monitor_destroy(&s->monitor);
    
    return 1;
}