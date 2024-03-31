CC = gcc
CFLAGS = -Wall -g -fsanitize=address -pthread
INPUT = random_integers.bin

all:
	$(CC) $(CFLAGS) -o externalmergesort ExternalMergeSort.c
	$(CC) $(CFLAGS) -o intcreator intcreator.c
	$(CC) $(CFLAGS) -o isSorted isSorted.c
	$(CC) $(CFLAGS) -o intreader intreader.c

test:
	./intcreator 10000
	./externalmergesort $(INPUT)
	./isSorted $(INPUT)


clean:
	rm -f *.o externalmergesort intcreator isSorted intreader
