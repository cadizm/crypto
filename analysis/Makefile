
CC = gcc
CFLAGS = -g -Wall -ansi -std=c99
INCLUDE =
LDFLAGS = -g -Wall
LIBS = -lcrypto -lm
DEFINES = -DCHAR_BUF_LEN=255


all: main

main: main.o keygen.o invkey.o crypt.o histo.o solve.o
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS)

main.o: main.c
	$(CC) -c $(CFLAGS) $(INCLUDE) $(DEFINES) $<

keygen.o: keygen.c keygen.h
	$(CC) -c $(CFLAGS) $(INCLUDE) $(DEFINES) $<

invkey.o: invkey.c invkey.h
	$(CC) -c $(CFLAGS) $(INCLUDE) $(DEFINES) $<

crypt.o: crypt.c crypt.h
	$(CC) -c $(CFLAGS) $(INCLUDE) $(DEFINES) $<

histo.o: histo.c histo.h
	$(CC) -c $(CFLAGS) $(INCLUDE) $(DEFINES) $<

solve.o: solve.c solve.h
	$(CC) -c $(CFLAGS) $(INCLUDE) $(DEFINES) $<

clean:
	rm -f *.o core main *.keys *.invkeys *.pt *.ct *.t*.i* f*.histo.* *.solve
