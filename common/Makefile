# Makefile for common library
CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I../libcs50

OBJS = pagedir.o index.o word.o

common.a: $(OBJS)
	ar cr common.a $(OBJS)

pagedir.o: pagedir.c pagedir.h
index.o: index.c index.h
word.o: word.c word.h

.PHONY: clean
clean:
	rm -f common.a *.o
