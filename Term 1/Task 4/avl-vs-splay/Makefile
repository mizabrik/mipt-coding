CC=g++
CFLAGS=-c -pedantic -Wall -Wextra -std=c++14 -ggdb

all: clean avl-stats

clean:
	rm -f *.o avl-stats

avl-stats: avl-stats.o avl.o
	$(CC) -ggdb avl-stats.o avl.o -o avl-stats

avl-stats.o: avl-stats.cc
	$(CC) $(CFLAGS) avl-stats.cc

avl.o: avl.cc
	$(CC) $(CFLAGS) avl.cc
