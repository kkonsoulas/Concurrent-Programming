#include "../assignment1/mySemLib.h"
#include <pthread.h>
#include <ctype.h>

#define RED 'R'
#define BLUE 'B'
#define SIZE_FACTOR 50

struct threadInfo {
    pthread_t thread;
    char color;
    char* bridgeColor;
    int* bridgeCapacity;
    int maxBridgeCapacity;
    int* myColorCars;
    int* oppositeColorCars;
    int* carsPassed;
    int crossTime; // How long for a car to cross
    int fairnessFactor; // How many cars of one color allowed to pass (multiplied by bridge capacity)
    mysem_t* bridgeAccess;
    mysem_t* myCarsAccess;
    mysem_t* oppCarsAccess;
    mysem_t* myQ;
    mysem_t* oppositeQ;
};
typedef struct threadInfo threadInfo_t;

void onBridge(char color, int crossTime){
    if(color == BLUE){
        printf("\e[44m%4ld  on Bridge\e[49m\n",pthread_self());
    }
    else{        
        printf("\e[41m%4ld on Bridge\e[49m\n",pthread_self()); 
    }

    sleep(crossTime);

    if(color == BLUE){
        printf("\e[44m%4ld  getting off Bridge\e[49m\n",pthread_self());
    }
    else{        
        printf("\e[41m%4ld getting off Bridge\e[49m\n",pthread_self()); 
    }
}

void* crossBridge(void *args) {
    threadInfo_t *thread_info = (threadInfo_t*) args;

    if(thread_info->color == RED){
        printf("\e[101m%4ld arrived on the west shore\e[49m\n",pthread_self()); 
    }
    else {
        printf("\e[104m%4ld arrived on the east shore\e[49m\n",pthread_self());
    }

    mysem_down(thread_info->myCarsAccess);
    (*thread_info->myColorCars)++;
    mysem_up(thread_info->myCarsAccess);

    mysem_down(thread_info->bridgeAccess);
    // If no cars have passed the first one passes
    if(*thread_info->bridgeCapacity == thread_info->maxBridgeCapacity){
        *thread_info->bridgeColor = thread_info->color;
    }
    
    //Wait to Queue
    if (*thread_info->bridgeCapacity == 0 || *thread_info->bridgeColor != thread_info->color ) {
        mysem_up(thread_info->bridgeAccess);
        mysem_down(thread_info->myQ);
        mysem_down(thread_info->bridgeAccess);
    }

    //Get on to the bridge
    (*thread_info->bridgeCapacity)--;
    if(*thread_info->oppositeColorCars > 0) {
        (*thread_info->carsPassed)++;
    }
    else {
        *thread_info->carsPassed = 0;
    }
    
    mysem_down(thread_info->myCarsAccess);
    (*thread_info->myColorCars)--; 
    mysem_up(thread_info->myCarsAccess); 
    if(*thread_info->bridgeCapacity > 0                                                               // bridge has space
        && *thread_info->myColorCars > 0                                                              // There are cars of my color waiting 
        && *thread_info->bridgeColor == thread_info->color                                            // The bridge is my color
        && *thread_info->carsPassed < thread_info->maxBridgeCapacity * thread_info->fairnessFactor) { //have not exceeded FAIRNESS VALUE
        mysem_up(thread_info->myQ);
    }
    

    mysem_up(thread_info->bridgeAccess);

    //Crossing Bridge
    onBridge(thread_info->color, thread_info->crossTime);
    
    mysem_down (thread_info->bridgeAccess);
    (*thread_info->bridgeCapacity)++;
    
    if (*thread_info->bridgeColor == thread_info->color                                          	// The bridge is my color
        && *thread_info->myColorCars > 0                                                            //I have same Color cars waiting
        && *thread_info->bridgeCapacity == 1                                                        //I was the first who got down the bridge
        && (*thread_info->carsPassed < thread_info->maxBridgeCapacity * thread_info->fairnessFactor //have not exceeded FAIRNESS VALUE OR
            || *thread_info->oppositeColorCars == 0)) {                                             // no opposite color cars
        // printf("i am waking up the next my c\n");
        mysem_up(thread_info->myQ);
    }
    if (*thread_info->bridgeCapacity == thread_info->maxBridgeCapacity                                      //I am the last who got down the bridge
        && *thread_info->oppositeColorCars > 0                                                              //there are opposite color cars
        && (*thread_info->myColorCars == 0                                                                  //no same color cars OR
            || *thread_info->carsPassed >= thread_info->maxBridgeCapacity* thread_info->fairnessFactor)) {  // exceeded FAIRNESS VALUE
        // printf("i am waking up the next opp c\n");
        *thread_info->bridgeColor = (thread_info->color == RED) ? BLUE : RED;
        *thread_info->carsPassed = 0;
        mysem_up(thread_info->oppositeQ);
    }

    mysem_up (thread_info->bridgeAccess);

    return NULL;

}
    

