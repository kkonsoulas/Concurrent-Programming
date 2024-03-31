#include <stdio.h>
#include <pthread.h> 
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include "../assignment1/mySemLib.h"

struct thread_info {
	unsigned long long potential_prime;
	pthread_t thread;
	char active;
	mysem_t activeSem;
	mysem_t *availableWorkersSem;
	mysem_t *bossSem;
	unsigned *availableWorkers;
	unsigned maxWorkers;
	unsigned *finishedWorkers;
}; 
typedef struct thread_info thread_info_t;

/* Checks if a number is prime */
void *check_prime_number(void *arg) {
	
	thread_info_t *thread_info = (thread_info_t*) arg;

	while (1) {
		mysem_down(thread_info->availableWorkersSem);
		(*thread_info->availableWorkers)++;
		//available workers will have a max of N-1 threads
		if (*thread_info->availableWorkers == 0) {
			printf("Wake up Boss %ld\n", pthread_self());
			mysem_up(thread_info->bossSem);
		}
		mysem_up(thread_info->availableWorkersSem);

		//block until master provides work
		mysem_down(&thread_info->activeSem);
		if (thread_info->active == 0) {
			break;
		}
        printf("Got more work to do %ld\n", pthread_self());

		// Check if the value is prime algorithm
		int is_prime = 1;
		if (thread_info->potential_prime <= 1) {
			is_prime = 0;
			printf("%lld: 0\n", thread_info->potential_prime);
		} 
		else {
			for (int i = 2; i * i <= thread_info->potential_prime; i++) {
				if (thread_info->potential_prime % i == 0) {
					is_prime = 0;
					break;
				}
			}
		}
		printf("%lld: %d\n",thread_info->potential_prime,is_prime);
		//

		/* Inform main thread that the work is complete*/
		thread_info->active = 0;
	}

	mysem_down(thread_info->availableWorkersSem);
	(*thread_info->finishedWorkers)++;

	printf("No more numbers to calculate exiting %ld\n", pthread_self());

	if(*thread_info->finishedWorkers == thread_info->maxWorkers){ //if you are the last worker running notify main to end
		printf("I am the last to finish, wake up boss to end the program %ld\n", pthread_self());
		mysem_up(thread_info->availableWorkersSem);
		mysem_up(thread_info->bossSem);
	}
	else{
		mysem_up(thread_info->availableWorkersSem);

	}

    return(NULL);
} 


int main (int argc, char* argv[]) {
	unsigned num_threads ,availableWorkers, finishedWorkers = 0;
	int  i = 0, res;
    unsigned long long potential_prime;
	mysem_t availableWorkersSem, bossSem;

	if (argc != 2) {
		printf("Invalid number of arguments\n");
		return 1;
	}
	
	num_threads = atoi(argv[1]);
	thread_info_t thread_array[num_threads];

	//init available workers semaphore
	availableWorkers = 0;
	availableWorkersSem.initialized='F';
	bossSem.initialized='F';
    mysem_init(&availableWorkersSem,1);
	mysem_init(&bossSem,0);

	//

	//create & init N workers
	for(int i=0 ;i < num_threads ;i++){
		thread_array[i].active = 0;
		thread_array[i].activeSem.initialized = 'F';
		mysem_init(&thread_array[i].activeSem,0);
		thread_array[i].availableWorkers = &availableWorkers;
		thread_array[i].availableWorkersSem = &availableWorkersSem;
		thread_array[i].bossSem = &bossSem;
		thread_array[i].finishedWorkers = &finishedWorkers;
		thread_array[i].maxWorkers = num_threads; 
		pthread_create(&thread_array[i].thread, NULL, check_prime_number, &thread_array[i]);
	}
	
	//job scheduling
	while (1) {
		mysem_down(&availableWorkersSem);
		availableWorkers--;
		if (availableWorkers == -1) {
			mysem_up(&availableWorkersSem);
			printf("No more work to assign, boss going to sleep %ld\n", pthread_self());
			mysem_down(&bossSem);
			printf("Boss woke up %ld\n", pthread_self());
		}
		else{
			mysem_up(&availableWorkersSem);
		}
		
		
		//find next worker and assign work
		while(1) {
			if (thread_array[i].active == 1) {
				i = (i+1)%num_threads;
				continue; //thread unavailable
			}
			
			res = scanf("%lld", &potential_prime);
			if (res == EOF) {
				break;
			}
			thread_array[i].potential_prime = potential_prime;
			thread_array[i].active = 1;
			printf("Wake up the worker\n");
			mysem_up(&thread_array[i].activeSem);
			i = (i+1)%num_threads;
			break;
		}

		if (res == EOF) {
			break;
		}
		
	}
	

	//Notify workers to finish
	for (i = 0; i < num_threads; i++) {
		mysem_up(&thread_array[i].activeSem);
	}
	printf("Boss waiting for everyone to finish\n");
	mysem_down(&bossSem);
	printf("Everyone finished ending the program\n");
	
	for(i = 0; i < num_threads; i++){
		mysem_destroy(&thread_array[i].activeSem);
	}

	mysem_destroy(&bossSem);
	mysem_destroy(&availableWorkersSem);
	
	return(0);
}







