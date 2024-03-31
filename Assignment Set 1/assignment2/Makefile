CC = gcc
CFLAGS = -Wall -g -fsanitize=address -pthread
TESTOUT = testout

all: testProgram
	$(CC) $(CFLAGS) -o testProgram testProgram.c


clean:
	rm -f *.o testProgram $(TESTOUT)

test:
	./testProgram 1 <prime_numbers_small.txt 2>$(TESTOUT)
	./testProgram 2 <prime_numbers_small.txt 2>>$(TESTOUT)
	./testProgram 3 <prime_numbers_small.txt 2>>$(TESTOUT)
	./testProgram 4 <prime_numbers_small.txt 2>>$(TESTOUT)
	./testProgram 5 <prime_numbers_small.txt 2>>$(TESTOUT)
	./testProgram 6 <prime_numbers_small.txt 2>>$(TESTOUT)
	./testProgram 7 <prime_numbers_small.txt 2>>$(TESTOUT)
	./testProgram 8 <prime_numbers_small.txt 2>>$(TESTOUT)
	./testProgram 1 <prime_numbers_large.txt 2>>$(TESTOUT)
	./testProgram 2 <prime_numbers_large.txt 2>>$(TESTOUT)
	./testProgram 3 <prime_numbers_large.txt 2>>$(TESTOUT)
	./testProgram 4 <prime_numbers_large.txt 2>>$(TESTOUT)
	./testProgram 5 <prime_numbers_large.txt 2>>$(TESTOUT)
	./testProgram 6 <prime_numbers_large.txt 2>>$(TESTOUT)
	./testProgram 7 <prime_numbers_large.txt 2>>$(TESTOUT)
	./testProgram 8 <prime_numbers_large.txt 2>>$(TESTOUT)
	./testProgram 1 <random_numbers_small.txt 2>>$(TESTOUT)
	./testProgram 2 <random_numbers_small.txt 2>>$(TESTOUT)
	./testProgram 3 <random_numbers_small.txt 2>>$(TESTOUT)
	./testProgram 4 <random_numbers_small.txt 2>>$(TESTOUT)
	./testProgram 5 <random_numbers_small.txt 2>>$(TESTOUT)
	./testProgram 6 <random_numbers_small.txt 2>>$(TESTOUT)
	./testProgram 7 <random_numbers_small.txt 2>>$(TESTOUT)
	./testProgram 8 <random_numbers_small.txt 2>>$(TESTOUT)
	./testProgram 1 <random_numbers_large.txt 2>>$(TESTOUT)
	./testProgram 2 <random_numbers_large.txt 2>>$(TESTOUT)
	./testProgram 3 <random_numbers_large.txt 2>>$(TESTOUT)
	./testProgram 4 <random_numbers_large.txt 2>>$(TESTOUT)
	./testProgram 5 <random_numbers_large.txt 2>>$(TESTOUT)
	./testProgram 6 <random_numbers_large.txt 2>>$(TESTOUT)
	./testProgram 7 <random_numbers_large.txt 2>>$(TESTOUT)
	./testProgram 8 <random_numbers_large.txt 2>>$(TESTOUT)
