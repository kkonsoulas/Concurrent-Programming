#include "mySemLib.h"

void print_init_result(int res){
    // printf("Result: %d", res);
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

void *testThread(void *input) {
    
    mysem_t *sem = (mysem_t*) input;
    
    printf("Process %ld attempting to down sem\n", pthread_self());
    mysem_down(sem);
    printf("Process %ld entered critical area\n", pthread_self());
    sleep(2);
    printf("Process %ld about to leave critical area\n", pthread_self());
    mysem_up(sem);

    return NULL;
}

int main(int argc, char* argv[]) {
    printf("\e[43mStarting test program test\e[49m\n");
    pthread_t t1, t2;
    mysem_t testSem;
    int res;

    testSem.semid = 0;
    printf("Initialising semaphore testSem:\n");
    res = mysem_init(&testSem, 1);
    print_init_result(res);

    res = pthread_create(&t1,NULL, testThread, &testSem);
    if(res) {
        exit(-1);
    }
    res = pthread_create(&t2,NULL, testThread, &testSem);
    if(res) {
        exit(-1);
    }

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    
    printf("Destroying semaphore testSem:\n");
    res = mysem_destroy(&testSem);
    print_destroy_result(res);

    printf("\e[41mEnd of test program test\e[49m\n");
    return (0);    
}