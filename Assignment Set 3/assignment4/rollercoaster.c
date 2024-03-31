#include <pthread.h>
#include <ctype.h>
#include "monitor.h"

#define SIZE_FACTOR 50
#define GETTINGOFF 2
#define TRAIN_QUEUE 1
#define BOARDING_QUEUE 0

struct threadInfo {
    pthread_t thread;
    int* onBoard;
    char* boarding;
    int maxTrainCapacity;
    int *noService;
    monitor_t *monitor;
};
typedef struct threadInfo threadInfo_t;

void* passengers (void *args) {
    threadInfo_t *passengerInfo = (threadInfo_t*) args;

    printf("\e[44mPassenger %ld arrived at the station\e[49m\n", pthread_self());

    monitor_lock (passengerInfo->monitor);
    while(1) {
        if((*passengerInfo->onBoard) < passengerInfo->maxTrainCapacity && (*passengerInfo->boarding) == 1) {
            if (*passengerInfo->noService == 1) {
                printf("\e[48;5;88mPassenger %ld didn't get on the train (no Service)\e[49m\n", pthread_self());
                monitor_notify(passengerInfo->monitor, TRAIN_QUEUE);
                monitor_notify(passengerInfo->monitor, BOARDING_QUEUE);
                monitor_unlock(passengerInfo->monitor);
                return NULL;
            }
            printf("\e[42mPassenger %ld got on the train\e[49m\n", pthread_self());
            (*passengerInfo->onBoard)++;

            if(*passengerInfo->onBoard == passengerInfo->maxTrainCapacity) {
                printf("\e[48;5;22mPassenger %ld was the last to get on the train\e[49m\n", pthread_self());
                *passengerInfo->boarding = 0;
                monitor_notify(passengerInfo->monitor, TRAIN_QUEUE);

                break;
            }
            monitor_notify(passengerInfo->monitor, BOARDING_QUEUE);
           
            break;
    
        }
        monitor_wait(passengerInfo->monitor,BOARDING_QUEUE);
    }
    
    monitor_wait(passengerInfo->monitor, GETTINGOFF);
    
    printf("\e[41mPassenger %ld got off the train\e[49m\n", pthread_self());
    (*passengerInfo->onBoard)--;
    
    if(*passengerInfo->onBoard == 0) {
        printf("\e[48;5;88mPassenger %ld was the last to get off the train\e[49m\n", pthread_self());
        *passengerInfo->boarding = 1;
        monitor_notify(passengerInfo->monitor, BOARDING_QUEUE);
        monitor_unlock(passengerInfo->monitor);
        return NULL;
    }
    
// terminate
    monitor_notify(passengerInfo->monitor, GETTINGOFF);
    monitor_unlock(passengerInfo->monitor);
    return NULL;
}

void* train (void *args) {
    threadInfo_t *trainInfo = (threadInfo_t*) args;
    monitor_lock(trainInfo->monitor);
    while (1) {
        printf("\e[43mTrain waiting in Station!\e[49m\n");
        monitor_wait(trainInfo->monitor, TRAIN_QUEUE);
        if  (*trainInfo->noService == 1){
            monitor_notify(trainInfo->monitor, GETTINGOFF);
            
            break;
        }
        printf("\e[43mTrain Departure...\e[49m\n");
        sleep(5);
        printf("\e[43mTrain Finished Ride...\e[49m\n");
        monitor_notify(trainInfo->monitor, GETTINGOFF);
    }
    monitor_unlock(trainInfo->monitor);
    return NULL;
}

int main(int argc, char* argv[]){    
    int amount, numPassengers = 0;
    int onBoard = 0;
    char boarding = 1;
    char selection;
    int noService = 0;
    monitor_t monitor;
    
    
    
    if (argc != 2) {
        printf("Invalid number of arguments\n");
        return 43;
    }
    monitor_init(&monitor, 3);
    int capacity = atoi(argv[1]);
    int size = SIZE_FACTOR*capacity;    
    threadInfo_t threadBuffer[size]; //passengers
    threadInfo_t trainInfo; //train


    trainInfo.onBoard = &onBoard;
    trainInfo.maxTrainCapacity = capacity;
    trainInfo.boarding = &boarding;
    trainInfo.noService = &noService;
    trainInfo.monitor = &monitor;

    pthread_create(&trainInfo.thread, NULL, train,  &trainInfo);

    
    do {
        scanf(" %c", &selection);
        selection = toupper(selection);
        if (selection == 'Q') {
            noService = 1;
            
            
            break;
        }

        switch (selection) {
            case 'I': {
                scanf("%d", &amount);
                printf("%c %d\n",selection,amount);
                //
                for (int i = 0; i < amount; i++) {
                    if(numPassengers == size){
                        printf("\e[43mNo other cars allowed to generate(max %d)\e[49m\n", size);
                        break;
                    }
                    threadBuffer[numPassengers].onBoard = &onBoard;
                    threadBuffer[numPassengers].maxTrainCapacity = capacity;
                    threadBuffer[numPassengers].boarding = &boarding;
                    threadBuffer[numPassengers].noService = &noService;
                    threadBuffer[numPassengers].monitor = &monitor;
                    
                    pthread_create(&threadBuffer[numPassengers].thread, NULL, passengers,  &threadBuffer[numPassengers]);
                    numPassengers++;                
                }
                
                break;
            }
            case 'S': {
                scanf("%d", &amount);
                sleep(amount);
                break;
            }
            case 'Q':
                noService = 1;
                monitor_notify(&monitor, TRAIN_QUEUE);
                break;
            default:  {
                printf("Invalid character\n");
                break;
            }
        }
    } while (selection != 'Q');

    noService = 1;
    monitor_notify(&monitor, TRAIN_QUEUE);
    pthread_join(trainInfo.thread,NULL);
    //wait for passengers to pass before blowing up the train 
    for(int i=0; i<numPassengers ;i++) {
        pthread_join(threadBuffer[i].thread,NULL);
    }
    
    monitor_destroy(&monitor);
    
    return 0;
}