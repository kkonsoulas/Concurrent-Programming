#include "monitor.h"

void monitor_init(monitor_t *monitor){
    pthread_mutex_init(&monitor->monitorLock,NULL);
    pthread_mutex_init(&monitor->idLock,NULL);
    pthread_cond_init(&monitor->condLock,NULL);

    
    monitor->lastId = 0;
    monitor->currentId = 0;
    return;
}

void monitor_destroy(monitor_t* monitor){
    pthread_mutex_destroy(&monitor->monitorLock);
    pthread_mutex_destroy(&monitor->idLock);
    pthread_cond_destroy(&monitor->condLock);
}

void monitor_lock(monitor_t* monitor){
    unsigned id;
    //get id
    pthread_mutex_lock(&monitor->idLock);
    id = monitor->lastId;
    monitor->lastId++;
    pthread_mutex_unlock(&monitor->idLock);

    // FIFO monitor Queue
    while(1){
        pthread_mutex_lock(&monitor->monitorLock);
        

        if(id == monitor->currentId){
            monitor->currentId++;
            break;
        }
        pthread_mutex_unlock(&monitor->monitorLock);
        

    }

    return;
}

void monitor_unlock(monitor_t* monitor){
    pthread_mutex_unlock(&monitor->monitorLock);
}

void monitor_wait(monitor_t* monitor){
    pthread_cond_wait(&monitor->condLock,&monitor->monitorLock);
    pthread_mutex_unlock(&monitor->monitorLock);
    monitor_lock(monitor);
}

void monitor_notify(monitor_t* monitor){
    pthread_cond_signal(&monitor->condLock);
}