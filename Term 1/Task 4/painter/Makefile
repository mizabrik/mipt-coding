CC=g++
CFLAGS=-c -pedantic -Wall -Wextra -std=c++14 -ggdb

all: clean painter

clean:
	rm -f *.o painter

painter: painter.o segment.o
	$(CC) -ggdb painter.o segment.o -o painter

painter.o: painter.cc
	$(CC) $(CFLAGS) painter.cc

segment.o: segment.cc
	$(CC) $(CFLAGS) segment.cc
