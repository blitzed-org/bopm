CFLAGS= -Wall -g -O3
CC= gcc

ifdef DEBUG_GCOV
CFLAGS += -fprofile-arcs -ftest-coverage -fbranch-probabilities
endif

ifdef DEBUG_GDB
CFLAGS += -ggdb
endif

objects = main.o config.o irc.o log.o scan.o stats.o

all: bopm
	rm -f *.da

config.o: config.h log.h
irc.o:    config.h extern.h irc.h   log.h    scan.h
log.o:    extern.h log.h
main.o:   extern.h irc.h    log.h   scan.h   stats.h
scan.o:   irc.h    extern.h log.h   config.h scan.h  stats.h
stats.o:  irc.h    extern.h stats.h

bopm: $(objects)
	$(CC) -o bopm $(objects)

.PHONY: clean
clean: 
	rm -f *.o *.da bopm

