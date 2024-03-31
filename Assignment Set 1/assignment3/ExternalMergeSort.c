#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#define BLOCK_SIZE 64 //those are ints
#define TEMP_FILE "TempFile"

struct thread_info {
    char *fileName;
    pthread_t thread;
    long unsigned start;
    long unsigned end;
    char finished;
};
typedef struct thread_info thread_info_t;

void insertionSort(int array[], int size) {
    int key, i, j;    
    
    for (i = 1; i < size; i++) {  
        key = array[i];  
        j = i - 1;  

        while (j >= 0 && array[j] > key) {  
            array[j + 1] = array[j];  
            j = j - 1;  
        }  
        array[j + 1] = key;  
    }
    
}

FILE *openFile(char* fileName, char *mode) {
    FILE* fd = fopen(fileName, mode); 
    if (fd == NULL) { 
        perror("Error while opening the file.\n"); 
        exit(43); 
    } 

    return fd;
}

int min (int x, int y) {
    return x < y ? x : y;
}

/* The function merges the two sorted parts 64 bytes at a time */
void mergeAndWrite(thread_info_t *input1, thread_info_t *input2) {
    //init

    long unsigned idx1 = 0, idx2 = 0, idxTemp = 0;
    long unsigned size1 = (input1->end - input1->start) + 1;
    long unsigned size2 = (input2->end - input2->start) + 1;
    long unsigned readSize, readSize1, readSize2, numsLeft;
    long unsigned numsLeft1 = size1;
    long unsigned numsLeft2 = size2;    

    int array1[BLOCK_SIZE];
    int array2[BLOCK_SIZE];
    int mergedArray[BLOCK_SIZE];
    
    FILE* fd1 = openFile(input1->fileName, "rb+");
    FILE* fd2 = openFile(input2->fileName, "rb+");
    FILE* fdRes = openFile(TEMP_FILE, "rb+");
    fseek(fdRes,input1->start * sizeof(int),SEEK_SET);

   
    readSize1 = min(numsLeft1, BLOCK_SIZE);
    fseek(fd1, input1->start * sizeof(int), SEEK_SET);
    fread(array1, sizeof(int), readSize1, fd1);


    readSize2 = min(numsLeft2, BLOCK_SIZE);
    fseek (fd2, input2->start * sizeof(int), SEEK_SET);
    fread(array2, sizeof(int), readSize2, fd2);

    // two array merge
    while (1) {
        //transfer ints into output buffer
        while(idx1 < readSize1 && idx2 < readSize2 && idxTemp < BLOCK_SIZE) {
            if (array1[idx1] <= array2[idx2]) {
                mergedArray[idxTemp] = array1[idx1];
                idx1++;
            } 
            else {
                mergedArray[idxTemp] = array2[idx2];
                idx2++;
            }
            idxTemp++;
        }

        //array1 is empty, refill it 
        if (idx1 == readSize1) {
            if (readSize1 < BLOCK_SIZE) {
                numsLeft1 -= readSize1;
                break;
            }
            idx1 = 0;
            numsLeft1 -= BLOCK_SIZE;
            readSize1 = min(numsLeft1, BLOCK_SIZE);
            fread(array1, sizeof(int), readSize1, fd1);
        }

        //array2 is empty, refill it 
        if (idx2 == readSize2) {
            if (readSize2 < BLOCK_SIZE) {
                numsLeft2 -= readSize2;
                break;
            }
            idx2 = 0;
            numsLeft2 -= BLOCK_SIZE;
            readSize2 = min(numsLeft2, BLOCK_SIZE);
            fread(array2, sizeof(int), readSize2, fd2);
        }

        //output array is full, write it
        if (idxTemp == BLOCK_SIZE) {
            fwrite(mergedArray, sizeof(int), BLOCK_SIZE, fdRes);
            idxTemp = 0;
        }
    }
    
    // Remaining elements in arr1, add them to the merged array
    while (numsLeft1 != 0) {
        if (idx1 == readSize1) {
            if (readSize1 < BLOCK_SIZE) {
                numsLeft1 -= readSize1;
                break;
            }
            idx1 = 0;
            numsLeft1 -= BLOCK_SIZE;
            readSize1 = min(numsLeft1, BLOCK_SIZE);
            fread(array1, sizeof(int), readSize1,fd1);
        }

        if (idxTemp == BLOCK_SIZE) {
            fwrite(mergedArray,  sizeof(int), BLOCK_SIZE, fdRes);
            idxTemp = 0;
        }
        mergedArray[idxTemp] = array1[idx1];
        idx1++;
        idxTemp++;
    }

    // Remaining elements in arr2, add them to the merged array
    while (numsLeft2 != 0) {
        if (idx2 == readSize2) {
            if (readSize2 < BLOCK_SIZE) {
                numsLeft2 -= readSize2;
                break;
            }
            idx2 = 0;
            numsLeft2 -= BLOCK_SIZE;
            readSize2 = min(numsLeft2, BLOCK_SIZE);
            fread(array2, sizeof(int), readSize2, fd2);
        } 
        if (idxTemp == BLOCK_SIZE) {
            fwrite(mergedArray, sizeof(int), BLOCK_SIZE, fdRes);
            idxTemp = 0;
        }
        mergedArray[idxTemp] = array2[idx2];
        idx2++;
        idxTemp++;
        
    }

    if (idxTemp != 0) {
        fwrite(mergedArray, sizeof(int), idxTemp, fdRes);
    }

    //copy back to input file
    idxTemp = 0;
    fseek(fdRes, input1->start * sizeof(int), SEEK_SET);
    fseek(fd1, input1->start *sizeof(int), SEEK_SET);
    numsLeft = size1 + size2;
    while (numsLeft != 0) {
        readSize = min(numsLeft, BLOCK_SIZE);
        fread(mergedArray, sizeof(int), readSize, fdRes);
        fwrite(mergedArray, sizeof(int), readSize, fd1);
        numsLeft = numsLeft - readSize;
    }
    fclose(fd1);
    fclose(fd2);
    fclose(fdRes);
    return;

}

