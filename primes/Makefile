
CC = gcc
CFLAGS = -g -Wall -ansi -std=c99 -Wno-deprecated-declarations
INCLUDE =
LDFLAGS = -g -Wall
LIBS = -lcrypto -lm
DEFINES = -DLITTLE_ENDIAN -DCHAR_BUF_LEN=1024 -DDEBUG_


all: main

main: main.o primes.o trialdiv.o millerrabin.o rndsearch.o util.o maurer.o
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS)

main.o: main.c
	$(CC) -c $(CFLAGS) $(INCLUDE) $(DEFINES) $<

primes.o: primes.c primes.h
	$(CC) -c $(CFLAGS) $(INCLUDE) $(DEFINES) $<

trialdiv.o: trialdiv.c trialdiv.h
	$(CC) -c $(CFLAGS) $(INCLUDE) $(DEFINES) $<

millerrabin.o: millerrabin.c millerrabin.h
	$(CC) -c $(CFLAGS) $(INCLUDE) $(DEFINES) $<

rndsearch.o: rndsearch.c rndsearch.h
	$(CC) -c $(CFLAGS) $(INCLUDE) $(DEFINES) $<

util.o: util.c util.h
	$(CC) -c $(CFLAGS) $(INCLUDE) $(DEFINES) $<

maurer.o: maurer.c maurer.h
	$(CC) -c $(CFLAGS) $(INCLUDE) $(DEFINES) $<

clean:
	rm -f *.o core main *.pms *.td *.mr *.rs *.ma
