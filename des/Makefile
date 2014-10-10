
CC = gcc
CFLAGS = -g -Wall -ansi -std=c99
INCLUDE =
LDFLAGS = -g -Wall
LIBS =
DEFINES = -DCHAR_BUF_LEN=255


all: main

main: main.o util.o des.o tablecheck.o keyschedule.o encrypt.o decrypt.o encrypt3.o decrypt3.o
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS)

main.o: main.c
	$(CC) -c $(CFLAGS) $(INCLUDE) $(DEFINES) $<

util.o: util.c util.h
	$(CC) -c $(CFLAGS) $(INCLUDE) $(DEFINES) $<

des.o: des.c des.h
	$(CC) -c $(CFLAGS) $(INCLUDE) $(DEFINES) $<

tablecheck.o: tablecheck.c tablecheck.h
	$(CC) -c $(CFLAGS) $(INCLUDE) $(DEFINES) $<

keyschedule.o: keyschedule.c keyschedule.h
	$(CC) -c $(CFLAGS) $(INCLUDE) $(DEFINES) $<

encrypt.o: encrypt.c encrypt.h
	$(CC) -c $(CFLAGS) $(INCLUDE) $(DEFINES) $<

decrypt.o: decrypt.c decrypt.h
	$(CC) -c $(CFLAGS) $(INCLUDE) $(DEFINES) $<

encrypt3.o: encrypt3.c encrypt3.h
	$(CC) -c $(CFLAGS) $(INCLUDE) $(DEFINES) $<

decrypt3.o: decrypt3.c decrypt3.h
	$(CC) -c $(CFLAGS) $(INCLUDE) $(DEFINES) $<

clean:
	rm -f *.o core main ferr fout *.enc *.dat *.0 *.1 *.2 *.3 *.4 *.5 *.6 *.7 *.8 *.9
