# Makefile for crawler
# Builds the crawler executable

CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I../libcs50 -I../common
LIBS = ../libcs50/libcs50.a ../common/common.a

.PHONY: all clean test

all: crawler

crawler: crawler.o ../common/common.a ../libcs50/libcs50.a
	$(CC) $(CFLAGS) -o crawler crawler.o $(LIBS)

crawler.o: crawler.c ../common/pagedir.h
	$(CC) $(CFLAGS) -c crawler.c

test: crawler
	bash -v testing.sh

clean:
	rm -f *~ *.o crawler
	rm -f testing.out