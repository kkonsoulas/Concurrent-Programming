CC = gcc
CFLAGS = -Wall -g -fsanitize=address -pthread
INPUT = input

all: testProgram.o Pipeslib.o
	$(CC) $(CFLAGS) -o testProgram testProgram.o Pipeslib.o

test:
	./testProgram $(INPUT)
	diff $(INPUT) $(INPUT).copy2
	

clean:
	rm -f *.o testProgram $(INPUT).copy $(INPUT).copy2
