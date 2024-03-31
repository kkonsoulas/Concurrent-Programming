#ifndef PIPESLIB_H
#define PIPESLIB_H
#include <stdio.h>
#include <stdlib.h>
#define SIZEARRAY 10 
#define SIZEBUFFER 64

struct pipe_buffer {
    char *pipe;
    int size;
    int idx_r;
    int idx_w;
    char write_done;

};
typedef struct pipe_buffer pipe_buffer_t;

int pipe_open(int size);

int pipe_write(int p, char c);

int pipe_writeDone(int p);

int pipe_read(int p, char *c);

void clear_pipes();

#endif 

