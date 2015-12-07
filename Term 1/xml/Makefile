CC=g++
CFLAGS=-c -Wall -std=c++14 -ggdb

all: clean simple-parser

run: simple-parser
	./simple-parser

clean:
	rm -f *.o simple-parser

debug: simple-parser
	gdb simple-parser

simple-parser: parser.o
	$(CC) -ggdb parser.o -o simple-parser

parser.o: parser.cc
	$(CC) $(CFLAGS) parser.cc
