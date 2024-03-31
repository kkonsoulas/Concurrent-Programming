
#include <stdio.h>
#include "list.h"
#include "mythreads.h"
#include "mycoroutines.h"
#include <stdlib.h>
#include <unistd.h>


void print_init_result(int res){
    if (res == 1) {
        printf("\e[32mSuccess\e[0m\n");
    }
    else if (res == -1) {
        printf("\e[31mFailed\e[0m\n");
    } 
    else {
        printf("\e[33mIncorrect value\e[0m\n");
    }
}

void print_destroy_result(int res){
    if (res == 1) {
        printf("\e[32mSuccess\e[0m\n");
    }
    else {
        printf("\e[31mFailed\e[0m\n");
    } 
}


void testThread(void *input) {
    unsigned interval = 0; 
    while (1) {
        interval++;
        if (interval % 100000 == 0) {
            printf("I am thread %u\n", mythreads_self());
        }
    } 

}

int main(int argc, char* argv[]) {
    printf("\e[43mStarting test program test\e[49m\n");
    thr_t t1, t2;
    sem_t testSem;
    int res;

    mythreads_init();


    printf("Initialising semaphore testSem:\n");
    res =  mythreads_sem_create(&testSem, 1);
    print_init_result(res);

    res = mythreads_create(&t1, testThread, &testSem);
    if(res) {
        exit(-1);
    }
    res = mythreads_create(&t2, testThread, &testSem);
    if(res) {
        exit(-1);
    }

    mythreads_yield();
    mythreads_yield();
    mythreads_yield();
    mythreads_destroy(&t1);
    mythreads_destroy(&t2);
    
    printf("Destroying semaphore testSem:\n");
    res = mythreads_sem_destroy(&testSem);
    print_destroy_result(res);

    printf("\e[41mEnd of test program test\e[49m\n");
    
    return (0);    
}