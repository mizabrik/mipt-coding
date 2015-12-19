CC=g++
CFLAGS=-c -Wall -std=c++14 -ggdb

all: clean height

clean:
	rm -f *.o height

height: height.o treap.o naive.o
	$(CC) -ggdb height.o treap.o naive.o -o height

height.o: height.cc
	$(CC) $(CFLAGS) height.cc

treap.o: treap.cc
	$(CC) $(CFLAGS) treap.cc

naive.o: naive.cc
	$(CC) $(CFLAGS) naive.cc
