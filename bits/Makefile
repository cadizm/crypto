
CC = gcc
CFLAGS = -g -Wall -ansi -std=c99
LDFLAGS = -g -Wall
LIBS = -lm

all: main

main: main.o hexdump.o enc-base64.o dec-base64.o mul.o div.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

main.o: main.c
	$(CC) -c $(CFLAGS) $<

hexdump.o: hexdump.c hexdump.h
	$(CC) -c $(CFLAGS) $<

enc-base64.o: enc-base64.c enc-base64.h
	$(CC) -c $(CFLAGS) $<

dec-base64.o: dec-base64.c dec-base64.h
	$(CC) -c $(CFLAGS) $<

mul.o: mul.c mul.h
	$(CC) -c $(CFLAGS) $<

div.o: div.c div.h
	$(CC) -c $(CFLAGS) $<

clean:
	rm -f *.o core main *.hex *.b64 *.dat *.mul *.div
