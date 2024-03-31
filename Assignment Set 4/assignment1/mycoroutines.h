#ifndef _COROUTINE_H
#define _COROUTINE_H
#include <ucontext.h>

typedef struct {
    ucontext_t context;
    void (*body)(void*);
    void* arg;
} co_t;

/* Init main coroutine */
int mycoroutines_init(co_t *main); 

/* Create coroutine */
int mycoroutines_create(co_t *co, void (body)(void *), void *arg); 

/* Switch coroutine */
int mycoroutines_switchto(co_t *co); 

/* Destroy coroutine */
int mycoroutines_destroy(co_t *co);

#endif 