CCFLAGS= -g -Wall -fno-exceptions -fno-rtti -O3
CC= gcc

objects = main.o config.o irc.o log.o scan.o

all: bopm

config.o:	extern.h	config.h
irc.o:		extern.h	irc.h
log.o:		extern.h	log.h
main.o:		extern.h
scan.o:		extern.h	scan.h

bopm: $(objects)
	$(CC) -o bopm $(objects)

.PHONY: clean
clean: 
	rm -f *.o bopm

