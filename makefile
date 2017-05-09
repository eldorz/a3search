CC=g++
CFLAGS=-Wall -Werror -std=c++11 -O2

all : a3search

debug : CFLAGS=-Wall -Werror -gdwarf-3 -std=c++11
debug : all

prof : CFLAGS=-Wall -Werror -pg -std=c++11
prof : all

a3search : a3search.o indexer.o search.o common.o
	$(CC) $(CFLAGS) -o a3search a3search.o indexer.o search.o common.o

a3search.o : a3search.cpp a3search.h constants.h indexer.h search.h
	$(CC) $(CFLAGS) -c a3search.cpp

indexer.o : indexer.cpp indexer.h constants.h stemming/english_stem.h
	$(CC) $(CFLAGS) -c indexer.cpp

search.o : search.cpp search.h constants.h
	$(CC) $(CFLAGS) -c search.cpp

common.o : common.cpp common.h
	$(CC) $(CFLAGS) -c common.cpp

stemming/english_stem.h : stemming/stemming.h

stemming/stemming.h : indexing/string_util.h indexing/common_lang_constants.h

indexing/string_util.h : utilities/safe_math.h utilities/utilities.h utilities/debug_logic.h

clean :
	-rm a3search *.o
