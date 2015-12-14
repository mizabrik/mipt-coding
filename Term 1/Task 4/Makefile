CC=g++
CFLAGS=-c -Wall -std=c++14 -ggdb

all: clean heaps-test

run: heaps-test
	./heaps-test

clean:
	rm -f *.o heaps-test

debug: heaps
	gdb heaps-test

heaps-test: test.o
	$(CC) -ggdb test.o -o heaps-test

test.o: test.cc
	$(CC) $(CFLAGS) test.cc
