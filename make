#!/bin/sh
gcc -g -Wall -fno-exceptions -fno-rtti -O3 -o main.o -c main.c
gcc -g -Wall -fno-exceptions -fno-rtti -O3 -o config.o -c config.c
gcc -g -Wall -fno-exceptions -fno-rtti -O3 -o irc.o -c irc.c
gcc -g -Wall -fno-exceptions -fno-rtti -O3 -o log.o -c log.c
gcc -o bopm main.o config.o irc.o log.o
