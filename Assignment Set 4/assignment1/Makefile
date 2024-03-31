CC = gcc
CFLAGS = -Wall -g

dataTransfer: mycoroutines.c mycoroutines.h dataTransfer.c
	$(CC) $(CFLAGS) mycoroutines.c dataTransfer.c -o dataTransfer 

clean:
	rm -f *.o dataTransfer *.copy

test2:
	./dataTransfer input
	diff input input.copy

test1:
	./dataTransfer testinput.txt
	diff testinput.txt testinput.txt.copy