int main(int argc, char* argv[]){
    char selection;
    unsigned amount = 0;
    //shared data
    char bridgeColor = BLUE;
    int redCars = 0;
    int blueCars = 0;
    int carsPassed = 0;
    int crossTime = 1; //default value if not specified by an arguement
    int fairnessFactor = 2; //also default value 
    mysem_t bridgeAccess;
    mysem_t redCarsAccess;
    mysem_t blueCarsAccess;
    mysem_t redQueueSem;
    mysem_t blueQueueSem;
    //
    int numCars = 0;
    
    if (argc < 2 && argc > 4) {
        printf("Invalid number of arguments\n");
        return 43;
    }
    if (argc == 3) {
        crossTime =  atoi(argv[2]);
    }
    else if (argc == 4) {
        crossTime =  atoi(argv[2]);
        fairnessFactor = atoi(argv[3]);
    }

    
    int bridgeCapacity = atoi(argv[1]);
    int maxBridgeCapacity = bridgeCapacity;
    int size = SIZE_FACTOR*bridgeCapacity;    
    threadInfo_t threadBuffer[size];   

    printf("Bridge Capacity: %d\n", bridgeCapacity);
    printf("Cross time: %d\n", crossTime);
    printf("Fairness Factor: %d\n", fairnessFactor);

    bridgeAccess.initialized = 'F';
    redCarsAccess.initialized = 'F';
    blueCarsAccess.initialized = 'F';
    redQueueSem.initialized = 'F';
    blueQueueSem.initialized = 'F';
    mysem_init(&bridgeAccess, 1);
    mysem_init(&redCarsAccess,1);
    mysem_init(&blueCarsAccess,1);
    mysem_init(&redQueueSem, 0);
    mysem_init(&blueQueueSem, 0);
    
    do {
        scanf(" %c", &selection);
        selection = toupper(selection);
        if (selection == 'Q') {
            break;
        }

        switch (selection) {
            case 'B': {}
            case 'R': {
                scanf("%d", &amount);
                
                for (int i = 0; i < amount; i++) {
                    if(numCars == size){
                        printf("\e[43mNo other cars allowed to generate(max %d)\e[49m\n", size);
                        break;
                    }
                    threadBuffer[numCars].color = selection;
                    threadBuffer[numCars].bridgeColor = &bridgeColor;
                    threadBuffer[numCars].bridgeCapacity = &bridgeCapacity;
                    threadBuffer[numCars].maxBridgeCapacity = maxBridgeCapacity;
                    threadBuffer[numCars].bridgeAccess = &bridgeAccess;
                    threadBuffer[numCars].carsPassed = &carsPassed;
                    threadBuffer[numCars].crossTime = crossTime;
                    threadBuffer[numCars].fairnessFactor = fairnessFactor;
                    
                    if (selection == RED) {
                        threadBuffer[numCars].myCarsAccess = &redCarsAccess;
                        threadBuffer[numCars].oppCarsAccess = &blueCarsAccess;
                        threadBuffer[numCars].myQ = &redQueueSem;
                        threadBuffer[numCars].oppositeQ = &blueQueueSem;
                        threadBuffer[numCars].myColorCars = &redCars;
                        threadBuffer[numCars].oppositeColorCars = &blueCars;

                    }
                    else{
                        threadBuffer[numCars].myCarsAccess = &blueCarsAccess;
                        threadBuffer[numCars].oppCarsAccess = &redCarsAccess;
                        threadBuffer[numCars].myQ = &blueQueueSem;
                        threadBuffer[numCars].oppositeQ = &redQueueSem;
                        threadBuffer[numCars].myColorCars = &blueCars;
                        threadBuffer[numCars].oppositeColorCars = &redCars;
                        
                    }

                    
                    pthread_create(&threadBuffer[numCars].thread, NULL ,crossBridge,  &threadBuffer[numCars]);
                    numCars++;
                }
                
                break;
            }
            case 'P': {
                printf("%c\n",selection);
                printf("\e[44mBlue Cars waiting: %d\e[49m\n",blueCars);
                printf("\e[41mRed Cars waiting: %d\e[49m\n",redCars); 
                break;
            }
            case 'S': {
                scanf("%d", &amount);
                sleep(amount);
                break;
            }
            case 'Q':
                break;
            default:  {
                printf("Invalid character\n");
                break;
            }
        }
    } while (selection != 'Q');

    //wait for cars to pass before demolishing the bridge
    for(int i=0; i<numCars ;i++) {
        pthread_join(threadBuffer[i].thread,NULL);
    }

    
    mysem_destroy(&bridgeAccess);
    mysem_destroy(&redCarsAccess);
    mysem_destroy(&blueCarsAccess);
    mysem_destroy(&redQueueSem);
    mysem_destroy(&blueQueueSem);
    
    
    return 0;
}