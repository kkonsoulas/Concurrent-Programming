
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
    
    sem_t *sem = (sem_t*) input;
    
    printf("Process attempting to down sem\n");
    mythreads_sem_down(sem);
    printf("Process entered critical area\n");
    mythreads_yield();

    printf("Process about to leave critical area\n");
    mythreads_sem_up(sem);

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

    mythreads_join(&t1);
    mythreads_join(&t2);
    
    printf("Destroying semaphore testSem:\n");
    res = mythreads_sem_destroy(&testSem);
    print_destroy_result(res);

    printf("\e[41mEnd of test program test\e[49m\n");
    
    return (0);    
}