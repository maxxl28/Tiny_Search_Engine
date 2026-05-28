# Makefile for indexer
CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I../libcs50 -I../common
LIBS = ../common/common.a ../libcs50/libcs50-given.a

.PHONY: all clean test

all: indexer indextest

indexer: indexer.o $(LIBS)
	$(CC) $(CFLAGS) -o $@ $^

indextest: indextest.o $(LIBS)
	$(CC) $(CFLAGS) -o $@ $^

indexer.o: indexer.c
indextest.o: indextest.c

test: indexer indextest
	bash -v testing.sh &> testing.out

clean:
	rm -f indexer indextest *.o testing.out
