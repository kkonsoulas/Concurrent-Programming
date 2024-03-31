CC = gcc
CFLAGS = -Wall -g 

all: test testloops

testloops: list.o mycoroutines.o mythreads.o testloops.o
	$(CC) $(CFLAGS) $^ -o $@

test: list.o mycoroutines.o mythreads.o test.o
	$(CC) $(CFLAGS) $^ -o $@

list.o: list.c list.h
	$(CC) $(CFLAGS) -c $< -o $@

mycoroutines.o: mycoroutines.c mycoroutines.h
	$(CC) $(CFLAGS) -c $< -o $@

mythreads.o: mythreads.c mythreads.h
	$(CC) $(CFLAGS) -c $< -o $@

test.o: test.c
	$(CC) $(CFLAGS) -c $< -o $@

testloops.o: testloops.c
	$(CC) $(CFLAGS) -c $< -o $@

test1: 
	./test

test2:
	./testloops


clean:
	rm -f *.o test testloops