
CC = gcc
CFLAGS = -g -Wall -ansi -std=c99 -Wno-deprecated-declarations
INCLUDE =
LDFLAGS = -g -Wall
LIBS = -lcrypto
DEFINES = -DCHAR_BUF=255


all: main

main: main.o stream.o encrypt.o pbm.o merge.o decrypt.o
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS)

main.o: main.c
	$(CC) -c $(CFLAGS) $(INCLUDE) $(DEFINES) $<

stream.o: stream.c stream.h
	$(CC) -c $(CFLAGS) $(INCLUDE) $(DEFINES) $<

encrypt.o: encrypt.c encrypt.h
	$(CC) -c $(CFLAGS) $(INCLUDE) $(DEFINES) $<

pbm.o: pbm.c pbm.h
	$(CC) -c $(CFLAGS) $(INCLUDE) $(DEFINES) $<

merge.o: merge.c merge.h
	$(CC) -c $(CFLAGS) $(INCLUDE) $(DEFINES) $<

decrypt.o: decrypt.c decrypt.h
	$(CC) -c $(CFLAGS) $(INCLUDE) $(DEFINES) $<

clean:
	rm -f *.o core main *.stream *.pbm
