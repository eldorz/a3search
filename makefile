CC=g++
CFLAGS=-Wall -Werror -std=c++11 -O3

all : a3search

debug : CFLAGS=-Wall -Werror -gdwarf-3 -std=c++11
debug : all

prof : CFLAGS=-Wall -Werror -pg -std=c++11
prof : all

a3search : a3search.o indexer.o
	$(CC) $(CFLAGS) -o a3search a3search.o indexer.o

a3search.o : a3search.cpp a3search.h
	$(CC) $(CFLAGS) -c a3search.cpp

indexer.o : indexer.cpp indexer.h
	$(CC) $(CFLAGS) -c indexer.cpp

clean :
	-rm a3search *.o
