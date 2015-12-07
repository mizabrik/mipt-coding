CC=g++
CFLAGS=-c -Wall -std=c++11 -g

all: clean calc # generate

run: calc
	./calc

debug: sort
	gdb sort

calc: main.o calc.o
	$(CC) -g main.o calc.o -o calc

#generate: generate.o util.o
	#$(CC) -g generate.o util.o -o generate

main.o: main.cc
	$(CC) $(CFLAGS) -g main.cc

calc.o: calc.cc
	$(CC) $(CFLAGS) -g calc.cc

clean:
	rm -f *.o calc
