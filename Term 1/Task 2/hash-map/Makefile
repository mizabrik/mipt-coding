CC=g++
CFLAGS=-c -Wall -std=c++11

all: hash-table

run: hash-table
	./hash-table

clean:
	rm *.o hash-table

debug: hash-map
	gdb hash-table

hash-table: main.o hash_table.o
	$(CC) -ggdb main.o hash_table.o -o hash-table

main.o: main.cc
	$(CC) $(CFLAGS) -ggdb main.cc

hash_table.o: hash_table.cc
	$(CC) $(CFLAGS) -ggdb hash_table.cc
