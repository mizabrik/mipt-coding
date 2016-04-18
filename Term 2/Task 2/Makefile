CC=gcc
CXX=g++
CXXFLAGS=-std=c++14 -O3 -g
LDLIBS=-lstdc++

test: test.o graph.o traversal.o

test.o: graph.h traversal.h

clean:
	$(RM) *.o test
