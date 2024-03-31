#include <stdio.h>
#include <pthread.h> 
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>

struct thread_info {
	unsigned long long potential_prime;
	pthread_t thread;
	char active;
	char finish;

}; 
typedef struct thread_info thread_info_t;

/* Checks if a number is prime */
void *check_prime_number(void *arg) {
	
	thread_info_t *thread_info = (thread_info_t*) arg;

	// Check if the value is prime
	while (thread_info->finish != 1) {
		//spin until is set to active by main thread
		while(thread_info->active == 0);

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
		
		if (is_prime) {
			printf("%lld: 1\n", thread_info->potential_prime);
		}
		else {
			printf("%lld: 0\n", thread_info->potential_prime);
		}

		/* Inform main thread that the work is complete*/
		thread_info->active = 0;
	}
		
    return(NULL);
} 


int main (int argc, char* argv[]) {
	int num_threads, i = 0, res;
    unsigned long long potential_prime;
	thread_info_t *thread_array;

	if (argc != 2) {
		printf("Invalid number of arguments\n");
		return 1;
	}
	
	// allocate thread info array
	num_threads = atoi(argv[1]);
	thread_array = (thread_info_t*) calloc(sizeof(thread_info_t), num_threads);
	if (thread_array == NULL) {
		perror ("Not enough memory\n");
		exit(42);
	}
	//
    
	//create N workers
	for(int i=0 ;i < num_threads ;i++){
		thread_array[i].active = 0;
		thread_array[i].finish = 0;
		pthread_create(&thread_array[i].thread, NULL, check_prime_number, &thread_array[i]);
	}
	
	//start counting time
	struct timeval start, end;
    gettimeofday(&start, NULL);
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
		i = (i+1)%num_threads;
	}

	//Notify workers to finish
	for (i = 0; i < num_threads; i++) {
		while(thread_array[i].active == 1);
		thread_array->finish = 1;
	}


	gettimeofday(&end, NULL);
    double time_spent = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
    printf("find prime numbers took %lf seconds to execute\n", time_spent);
	fprintf(stderr,"find prime numbers took %lf seconds to execute with %d threads\n", time_spent, num_threads);
	return(0);
}







