CC = gcc
CFLAGS = -Wall -g 

all: readerswriters

readerswriters: list.o mycoroutines.o mythreads.o readerswriters.o
	$(CC) $(CFLAGS) $^ -o $@

list.o: ../assignment2/list.c ../assignment2/list.h
	$(CC) $(CFLAGS) -c $< -o $@

mycoroutines.o: ../assignment2/mycoroutines.c ../assignment2/mycoroutines.h
	$(CC) $(CFLAGS) -c $< -o $@

mythreads.o: ../assignment2/mythreads.c ../assignment2/mythreads.h
	$(CC) $(CFLAGS) -c $< -o $@

readerswriters.o: readerswriters.c
	$(CC) $(CFLAGS) -c $< -o $@

test:
	./readerswriters <test.txt

clean:
	rm -f *.o readerswriters