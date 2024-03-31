#ifndef _LIST_H
#define _LIST_H
#include "mycoroutines.h"

#define RUNNING 0
#define BLOCKED_BY_SEM 1
#define BLOCKED_BY_THREAD 2
#define KILLED_THREAD 3
#define SEM_DOWN 4
#define SEM_UP 5

struct threads {
    // void * 	handle;
    co_t coroutine;
    //0 = running
    //1 = blocked by semaphore
    //2 = blocked by thread
    //3 = killed thread
    char state;
    unsigned blockerId;
    unsigned id;
};
typedef struct threads thr_t;

struct node {
    struct node *next;
    struct node *prev;
    // unsigned id;
    thr_t thread;
};
typedef struct node node_t;

struct list {
    node_t *head;
    unsigned size;
    unsigned nextId;
    thr_t* scheduler;
    thr_t* blocker;
};
typedef struct list list_t;


int init_list(list_t *list);

int append(list_t *list, node_t *node, char giveId);

int search_and_delete(list_t *list, unsigned id);

int clear(list_t *list);

void print_list(list_t *list);

node_t* detach_byaddress(node_t *detachable);

node_t* search_list(list_t *list, unsigned id);

void destroy_node(node_t *node);

int search_for_blocked(list_t* proccess_list, list_t *blocked_list, unsigned blocker_id, char ReasonForBlock);

#endif