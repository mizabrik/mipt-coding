CC=g++
CFLAGS=-c -pedantic -Wall -Wextra -std=c++14 -ggdb

all: clean height avl-stats

clean:
	rm -f *.o height avl-stats

height: height.o treap.o naive.o
	$(CC) -ggdb height.o treap.o naive.o -o height

avl-stats: avl-stats.o avl.o
	$(CC) -ggdb avl-stats.o avl.o -o avl-stats

height.o: height.cc
	$(CC) $(CFLAGS) height.cc

treap.o: treap.cc
	$(CC) $(CFLAGS) treap.cc

naive.o: naive.cc
	$(CC) $(CFLAGS) naive.cc

avl-stats.o: avl-stats.cc
	$(CC) $(CFLAGS) avl-stats.cc

avl.o: avl.cc
	$(CC) $(CFLAGS) avl.cc
