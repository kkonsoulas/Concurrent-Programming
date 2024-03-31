#include "Pipeslib.h"

pipe_buffer_t arrayOfPipes[SIZEARRAY] = {{'\0'}};


int pipe_open(int size) {
    
    //Invalid Size Check
    if(size <= 1) {
        fprintf(stderr,"Invalid Pipe Size\n");
        return -1;
    }
    
    int i;
    for (i = 0; i < SIZEARRAY; i++) {
        if (arrayOfPipes[i].pipe == NULL) {
            arrayOfPipes[i].pipe = (char *) malloc(size*sizeof(char));
            if (arrayOfPipes[i].pipe == NULL) {
                return -1;
            }
            arrayOfPipes[i].size = size;
            arrayOfPipes[i].idx_r = 0;
            arrayOfPipes[i].idx_w = 0;
            arrayOfPipes[i].write_done = 0;
            return i;
        }
    }

    return -1;
}

int pipe_write(int p, char c) {
    //Check if pipe is open
    if(arrayOfPipes[p].pipe == NULL) {
        return -1;
    }
    //

    //spin till pipe gets empty
    while((arrayOfPipes[p].idx_w + 1)%arrayOfPipes[p].size == arrayOfPipes[p].idx_r);

    arrayOfPipes[p].pipe[(arrayOfPipes[p].idx_w)] = c; //assign byte
    arrayOfPipes[p].idx_w = (arrayOfPipes[p].idx_w + 1)%arrayOfPipes[p].size; //move write index 1 byte forward
    
    return 1;
}

int pipe_writeDone(int p) {
    if(arrayOfPipes[p].pipe == NULL) {
        return -1;
    }

    arrayOfPipes[p].write_done = 1;

    return 1;
    
}

int pipe_read(int p, char *c) {
    if(arrayOfPipes[p].pipe == NULL) {
        return -1;
    }


    while(arrayOfPipes[p].idx_r == arrayOfPipes[p].idx_w) {
        if (arrayOfPipes[p].write_done == 1) {
            free(arrayOfPipes[p].pipe);
            arrayOfPipes[p].pipe = NULL;
            return 0;
        }
    };

    *c = arrayOfPipes[p].pipe[(arrayOfPipes[p].idx_r)] ;
    arrayOfPipes[p].idx_r = (arrayOfPipes[p].idx_r + 1)%arrayOfPipes[p].size;

    return 1;
    
}

void clear_pipes() {
    int i;
    for(i=0 ; i < SIZEARRAY ; i++){
        if (arrayOfPipes[i].pipe != NULL) {
            free(arrayOfPipes[i].pipe);
            arrayOfPipes[i].pipe = NULL;
        }
    }
}