CFLAGS= -Wall -g
CC= gcc

ifdef DEBUG_GCOV
CFLAGS += -fprofile-arcs -ftest-coverage -fbranch-probabilities
endif

ifdef DEBUG_GDB
CFLAGS += -ggdb
endif

objects = main.o config.o irc.o log.o scan.o

all: bopm
	rm -f *.da

config.o:	extern.h	config.h
irc.o:		extern.h	irc.h
log.o:		extern.h	log.h
main.o:		extern.h
scan.o:		extern.h	scan.h

bopm: $(objects)
	$(CC) -o bopm $(objects)

.PHONY: clean
clean: 
	rm -f *.o *.da bopm