//reads chunck, sorts it and flushes it back into file
void blockSort(long unsigned int start,long unsigned int end, char* filename) {
    //init
    FILE *fd = openFile(filename, "rb+");
    int array[BLOCK_SIZE];
    unsigned long size = (end - start) + 1; 
    //

    //read chunck
    fseek(fd,start*sizeof(int),SEEK_SET);
    fread(array, sizeof(int), size, fd);    
    //
    
    //sort
    insertionSort(array, (end - start) + 1); 

    //write sorted chunck back
    fseek(fd,start*sizeof(int),SEEK_SET);
    fwrite(array, sizeof(int) , size, fd);
    fclose(fd);
    //

    return;
}

void* externalMergeSort(void* args){
    thread_info_t* thread_info = (thread_info_t*) args;
    unsigned long size = (thread_info->end - thread_info->start) + 1;
    
    // If it is small enough sort it
    if (size <= BLOCK_SIZE) {
        blockSort(thread_info->start ,thread_info->end ,thread_info->fileName);
        thread_info->finished = 1;
        return NULL;
    }
    //

    //init
    pthread_t t1, t2;
    thread_info_t input1, input2;
    input1.fileName = thread_info->fileName;
    input2.fileName = thread_info->fileName;
    input1.finished = 0;
    input2.finished = 0;
    input1.start = thread_info->start;
    input1.end = thread_info->start + ((size/2)-1);
    input2.start = thread_info->start + (size/2);
    input2.end = thread_info->start + size - 1;
    //

    pthread_create(&t1, NULL, externalMergeSort, &input1);
    pthread_create(&t2, NULL, externalMergeSort, &input2);

    while(!input1.finished);
    while(!input2.finished);

    mergeAndWrite(&input1, &input2);
    thread_info->finished = 1;
    
    return NULL;
}


int main (int argc, char *argv[]) {
    pthread_t t0;
    thread_info_t input;

    //arguments check
    if (argc != 2) {
        printf ("Invalid number of arguments\n");
        return(-1);
    }
    //
    //create temp helper file
    FILE* temp = fopen(TEMP_FILE, "wb+");
    fclose(temp);

    //file size calc
    FILE* fd = openFile(argv[1],"rb+");
    fseek(fd, 0L, SEEK_END);
    input.end  = ftell(fd)/sizeof(int) - 1; //we know the file contains only integers
    input.start=0;
    fclose(fd);
    //
    
    input.finished = 0;
    input.fileName = argv[1];

    pthread_create(&t0, NULL, externalMergeSort, &input);
    while(!input.finished);

    if (remove(TEMP_FILE) != 0) {
        printf("Failed to delete tempfile\n");
    }

    return (0);
}