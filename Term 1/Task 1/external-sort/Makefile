CC=g++
CFLAGS=-c -Wall -std=c++11

all: clean sort # generate

run: sort
	./sort

debug: sort
	gdb sort

sort: main.o util.o
	$(CC) -g main.o util.o -o sort

#generate: generate.o util.o
	#$(CC) -g generate.o util.o -o generate

main.o: main.cc
	$(CC) $(CFLAGS) -g main.cc

util.o: util.cc
	$(CC) $(CFLAGS) -g util.cc

#generate.o: generate.cc
	#$(CC) $(CFLAGS) -g generate.cc

clean:
	rm -f *.o sort
