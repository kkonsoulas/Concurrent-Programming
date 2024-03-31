#include <stdio.h>
#include <stdlib.h>
#include <time.h>

 // You can change this to the desired number of random integers

int main(int argc, char* argv[]) {
    // Seed the random number generator
    srand(time(NULL));

    // Open a binary file for writing
    FILE *file = fopen("random_integers.bin", "wb");

    // Check if the file was opened successfully
    if (file == NULL) {
        fprintf(stderr, "Error opening file for writing.\n");
        return 1;
    }

    // Generate random integers and write them to the file
    for (int i = 0; i < atoi(argv[1]); ++i) {
        int randomInt = rand()%10000;
        fwrite(&randomInt, sizeof(int), 1, file);
    }

    // Close the file
    fclose(file);

    printf("Random integers have been written to random_integers.bin\n");

    return 0;
}
