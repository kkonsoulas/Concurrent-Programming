#include "mySemLib.h"

void print_up_result(int res){
    // printf("Result: %d", res);
    if (res == 1) {
        printf("\e[32mSuccess\e[0m\n");
    }
    else if (res == 0) {
        printf("\e[31mFailed (lost up)\e[0m\n");
    }
    else { //-1
        printf("\e[33mFailed (not initialized)\e[0m\n");
    }
}

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


int main(int argc, char *argv[]) {
    printf("\e[43mStarting test program test\e[49m\n");
    mysem_t testSem;
    int res;
    // testSem.semid = 0;
    // printf("Attempting to up testSem\n");
    // res = mysem_up(&testSem);
    // print_up_result(res);

    printf("Initialising semaphore testSem to 0:\n");
    res = mysem_init(&testSem, 0);
    print_init_result(res);

    printf("Attempting to up testSem\n");
    res = mysem_up(&testSem);
    print_up_result(res);

    printf("Attempting to up testSem\n");
    res = mysem_up(&testSem);
    print_up_result(res);


    printf("Destroying semaphore testSem:\n");
    res = mysem_destroy(&testSem);
    if (res == 1) {
        printf("\e[32mSuccess\e[0m\n");
    }
    else {
        printf("\e[31mFailed\e[0m\n");
    }
    printf("\e[41mEnd of lost up test\e[49m\n");
}