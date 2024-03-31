CC = gcc
CFLAGS = -Wall -g -fsanitize=address -pthread
INPUT = input

all: rollercoaster.o monitor.o
	$(CC) $(CFLAGS) -o rollercoaster rollercoaster.o monitor.o

monitor.o: monitor.c monitor.h

test:
	./rollercoaster 5 <test.txt

clean:
	rm -f *.o rollercoaster	