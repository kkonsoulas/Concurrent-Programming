CC = gcc
CFLAGS = -Wall -g -fsanitize=address -pthread
INPUT = input

all: criticalSectionTest.o initTest.o lostUp.o mySemLib.o monitor.o
	$(CC) $(CFLAGS) -o criticalSectionTest criticalSectionTest.o mySemLib.o monitor.o
	$(CC) $(CFLAGS) -o lostUp lostUp.o mySemLib.o monitor.o
	$(CC) $(CFLAGS) -o initTest initTest.o mySemLib.o monitor.o

monitor.o: ../monitor.h ../monitor.c
	$(CC) $(CFLAGS) -c ../monitor.c -o monitor.o

test1:
	echo "Critical Section Test"
	./criticalSectionTest

test2:
	echo "Lost Up Test"
	./lostUp

test3:
	echo "Init Test"
	./initTest

clean:
	rm -f *.o criticalSectionTest initTest lostUp
