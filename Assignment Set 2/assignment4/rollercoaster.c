#include "../assignment1/mySemLib.h"
#include <pthread.h>
#include <ctype.h>

#define SIZE_FACTOR 50

struct threadInfo {
    pthread_t thread;
    int* onBoard;
    char* boarding;
    int maxTrainCapacity;
    int *noService;
    mysem_t* trainSem;
    mysem_t* onBoardSem;
    mysem_t* boardingSem;
    mysem_t* gettingOffSem;
};
typedef struct threadInfo threadInfo_t;

void* passengers (void *args) {
    threadInfo_t *passengerInfo = (threadInfo_t*) args;

    printf("\e[44mPassenger %ld arrived at the station\e[49m\n", pthread_self());

    while(1) {
        mysem_down(passengerInfo->boardingSem);

        mysem_down(passengerInfo->onBoardSem);
        if((*passengerInfo->onBoard) < passengerInfo->maxTrainCapacity && (*passengerInfo->boarding) == 1) {
            if (*passengerInfo->noService == 1) {
                printf("\e[48;5;88mPassenger %ld didn't get on the train (no Service)\e[49m\n", pthread_self());
                mysem_up(passengerInfo->boardingSem);
                mysem_up(passengerInfo->onBoardSem);
                return NULL;
            }
            printf("\e[42mPassenger %ld got on the train\e[49m\n", pthread_self());
            (*passengerInfo->onBoard)++;

            if(*passengerInfo->onBoard == passengerInfo->maxTrainCapacity) {
                printf("\e[48;5;22mPassenger %ld was the last to get on the train\e[49m\n", pthread_self());
                *passengerInfo->boarding = 0;
                mysem_up(passengerInfo->trainSem);
                mysem_up(passengerInfo->onBoardSem);
                break;
            }
            mysem_up(passengerInfo->boardingSem);
            mysem_up(passengerInfo->onBoardSem);
            break;
    
        }
        mysem_up(passengerInfo->onBoardSem);
    }    //
    
    mysem_down(passengerInfo->gettingOffSem);
    //

    mysem_down(passengerInfo->onBoardSem);
    
    printf("\e[41mPassenger %ld got off the train\e[49m\n", pthread_self());
    (*passengerInfo->onBoard)--;
    
    if(*passengerInfo->onBoard == 0) {
        printf("\e[48;5;88mPassenger %ld was the last to get off the train\e[49m\n", pthread_self());
        *passengerInfo->boarding = 1;
        mysem_up(passengerInfo->boardingSem);
        mysem_up(passengerInfo->onBoardSem);
        return NULL;
    }
    mysem_up(passengerInfo->onBoardSem);
    mysem_up(passengerInfo->gettingOffSem);

// terminate
    return NULL;
}

void* train (void *args) {
    threadInfo_t *trainInfo = (threadInfo_t*) args;
    
    while (1) {
        printf("\e[43mTrain waiting in Station!\e[49m\n");
        mysem_down(trainInfo->trainSem);
        if  (*trainInfo->noService == 1){
            mysem_up(trainInfo->gettingOffSem);
            break;
        }
        printf("\e[43mTrain Departure...\e[49m\n");
        sleep(5);
        printf("\e[43mTrain Finished Ride...\e[49m\n");
        mysem_up(trainInfo->gettingOffSem);
    }
    return NULL;
}

int main(int argc, char* argv[]){    
    int amount, numPassengers = 0;
    int onBoard = 0;
    char boarding = 1;
    char selection;
    int noService = 0;
    
    mysem_t trainSem;
    mysem_t onBoardSem;    
    mysem_t boardingSem;
    mysem_t gettingOffSem;     
    
    trainSem.initialized = 'F';
    onBoardSem.initialized = 'F';
    boardingSem.initialized = 'F';
    gettingOffSem.initialized = 'F';

    if (argc != 2) {
        printf("Invalid number of arguments\n");
        return 43;
    }
    int capacity = atoi(argv[1]);
    int size = SIZE_FACTOR*capacity;    
    threadInfo_t threadBuffer[size]; //passengers
    threadInfo_t trainInfo; //train


    trainInfo.onBoard = &onBoard;
    trainInfo.maxTrainCapacity = capacity;
    trainInfo.onBoardSem = &onBoardSem;
    trainInfo.boardingSem = &boardingSem;
    trainInfo.trainSem = &trainSem;
    trainInfo.gettingOffSem = &gettingOffSem;
    trainInfo.boarding = &boarding;
    trainInfo.noService = &noService;

    trainSem.initialized = 'F';
    onBoardSem.initialized = 'F';
    boardingSem.initialized = 'F';
    gettingOffSem.initialized = 'F';

    mysem_init(&trainSem, 0);
    mysem_init(&onBoardSem, 1);
    mysem_init(&boardingSem, 1);
    mysem_init(&gettingOffSem, 0);

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
                    threadBuffer[numPassengers].onBoardSem = &onBoardSem;
                    threadBuffer[numPassengers].boardingSem = &boardingSem;
                    threadBuffer[numPassengers].boarding = &boarding;
                    threadBuffer[numPassengers].trainSem = &trainSem;
                    threadBuffer[numPassengers].gettingOffSem = &gettingOffSem;
                    threadBuffer[numPassengers].noService = &noService;
                    
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
                break;
            default:  {
                printf("Invalid character\n");
                break;
            }
        }
    } while (selection != 'Q');

    noService = 1;
    mysem_up(trainInfo.trainSem);
    pthread_join(trainInfo.thread,NULL);
    //wait for passengers to pass before blowing up the train 
    for(int i=0; i<numPassengers ;i++) {
        pthread_join(threadBuffer[i].thread,NULL);
    }
    
    
    mysem_destroy(&onBoardSem);
    mysem_destroy(&boardingSem);
    mysem_destroy(&gettingOffSem);
    mysem_destroy(&trainSem);
    
    return 0;
}