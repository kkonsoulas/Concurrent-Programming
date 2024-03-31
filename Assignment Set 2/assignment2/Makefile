CC = gcc
CFLAGS = -Wall -g -fsanitize=address -pthread
TESTOUT = testout

all: primeNumbers.o ../assignment1/mySemLib.o
	$(CC) $(CFLAGS) -o primeNumbers primeNumbers.o ../assignment1/mySemLib.o


clean:
	rm -f *.o primeNumbers $(TESTOUT)

testexam:
	./primeNumbers 2 <exam_test.txt

test1:
	echo "prime_numbers_small"
	time ./primeNumbers 1 <prime_numbers_small.txt 2>$(TESTOUT)
	time ./primeNumbers 2 <prime_numbers_small.txt 2>>$(TESTOUT) 
	time ./primeNumbers 3 <prime_numbers_small.txt 2>>$(TESTOUT) 
	time ./primeNumbers 4 <prime_numbers_small.txt 2>>$(TESTOUT) 
	time ./primeNumbers 5 <prime_numbers_small.txt 2>>$(TESTOUT) 
	time ./primeNumbers 6 <prime_numbers_small.txt 2>>$(TESTOUT) 
	time ./primeNumbers 7 <prime_numbers_small.txt 2>>$(TESTOUT) 
	time ./primeNumbers 8 <prime_numbers_small.txt 2>>$(TESTOUT) 
test2:
	echo "prime_numbers_large"
	time ./primeNumbers 1 <prime_numbers_large.txt 2>>$(TESTOUT) 
	time ./primeNumbers 2 <prime_numbers_large.txt 2>>$(TESTOUT) 
	time ./primeNumbers 3 <prime_numbers_large.txt 2>>$(TESTOUT) 
	time ./primeNumbers 4 <prime_numbers_large.txt 2>>$(TESTOUT) 
	time ./primeNumbers 5 <prime_numbers_large.txt 2>>$(TESTOUT) 
	time ./primeNumbers 6 <prime_numbers_large.txt 2>>$(TESTOUT) 
	time ./primeNumbers 7 <prime_numbers_large.txt 2>>$(TESTOUT) 
	time ./primeNumbers 8 <prime_numbers_large.txt 2>>$(TESTOUT) 
test3:
	echo "random_numbers_small"
	time ./primeNumbers 1 <random_numbers_small.txt 2>>$(TESTOUT) 
	time ./primeNumbers 2 <random_numbers_small.txt 2>>$(TESTOUT) 
	time ./primeNumbers 3 <random_numbers_small.txt 2>>$(TESTOUT) 
	time ./primeNumbers 4 <random_numbers_small.txt 2>>$(TESTOUT) 
	time ./primeNumbers 5 <random_numbers_small.txt 2>>$(TESTOUT) 
	time ./primeNumbers 6 <random_numbers_small.txt 2>>$(TESTOUT) 
	time ./primeNumbers 7 <random_numbers_small.txt 2>>$(TESTOUT) 
	time ./primeNumbers 8 <random_numbers_small.txt 2>>$(TESTOUT) 
	echo "random_numbers_large"
test4:
	time ./primeNumbers 1 <random_numbers_large.txt 2>>$(TESTOUT) 
	time ./primeNumbers 2 <random_numbers_large.txt 2>>$(TESTOUT) 
	time ./primeNumbers 3 <random_numbers_large.txt 2>>$(TESTOUT) 
	time ./primeNumbers 4 <random_numbers_large.txt 2>>$(TESTOUT) 
	time ./primeNumbers 5 <random_numbers_large.txt 2>>$(TESTOUT) 
	time ./primeNumbers 6 <random_numbers_large.txt 2>>$(TESTOUT) 
	time ./primeNumbers 7 <random_numbers_large.txt 2>>$(TESTOUT) 
	time ./primeNumbers 8 <random_numbers_large.txt 2>>$(TESTOUT) 
