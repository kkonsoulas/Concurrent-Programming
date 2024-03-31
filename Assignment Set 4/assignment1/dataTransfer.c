#include <stdio.h>
#include "mycoroutines.h"
#include <string.h>
#include <stdlib.h>

#define BUFFERSIZE 64

struct dataTransfer {
    char buffer[BUFFERSIZE];
    co_t main_coroutine;
    co_t producer_coroutine;
    co_t consumer_coroutine;
    char *fileName;
    unsigned bytesWritten;
    char writeDone;
};
typedef struct dataTransfer dataTransfer_t;

void producer_coroutine(void *arg) {
    printf("Producer starting\n");
    dataTransfer_t *data = (dataTransfer_t*)arg;

    FILE *fd;
    fd = fopen(data->fileName, "r");
    int i = 0;

    while (1) {
        data->bytesWritten = 0;
        for (i = 0; i < BUFFERSIZE; i++) {
            data->buffer[i] = fgetc(fd);
            if (feof(fd)) {
                data->writeDone = 1;
                break;
            }
        }
        data->bytesWritten = i;

        mycoroutines_init(&data->producer_coroutine);
        printf("Producer filled the buffer switching to consumer\n");
        mycoroutines_switchto(&data->consumer_coroutine);

        if (data->writeDone == 1) {
            break;
        }
    }
    fclose(fd);
}

void consumer_coroutine(void *arg) {
    dataTransfer_t *data = (dataTransfer_t*)arg;

    printf("Consumer starting\n");
    int i = 0;
    char c;
    FILE *fd;
    int size = strlen(data->fileName) + strlen(".copy");
    char *fileCopy;
    fileCopy = calloc(size + 1, sizeof(char));  
    strcpy(fileCopy, data->fileName);
    strcat(fileCopy, ".copy");

    fd = fopen(fileCopy, "w+");
    if (fd == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    while (1) {
        for (i = 0; i < data->bytesWritten; i++) {
            c = data->buffer[i];
            fputc(c, fd);
        }


        mycoroutines_init(&data->consumer_coroutine);
        if (data->writeDone == 1) {
            printf("Consumer emptied the buffer but no more to copy switching to main\n");
            mycoroutines_switchto(&data->main_coroutine);
        } else {
            printf("Consumer emptied the buffer switching to producer\n");
            mycoroutines_switchto(&data->producer_coroutine);
        }
    }
    fclose(fd);

    if (fileCopy != NULL) {
        free(fileCopy);
        fileCopy = NULL;
    }
}


int main(int argc, char *argv[]) {
    dataTransfer_t arguments;

    if (argc < 2) {
        printf("Invalid number of arguments\n");
        return 1;
    }

    arguments.fileName = argv[1];

    printf("Creating coroutines\n");
    mycoroutines_init(&arguments.main_coroutine);
    mycoroutines_create(&arguments.producer_coroutine, producer_coroutine, &arguments);
    mycoroutines_create(&arguments.consumer_coroutine, consumer_coroutine, &arguments);


    printf("Switching from main to producer to start the process\n");
    mycoroutines_switchto(&arguments.producer_coroutine);
    printf("Came back to main about to destroy everything\n");
    
    mycoroutines_destroy(&arguments.producer_coroutine);
    mycoroutines_destroy(&arguments.consumer_coroutine);
    

    return 0;
}