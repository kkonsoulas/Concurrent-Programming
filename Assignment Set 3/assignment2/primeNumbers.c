#include <stdio.h>
#include <pthread.h> 
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include "monitor.h"

#define NOT_FULL 0
#define NOT_EMPTY 1

struct thread_info {
	unsigned long long potential_prime;
	pthread_t thread;
	char active;
	monitor_t *monitor;
	int *full;
	unsigned maxWorkers;
	int *free;
	int *posStorage;
	unsigned long long *storage;
}; 
typedef struct thread_info thread_info_t;

/* Checks if a number is prime */
void *check_prime_number(void *arg) {
	
	thread_info_t *thread_info = (thread_info_t*) arg;
	unsigned long long potential_prime;

	while (1) {
		monitor_lock (thread_info->monitor);
		(*thread_info->full)--;
		if (*thread_info->full < 0 && thread_info->active == 1) {
        	printf("No more work to do going to sleep %ld\n", pthread_self());
			monitor_wait(thread_info->monitor,NOT_EMPTY);
		}
		if (thread_info->active == 0 && *thread_info->posStorage == 0) {
			printf("Time to die %ld\n", pthread_self());
			monitor_unlock(thread_info->monitor);
			return NULL;
		}
        printf("Got more work to do %ld\n", pthread_self());


		//get element
		(*thread_info->posStorage)--;
		potential_prime = thread_info->storage[*thread_info->posStorage];
		monitor_unlock(thread_info->monitor);


		// Check if the value is prime algorithm
		int is_prime = 1;
		
		if (potential_prime <= 1) {
			is_prime = 0;
		} 
		else {
			for (int i = 2; i * i <= potential_prime; i++) {
				if (potential_prime % i == 0) {
					is_prime = 0;
					break;
				}
			}
		}
		printf("%lld: %d\n", potential_prime,is_prime);


		monitor_lock (thread_info->monitor);
		(*thread_info->free)++;
		if (*thread_info->free <=0) {
        	printf("Done waking up boss %ld\n", pthread_self());
			monitor_notify(thread_info->monitor, NOT_FULL);
		}
		monitor_unlock(thread_info->monitor);

	}



    return(NULL);
} 


int main (int argc, char* argv[]) {
	unsigned num_threads;
	int res;
    unsigned long long potential_prime;
	monitor_t monitor;
	int posStorage = 0;

	monitor_init(&monitor, 2);
	

	if (argc != 2) {
		printf("Invalid number of arguments\n");
		return 1;
	}
	
	num_threads = atoi(argv[1]);
	thread_info_t thread_array[num_threads];

	unsigned long long storage[num_threads];
	int free = atoi(argv[1]);
	int full = 0;


	//create & init N workers
	for(int i=0 ;i < num_threads ;i++){
		thread_array[i].active = 1;
		thread_array[i].free = &free;
		thread_array[i].monitor = &monitor;
		thread_array[i].full = &full;
		thread_array[i].maxWorkers = num_threads; 
		thread_array[i].storage = storage;
		thread_array[i].posStorage = &posStorage;
		pthread_create(&thread_array[i].thread, NULL, check_prime_number, &thread_array[i]);
	}

	//job scheduling
	while (1) {
		monitor_lock(&monitor);
		free--;
		if (free < 0) {
			printf("No more work to assign, boss going to sleep %ld\n", pthread_self());
			monitor_wait(&monitor, NOT_FULL); //wait at NOT_FULL queue
			printf("Boss woke up %ld\n", pthread_self());
		}

		//place in storage
		res = scanf("%lld", &potential_prime);
		if (res == EOF) {
			monitor_unlock(&monitor);
			break;
		}
		
		storage[posStorage] = potential_prime;

		posStorage++;
		full++;
		if (full >= 0) {
			monitor_notify(&monitor, NOT_EMPTY);
		}
		monitor_unlock(&monitor);
		
	}
	printf("No more numbers to calculate waiting for all workers to finish\n");
	for (int i =0; i < num_threads; i++) {
		thread_array[i].active = 0;
	}
	monitor_lock(&monitor);
	for (int i =0; i < num_threads; i++) {
		monitor_notify(&monitor, NOT_EMPTY);
	}
	monitor_unlock(&monitor);
	for (int i =0; i < num_threads; i++) {
		pthread_join(thread_array[i].thread, NULL);
	}
	
	monitor_destroy(&monitor);
	
	return(0);
}
