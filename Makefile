CFLAGS= -Wall -g -O3
CC= gcc

ifdef DEBUG_GCOV
CFLAGS += -fprofile-arcs -ftest-coverage -fbranch-probabilities
endif

ifdef DEBUG_GDB
CFLAGS += -ggdb
endif

objects = main.o config.o irc.o log.o misc.o scan.o stats.o opercmd.o

all: bopm
	rm -f *.da

config.o:  config.h                log.h
irc.o:     config.h extern.h irc.h log.h        opercmd.h scan.h
log.o:              extern.h       log.h
main.o:             extern.h irc.h log.h        opercmd.h scan.h stats.h
misc.o:             extern.h             misc.h
opercmd.o:          extern.h irc.h log.h misc.h opercmd.h scan.h
scan.o:    config.h extern.h irc.h log.h        opercmd.h scan.h stats.h
stats.o:            extern.h irc.h       misc.h                  stats.h

bopm: $(objects)
	$(CC) -o bopm $(objects)

.PHONY: clean
clean: 
	rm -f *.o *.da bopm

