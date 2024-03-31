#include "Pipeslib.h"
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>

struct threadInput {
	char *filename;
	int *p;
	char pull;
	char bothPhasesDone;
};
typedef struct threadInput threadInput_t;

//read from pipe
void pipePull(int p, FILE *fd) {
    char c;
    int res;

	//read from pipe, write to file
	while (1) {
    	res = pipe_read(p,&c);
        if (res == 0) {
			fclose(fd);
            break;
        }
		fputc(c,fd);
    }

	return;
}

//write to pipe
void pipePush(int p, FILE *fd) {
	char c;

	//read from file, write to pipe
	while(1){
		c = fgetc(fd);
		if(feof(fd)){
			break;
		}
		pipe_write(p,c);
	}
	//

    pipe_writeDone(p);
	fclose(fd);
	return;
}

void *transferThroughPipe(void *arg) {

	threadInput_t *input = (threadInput_t*)arg;
	FILE *fd;
	//push phase1
	if (input->pull == 1) {
		//pocess 2
		strcat(input->filename,".copy");
		fd = fopen(input->filename, "w+");
		if (fd == NULL) {
			perror("Error opening the file");
			exit(43);
		}
		
		pipePull(input->p[0], fd);
		fd = fopen(input->filename, "r");
		if (fd == NULL) {
			perror("Error opening the file");
			exit(43);
		}
		pipePush(input->p[1],fd);
		return NULL;
	}
	else {
		//process 1
		fd = fopen(input->filename, "r");
		if (fd == NULL) {
			perror("Error opening the file");
			exit(43);
		}
		pipePush(input->p[0], fd); //pipe1
		strcat(input->filename,".copy2");
		fd = fopen(input->filename, "w");
		if (fd == NULL) {
			perror("Error opening the file");
			exit(43);
		}
		pipePull(input->p[1], fd); //pipe2
		input->bothPhasesDone = 1;
		return NULL;
	}

	return NULL;
}



int main(int argc, char *argv[]) {
	int pipes[2];
    int res;
    pthread_t t1,t2;
	threadInput_t input1;
	threadInput_t input2;

	if (argc != 2) {
		printf("Invalid number of arguments\n");
		return -1;
	}

	input1.filename = calloc(sizeof(char), strlen(argv[1]) + 7);
	input2.filename = calloc(sizeof(char), strlen(argv[1]) + 7);
	strcpy(input1.filename, argv[1]);
	strcpy(input2.filename, argv[1]);
	
	//init the inputs
	pipes[0] = pipe_open(SIZEBUFFER);
	pipes[1] = pipe_open(SIZEBUFFER);
	input1.p = pipes;
	input2.p = pipes;

	input1.pull = 0;
	input2.pull = 1;

	input1.bothPhasesDone = 0 ;
	input2.bothPhasesDone = 0;


    res = pthread_create(&t1, NULL , transferThroughPipe, &input1);
	if (res) {
		exit(-1);
	}

	res = pthread_create(&t2, NULL , transferThroughPipe, &input2);
	if (res) {
		exit(-1);
	}
	//

	// wait for threads
	while (input1.bothPhasesDone == 0);
	free(input1.filename);
	free(input2.filename);


	return 0;
}