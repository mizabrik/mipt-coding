CC=g++
CFLAGS=-c -pedantic -Wall -Wextra -std=c++14 -O3

all: clean permutation-test

clean:
	rm -f *.o permutation-test

permutation-test: test.o
	$(CC) test.o -o permutation-test

test.o: test.cc
	$(CC) $(CFLAGS) test.cc
