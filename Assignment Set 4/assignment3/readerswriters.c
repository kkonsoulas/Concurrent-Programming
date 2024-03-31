#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../assignment2/mythreads.h"
#define BUFFERSIZE 256
#define MAX_SIZE 1000

struct args {
    sem_t *mutex;
    sem_t *writeQ;
    sem_t *readerQ;
    int *num_writers_wait;
    int *num_readers_wait;
    int *num_writers_cs; 
    int *num_readers_cs;
    char* buffer;
};
typedef struct args args_t;


void myread(args_t *data){
    mythreads_yield();
    printf("Thread %u: %s",mythreads_self(), data->buffer);
    return;
}

void mywrite(args_t *data){
    mythreads_yield();
    printf("Writing in buffer\n");
    sprintf(data->buffer,"This is a sentence written by Thread %u\n",mythreads_self());

    return;
}

void writer(void *args) {
    printf("Writer %u arrived\n", mythreads_self());
    args_t *data = (args_t*) args;
    
    mythreads_sem_down(data->mutex);
    if (*data->num_readers_cs + *data->num_writers_cs > 0) {
        *data->num_writers_wait += 1;
        mythreads_sem_up(data->mutex);
       
        mythreads_sem_down(data->writeQ);
    }
    else {
        *data->num_writers_cs += 1;
        mythreads_sem_up(data->mutex);
    }
    
    /* Write */
    mywrite(data);
    
    mythreads_sem_down(data->mutex);
    *data->num_writers_cs -= 1;
    if(*data->num_readers_wait > 0) {
        *data->num_readers_wait -= 1;
        *data->num_readers_cs += 1;
        mythreads_sem_up(data->readerQ);
    }
    else {
        if (*data->num_writers_wait > 0) {
            *data->num_writers_wait -= 1;
            *data->num_readers_cs += 1;
            mythreads_sem_up(data->writeQ);
        }
        mythreads_sem_up(data->mutex);
    }
    printf("Writer end\n");
}

void reader(void *args){

    args_t *data = (args_t*) args;
    printf("Reader %u arrived\n", mythreads_self());
    
    mythreads_sem_down(data->mutex);
    if(*data->num_writers_cs + *data->num_writers_wait > 0) {
        *data->num_readers_wait += 1;
        mythreads_sem_up(data->mutex);
        mythreads_sem_down(data->readerQ);
        
        if (*data->num_readers_wait > 0) {
            *data->num_readers_wait -= 1;
            *data->num_readers_cs += 1;
            mythreads_sem_up(data->readerQ);
        }
        else {
            mythreads_sem_up(data->mutex);
        }
    }
    else {
       *data->num_readers_cs += 1; 
       mythreads_sem_up(data->mutex);
    }
    
    /* Read */
    myread(data);
    
    mythreads_sem_down(data->mutex);
    *data->num_readers_cs -= 1;

    if (*data->num_readers_cs == 0 && *data->num_writers_wait > 0) {
        *data->num_writers_wait -= 1;
        *data->num_writers_cs += 1;
        mythreads_sem_up(data->writeQ);
    }
    mythreads_sem_up(data->mutex);
}

int main (int argc, char *argv[]) {
    args_t *data;
    data = malloc(sizeof(args_t));
    sem_t mutex;
    sem_t writeQ;
    sem_t readerQ;
    int num_writers_wait = 0;
    int num_readers_wait = 0;
    int num_writers_cs = 0; 
    int num_readers_cs = 0;
    char c;
    thr_t thread_array[MAX_SIZE];
    unsigned size = 0;
    int num;
    mythreads_init();
    
    
    data->mutex = &mutex;
    data->writeQ = &writeQ;
    data->readerQ = &readerQ;
    data->num_writers_wait = &num_writers_wait;
    data->num_readers_wait = &num_readers_wait;
    data->num_writers_cs = &num_writers_cs;
    data->num_readers_cs = &num_readers_cs;
    data->buffer = malloc(BUFFERSIZE*sizeof(char));
    strcpy(data->buffer, "Noone has written anything yet\n");

    mythreads_sem_create(data->mutex, 1);
    mythreads_sem_create(data->writeQ, 0);
    mythreads_sem_create(data->readerQ, 0);

    do{
        scanf (" %c", &c);
        switch (c)  {
            case 'r':{
                scanf(" %d", &num);
                printf("Add %d readers\n", num);
                for (int i = 0; i < num; i++) {

                    mythreads_create(&thread_array[size], reader, data);
                    size++;
                    if (size > MAX_SIZE) {
                        break;
                    }
                }
                break;
            }
            case 'w': {
                scanf ("%d", &num);
                printf("Add %d writers\n", num);
                for (int i = 0; i < num; i++) {

                    mythreads_create(&thread_array[size], writer, data);
                    size++;
                    if (size > MAX_SIZE) {
                        break;
                    }
                }
                break;
            }
            case 'q': {
                break;
            }
            default: {
                printf("Wrong Command\n");
                break;
            }   
        }

        if (size > MAX_SIZE) {
            break;
        }
    }while(c != 'q');

    printf("Joining Threads\n");
    for (int i = 0; i < size; i++) {
        mythreads_join(&thread_array[i]);
    }
    
    printf("Exiting Readers Writers\n");
    free(data->buffer);
    mythreads_sem_destroy(data->mutex);
    mythreads_sem_destroy(data->readerQ);
    mythreads_sem_destroy(data->writeQ);
    free(data);
    return 0;
}