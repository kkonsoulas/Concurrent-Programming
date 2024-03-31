CC = gcc
CFLAGS = -Wall -g -fsanitize=address -pthread
INPUT = input

all: rollercoaster.o mySemLib.o
	$(CC) $(CFLAGS) -o rollercoaster rollercoaster.o mySemLib.o

mySemLib.o: ../assignment1/mySemLib.c ../assignment1/mySemLib.h
	$(CC) $(CFLAGS) -c ../assignment1/mySemLib.c -o mySemLib.o
test:
	./rollercoaster 5 <test.txt

clean:
	rm -f *.o rollercoaster	