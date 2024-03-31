#include "mySemLib.h"
extern int errno;


int mysem_init(mysem_t *s, int n) {

    //check if valid value
    if(n != 0 && n != 1) {
        return 0;
    }

    if (s->initialized == 'T') {
        return -1; //semaphore already exists
    }

    do {
        s->semid = semget(IPC_PRIVATE, 2, IPC_CREAT | IPC_EXCL | 0666);

        if (s->semid == -1) {
            perror("semget, retrying...\n"); 
        }
    } while (s->semid == -1);

    semctl(s->semid, 0, SETVAL, n);
    semctl(s->semid, 1, SETVAL, 1); //second sem to ensure no lost up 
    s->initialized = 'T';

    return 1;    
}

int mysem_down(mysem_t *s) {
    struct sembuf action;
    int result;

    if (s->initialized != 'T') {
        return -1; //semaphore not initialized
    }

    action.sem_num = 0; // only one semaphore
    action.sem_op = -1; // reduce by one
    action.sem_flg = 0; // no flags
    result = semop(s->semid, &action, 1);    
    if (result == -1) {
        fprintf(stderr,"Value: %d Str: %s",errno,strerror(errno));
        perror("down failed, system fault...\n"); 
        exit(43);
    }
    
    return 1;
}

int mysem_up(mysem_t *s){
    struct sembuf action;
    int result;

    if (s->initialized != 'T') {
        return -1; //semaphore not initialized
    }

    //using the second semaphore we avoid undetected lost up
    action.sem_num = 1; // second sm
    action.sem_op = -1; // down
    action.sem_flg = 0; // no flag
    
    result = semop(s->semid, &action, 1); 
    if (result == -1) {
        fprintf(stderr,"Value: %d Str: %s",errno,strerror(errno));
        perror("down of helper semaphore failed, system fault...\n"); 
        exit(43);
    }
    int value = semctl(s->semid, 0, GETVAL);

    if (value == 1) {
		action.sem_num = 1; // second sm
		action.sem_op = 1; // up
		action.sem_flg = 0; // no flag
		result = semop(s->semid, &action, 1); 
		if (result == -1) {
            fprintf(stderr,"Value: %d Str: %s",errno,strerror(errno));
            perror("up of helper semaphore failed, system fault...\n"); 
            
			exit(43);
		}	
        printf("Detected lost up\n");
        return 0; // lost up
    }
    action.sem_num = 0; // only one semaphore
    action.sem_op = 1; // increase by one
    action.sem_flg = 0; // no flag
    
    result = semop(s->semid, &action, 1);    
    if (result == -1) {
        fprintf(stderr,"Value: %d Str: %s",errno,strerror(errno));
        perror("up failed, system fault...\n"); 
        exit(43);
    }

    action.sem_num = 1; // second sm
    action.sem_op = 1; // up
    action.sem_flg = 0; // no flag
    result = semop(s->semid, &action, 1); 
    if (result == -1) {
        fprintf(stderr,"Value: %d Str: %s",errno,strerror(errno));
        perror("up of helper semaphore failed, system fault...\n"); 
        exit(43);
    }
    
    return 1;
}

int mysem_destroy(mysem_t *s) {
    int res;
    res = semctl(s->semid, 0, IPC_RMID, 0);
    
    return(res ? -1 : 1);
}