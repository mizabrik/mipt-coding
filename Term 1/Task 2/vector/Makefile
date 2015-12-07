CC=g++
CFLAGS=-c -Wall -std=c++11

all: vector

run: vector
	./vector

clean:
	rm -f *.o vector

debug: vector
	gdb vector

vector: main.o
	$(CC) -ggdb main.o -o vector

main.o: main.cc
	$(CC) $(CFLAGS) -ggdb main.cc
