CC=gcc
CPPC=g++
CPPFLAGS=-Wall -Werror -std=c++11 -O2
CFLAGS=-w -O2
WORDNET_OFILES=wordnet/binsrch.o wordnet/search.o wordnet/wnrtl.o wordnet/morph.o wordnet/wnglobal.o wordnet/wnhelp.o wordnet/wnutil.o

all : a3search

debug : CPPFLAGS=-Wall -Werror -gdwarf-3 -std=c++11
debug : CFLAGS=-w -gdwarf-3
debug : all

prof : CPPFLAGS=-Wall -Werror -pg -std=c++11
prof : CFLAGS=-w -pg
prof : all

a3search : a3search.o indexer.o search.o common.o $(WORDNET_OFILES)
	$(CPPC) $(CPPFLAGS) -o a3search a3search.o indexer.o search.o common.o $(WORDNET_OFILES)

a3search.o : a3search.cpp a3search.h constants.h indexer.h search.h
	$(CPPC) $(CPPFLAGS) -c a3search.cpp

indexer.o : indexer.cpp indexer.h constants.h english_stem.h
	$(CPPC) $(CPPFLAGS) -c indexer.cpp

search.o : search.cpp search.h constants.h
	$(CPPC) $(CPPFLAGS) -c search.cpp

common.o : common.cpp common.h
	$(CPPC) $(CPPFLAGS) -c common.cpp

wordnet/binsrch.o : wordnet/binsrch.c
	$(CC) $(CFLAGS) -o wordnet/binsrch.o -c wordnet/binsrch.c 

wordnet/search.o : wordnet/search.c wordnet/wn.h
	$(CC) $(CFLAGS) -o wordnet/search.o -c wordnet/search.c 

wordnet/wnrtl.o : wordnet/wnrtl.c wordnet/wn.h
	$(CC) $(CFLAGS) -o wordnet/wnrtl.o -c wordnet/wnrtl.c

wordnet/morph.o : wordnet/morph.c wordnet/wn.h
	$(CC) $(CFLAGS) -o wordnet/morph.o -c wordnet/morph.c

wordnet/wnglobal.o : wordnet/wnglobal.c
	$(CC) $(CFLAGS) -o wordnet/wnglobal.o -c wordnet/wnglobal.c

wordnet/wnhelp.o : wordnet/wnhelp.c wordnet/wn.h
	$(CC) $(CFLAGS) -o wordnet/wnhelp.o -c wordnet/wnhelp.c

wordnet/wnutil.o : wordnet/wnutil.c wordnet/wn.h
	$(CC) $(CFLAGS) -o wordnet/wnutil.o -c wordnet/wnutil.c

clean :
	-rm a3search *.o wordnet/*.o
	-rm -rf indices
