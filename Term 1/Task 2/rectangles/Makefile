CC=g++
CFLAGS=-c -Wall -std=c++11 -g

all: clean rectangles

run: 
	./rectangles

rectangles: main.o rectangles.o
	$(CC) -g main.o rectangles.o -o rectangles

main.o: main.cc
	$(CC) $(CFLAGS) -g main.cc

rectangles.o: rectangles.cc
	$(CC) $(CFLAGS) -g rectangles.cc

clean:
	rm -f *.o rectangles
