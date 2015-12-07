CC=g++
CFLAGS=-Wall -std=c++14 -g -c

all: run

run: nested_length
	./nested_length

#nested_length: main.o array.o segments.o
nested_length: main.o segments.o
	#$(CC) -std=c++14 -g main.o array.o segments.o -o nested_length
	$(CC) -std=c++14 -g main.o segments.o -o nested_length

main.o: main.cc
	$(CC) $(CFLAGS) main.cc

#array.o: array.cc
	#$(CC) $(CFLAGS) array.cc

segments.o: segments.cc
	$(CC) $(CFLAGS) segments.cc
