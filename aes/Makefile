
CC = gcc
CFLAGS = -g -Wall -ansi -std=c99
INCLUDE =
LDFLAGS = -g -Wall
LIBS =
DEFINES = -DCHAR_BUF_LEN=1024 -DDEBUG_


all: main

main: main.o util.o tablecheck.o modprod.o inverse.o keyexpand.o encrypt.o decrypt.o
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS)

main.o: main.c
	$(CC) -c $(CFLAGS) $(INCLUDE) $(DEFINES) $<

util.o: util.c util.h
	$(CC) -c $(CFLAGS) $(INCLUDE) $(DEFINES) $<

tablecheck.o: tablecheck.c tablecheck.h
	$(CC) -c $(CFLAGS) $(INCLUDE) $(DEFINES) $<

modprod.o: modprod.c modprod.h
	$(CC) -c $(CFLAGS) $(INCLUDE) $(DEFINES) $<

inverse.o: inverse.c inverse.h
	$(CC) -c $(CFLAGS) $(INCLUDE) $(DEFINES) $<

keyexpand.o: keyexpand.c keyexpand.h
	$(CC) -c $(CFLAGS) $(INCLUDE) $(DEFINES) $<

encrypt.o: encrypt.c encrypt.h
	$(CC) -c $(CFLAGS) $(INCLUDE) $(DEFINES) $<

decrypt.o: decrypt.c decrypt.h
	$(CC) -c $(CFLAGS) $(INCLUDE) $(DEFINES) $<

clean:
	rm -f *.o core main *.mp *.inv *.enc
