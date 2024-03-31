#include "mythreads.h"
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <stdio.h>

#define ALARM_INTERVAL_SEC 0
#define ALARM_INTERVAL_MILI 100

list_t *process_list;
list_t *blocked_list;
node_t *current_process;
struct itimerval timer;
unsigned next_sem_id = 0;

char alarmTriggered;
char yielded = 0;

void scheduler() {
    printf("Scheduling...\n");

    node_t *curr;
    curr = current_process->next;

    if (alarmTriggered == 0 && yielded == 0) {
		printf("Finished process %u\n", current_process->thread.id);

        /* Check the blocked process list for someone waiting for the finished process*/
        search_for_blocked(process_list, blocked_list, current_process->thread.id, BLOCKED_BY_THREAD);
        process_list->size--;

        /* Delete the process */
        destroy_node(detach_byaddress(current_process));
    }

    if(curr == process_list->head){
        curr = curr->next;
    }

    if(curr == process_list->head){
        fprintf(stderr, "Last active process blocked no active threads\n");
        exit(43);
    }
    current_process = curr;
    
    if(alarmTriggered == 1){
        alarmTriggered = 0;
    }
    else{
        yielded = 0;
    }
    
    printf("Process id %d about to run\n", curr->thread.id);

    setcontext(&current_process->thread.coroutine.context);

}

void timer_handler(int signum) {
    printf("Time!\n");
    alarmTriggered = 1;

    mycoroutines_switchto(&current_process->thread.coroutine ,&process_list->scheduler->coroutine);

}

void setup_timer() {
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &timer_handler;

    sigaction(SIGVTALRM, &sa, NULL);

    timer.it_value.tv_sec = ALARM_INTERVAL_SEC;
    timer.it_value.tv_usec = ALARM_INTERVAL_MILI;
    timer.it_interval = timer.it_value;

    setitimer(ITIMER_VIRTUAL, &timer, NULL);
}

//Thread Blocker and Destroyer function
void blocker(void* args){
    blocker_t* data = (blocker_t*) args;
    node_t *node;

    if (data->state == KILLED_THREAD) {
        printf("Attempting to kill %u\n", data->blockerId);
        //if a process tries to delete itself 
        if(data->blockerId == current_process->thread.id) {
            //deleted from scheduler and unblock waiting threads
		    setcontext(&process_list->scheduler->coroutine.context);
            return;
        }

        
        node_t *result;
        result = search_list (process_list, data->blockerId);

        if (result == NULL) {
            result = search_list(blocked_list, data->blockerId);
            if(result == NULL) {
                return;
            }
        }

        node = result;
        node = detach_byaddress(node);
        mycoroutines_destroy(&node->thread.coroutine);
        free(node);

        setcontext(&current_process->thread.coroutine.context);
        
        return;
    }

    if(data->state == BLOCKED_BY_THREAD){
        node = search_list(process_list, data->blockerId);
        if (node == NULL) {
            setcontext(&current_process->thread.coroutine.context);
            return;
        }
        current_process = data->threadNode->prev;
        process_list->size--;

        node = detach_byaddress(data->threadNode);
        node->thread.state = BLOCKED_BY_THREAD;
        node->thread.blockerId = data->blockerId;
        printf("Appending thread %d to blocked list waiting to join with %u\n", node->thread.id, node->thread.blockerId);
        append(blocked_list, node, 1);

		yielded = 1;
       
		setcontext(&process_list->scheduler->coroutine.context);
        
    }

    if(data->state == SEM_DOWN){
        printf("Thread %u down semid: %u\n", current_process->thread.id, data->sem->semid);
        if (data->sem->value == 1) {
            data->sem->value = 0;
            
            setcontext(&data->threadNode->thread.coroutine.context);
            return;
        }
        current_process = data->threadNode->prev;
        process_list->size--;


        node = detach_byaddress(data->threadNode);
        node->thread.state = BLOCKED_BY_SEM;
        node->thread.blockerId = data->sem->semid;
        printf("Appending thread %d to blocked list waiting for semaphore\n", node->thread.id);
        append(blocked_list, node, 1);
        
        yielded = 1;
		setcontext(&process_list->scheduler->coroutine.context);

        return;
    }

    if (data->state == SEM_UP) {
        printf("Thread %u up semid: %u\n", current_process->thread.id, data->sem->semid);
        if(data->sem->value == 1) {
            fprintf(stderr, "Lost up\n");
            setcontext(&data->threadNode->thread.coroutine.context);
            return;
        }
        
        /* Find the first blocked thread waiting for the semaphore */
        int res = search_for_blocked(process_list, blocked_list, data->sem->semid, BLOCKED_BY_SEM);
        if (res == 0) {
            data->sem->value = 1;
        }

        setcontext(&data->threadNode->thread.coroutine.context);
        return; 
    }
    
    setcontext(&data->threadNode->thread.coroutine.context);
    return;
}


