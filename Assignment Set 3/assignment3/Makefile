CC = gcc
CFLAGS = -Wall -g -fsanitize=address -pthread
INPUT = input

all: narrowBridge.o monitor.o
	$(CC) $(CFLAGS) -o narrowBridge narrowBridge.o monitor.o

monitor.o: monitor.h monitor.c

test1:
	echo "Running test 1, only red cars"
	./narrowBridge 5 5 <onlyRedCars.txt

test2:
	echo "Running test 2, red blue cars"
	./narrowBridge 5 1 1 <redBlue.txt

test3:
	echo "Running test 3, blue red cars contest fairness 1"
	./narrowBridge 5 1 1 <blueRedContest.txt

test4:
	echo "Running test 4, blue red cars contest fairness 2"
	./narrowBridge 5 1 2 <blueRedContest.txt
test5:
	echo "Running test 5, waiting for the other side"
	./narrowBridge 5 3 <waitingForTheOtherSide.txt
test6:
	echo "Running test 6, traffic"
	./narrowBridge 5 1 2 <traffic.txt

clean:
	rm -f *.o narrowBridge	