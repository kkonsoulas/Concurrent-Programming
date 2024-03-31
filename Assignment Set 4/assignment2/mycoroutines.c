// #include <ucontext.h>
#include "mycoroutines.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define STACK_SIZE 4096

// Initialize the main coroutine context
int mycoroutines_init(co_t *main) {
    getcontext(&main->context);              
    return 0;
}

int mycoroutines_create(co_t *co, void (body)(void *), void *arg) {
    co->body = body;
    co->arg = arg;

    co->context.uc_stack.ss_sp = malloc(STACK_SIZE);
    co->context.uc_stack.ss_size = STACK_SIZE;

    makecontext(&(co->context), (void (*)(void))co->body, 1, co->arg);

    return 0;
}

int mycoroutines_switchto(co_t *current, co_t *new) {

    if (!(new->context.uc_stack.ss_sp)) {
        fprintf(stderr,"Should never see this Fatal Error\n");
        exit(43);
    }
    swapcontext(&current->context, &new->context);

    return 0;
}


int mycoroutines_destroy(co_t *co) {

    if (co->context.uc_stack.ss_size == 0) {
        return -1;
    }
    
    free(co->context.uc_stack.ss_sp);

    co->context.uc_stack.ss_sp = NULL;
    co->context.uc_stack.ss_size = 0;

    return 0;
}
