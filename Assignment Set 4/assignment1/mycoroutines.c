#include "mycoroutines.h"
#include <stdlib.h>
#include <stdio.h>
#define STACK_SIZE 8192

int mycoroutines_init(co_t *main) {
    getcontext(&main->context);
    return 0;
}

int mycoroutines_create(co_t *co, void (body)(void *), void *arg) {
    getcontext(&co->context);

    co->body = body;
    co->arg = arg;

    co->context.uc_stack.ss_sp = malloc(STACK_SIZE);
    co->context.uc_stack.ss_size = STACK_SIZE;

    /* When coroutine ends exit process */
    /* All swaps will be done manually for this assigment */
    co->context.uc_link = NULL;
    makecontext(&co->context, (void (*)(void))co->body, 1, co->arg);

    return 0;
}

int mycoroutines_switchto(co_t *co) {
    co_t current_context;

    /* Not initialized */
    if (!co->context.uc_stack.ss_sp) {
        mycoroutines_create(co, co->body, co->arg);
    }

    swapcontext(&current_context.context, &co->context);
    return 0;
}

int mycoroutines_destroy(co_t *co) {

    /* Not initialized */
    if (co->context.uc_stack.ss_size == 0) {
        return -1;
    }

    free(co->context.uc_stack.ss_sp);

    co->context.uc_stack.ss_sp = NULL;
    co->context.uc_stack.ss_size = 0;

    return 0;
}
