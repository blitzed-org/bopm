CCFLAGS= -g -Wall -fno-exceptions -fno-rtti -O3
CC= gcc

all: object bopm

object: 
	$(CC) $(CCFLAGS) -o main.o -c main.c
	$(CC) $(CCFLAGS) -o config.o -c config.c
	$(CC) $(CCFLAGS) -o irc.o -c irc.c
	$(CC) $(CCFLAGS) -o log.o -c log.c
	$(CC) $(CCFLAGS) -o scan.o -c scan.c

bopm: 
	$(CC) -o bopm main.o config.o irc.o log.o scan.o

clean: 
	rm -f *.o bopm

