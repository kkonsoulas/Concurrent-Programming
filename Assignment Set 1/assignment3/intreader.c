#include <stdio.h>

#define MAX_INTS 10000 // Adjust this based on the maximum number of integers in the file

int main(int argc,char* argv[]) {
    FILE *file = fopen(argv[1], "rb");

    if (file == NULL) {
        fprintf(stderr, "Error opening file for reading.\n");
        return 1;
    }

    int integers[MAX_INTS];
    size_t numIntsRead = fread(integers, sizeof(int), MAX_INTS, file);

    fclose(file);

    if (numIntsRead == 0) {
        fprintf(stderr, "Error reading integers from file.\n");
        return 1;
    }

    printf("Read %zu integers from the file:\n", numIntsRead);

    printf("\n");
    for (size_t i = 0; i < numIntsRead; ++i) {
        printf("%d ", integers[i]);
    }
    printf("\n");

    return 0;
}
