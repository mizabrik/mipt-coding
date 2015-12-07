CC=g++
CFLAGS=-c -Wall -std=c++11

all: clean deque-test

run: deque-test
	./deque-test

clean:
	rm -f *.o deque-test

debug: deque
	gdb deque-test

deque-test: test.o
	$(CC) -ggdb test.o -o deque-test

test.o: test.cc
	$(CC) $(CFLAGS) -ggdb test.cc