int mythreads_init() {

    process_list = malloc(sizeof(list_t));
    blocked_list = malloc(sizeof(list_t));
    init_list(process_list);
    init_list(blocked_list);
    blocked_list->head->thread.id = -2;
    blocked_list->scheduler = NULL;
    blocked_list->blocker = NULL;

    //initialise scheduler thread
    process_list->scheduler = malloc(sizeof(thr_t));
    mycoroutines_init(&process_list->scheduler->coroutine);

    //initialise blocker thread
    process_list->blocker = malloc(sizeof(thr_t));
    mycoroutines_init(&process_list->blocker->coroutine);

    // Modify the signal mask of scheduler & blocker to ignore SIGALRM
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    process_list->scheduler->coroutine.context.uc_sigmask = mask;
    mycoroutines_create(&process_list->scheduler->coroutine,scheduler,NULL);

    
    process_list->blocker->coroutine.context.uc_sigmask = mask;

    node_t *main = malloc(sizeof(node_t));
    mycoroutines_init(&main->thread.coroutine);
    main->thread.state = RUNNING;
    append(process_list,main, 0);
    current_process = main;

    setup_timer();

    return 0;
}

int mythreads_create(thr_t *thr, void (body)(void *), void *arg){

    node_t *threadNode = malloc(sizeof(node_t));
    threadNode->thread.state = RUNNING;
    mycoroutines_init(&threadNode->thread.coroutine);
    threadNode->thread.coroutine.context.uc_link = &process_list->scheduler->coroutine.context;
    mycoroutines_create(&threadNode->thread.coroutine, body, arg);
    append(process_list , threadNode, 0);
	*thr = threadNode->thread;

    printf("Created thread %u\n", thr->id);
    
    return 0;
}

int mythreads_yield() {
    printf("Process %u yielded\n", current_process->thread.id);
    yielded = 1;

    mycoroutines_switchto(&current_process->thread.coroutine ,&process_list->scheduler->coroutine);

    return 0;
}

int mythreads_join(thr_t *thr){

	blocker_t args;
    
	args.blockerId = thr->id;
	args.state = BLOCKED_BY_THREAD;
	args.threadNode = current_process;

    mycoroutines_destroy(&process_list->blocker->coroutine);
	mycoroutines_create(&process_list->blocker->coroutine,blocker,&args);
    mycoroutines_switchto(&current_process->thread.coroutine ,&process_list->blocker->coroutine);    
    
	return 0;
}

int mythreads_destroy(thr_t *thr) {
    blocker_t args;
    

    args.state = KILLED_THREAD;
    args.blockerId = thr->id;
    printf("Destroying thread %d\n",thr->id);
    
    mycoroutines_destroy(&process_list->blocker->coroutine);
    mycoroutines_create(&process_list->blocker->coroutine,blocker,&args);
    mycoroutines_switchto(&current_process->thread.coroutine, &process_list->blocker->coroutine);

    return 0;
}

int clear_everything() {
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 0;
    setitimer(ITIMER_REAL, &timer, NULL);

    clear(process_list);
    clear(blocked_list);

    return 0;
}

int mythreads_sem_create(sem_t *s, int val) {
    if (s->initialized == 'T') {
        printf("Sem already initialized\n");
        return 0;
    }

    if (val != 0 && val != 1)  {
        printf("Invalid sem value\n");
        return -1;
    }

    s->semid = next_sem_id;
    next_sem_id++;
    s->value = val;
    s->initialized = 'T';

    printf("Semid: %u created with init: %d\n", s->semid, s->value);

    return 1;
} 

int mythreads_sem_down(sem_t *s) {
    
    if (s->initialized != 'T') {
        return -1;
    }
    blocker_t args;
    
	args.blockerId = s->semid;
	args.state = SEM_DOWN;
	args.threadNode = current_process;
    args.sem = s;
    
    mycoroutines_destroy(&process_list->blocker->coroutine);
	mycoroutines_create(&process_list->blocker->coroutine,blocker,&args);
    mycoroutines_switchto(&current_process->thread.coroutine ,&process_list->blocker->coroutine);    

    return 1;    
}

int mythreads_sem_up(sem_t *s) {
    if (s->initialized != 'T') {
        return -1;
    }

    blocker_t args;
    
	args.blockerId = s->semid;
	args.state = SEM_UP;
	args.threadNode = current_process;
    args.sem = s;
    
    mycoroutines_destroy(&process_list->blocker->coroutine);
	mycoroutines_create(&process_list->blocker->coroutine,blocker,&args);
    mycoroutines_switchto(&current_process->thread.coroutine ,&process_list->blocker->coroutine);    

    return 1;  
}

unsigned mythreads_self() {
    return current_process->thread.id;
}

int mythreads_sem_destroy(sem_t *s) {
    if (s->initialized != 'T') {
        return -1;
    }
    s->initialized = 'F';

    return 1;
}