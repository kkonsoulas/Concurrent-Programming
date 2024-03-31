#include<stdio.h>
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

int main(int argc, char *argv[]){
    int res;
    mysem_t sem1, unusedSem;

    printf("\e[43mStarting init test\e[49m\n");

    printf("Initialising semaphore sem1 with wrong value:\n");
    res = mysem_init(&sem1, 23);
    print_init_result(res);

    printf("Initialising semaphore sem1:\n");
    res = mysem_init(&sem1, 1);
    print_init_result(res);

    printf("Initialising the same semaphore sem1:\n");
    res = mysem_init(&sem1, 1);
    print_init_result(res);

    printf("Initialising the semaphore sem1 with wrong value:\n");
    res = mysem_init(&sem1, 42);
    print_init_result(res);

    printf("Destroying semaphore sem1:\n");
    res = mysem_destroy(&sem1);
    print_destroy_result(res);

    printf("Destroying again semaphore sem1:\n");
    res = mysem_destroy(&sem1);
    print_destroy_result(res);   


    printf("Destroying unitialized semaphore unusedSem:\n");
    res = mysem_destroy(&unusedSem);
    print_destroy_result(res);


    
    printf("\e[41mEnd of init test\e[49m\n");



    return 0;
}