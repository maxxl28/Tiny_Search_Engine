CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I../libcs50 -I../common
LIBS = ../common/common.a ../libcs50/libcs50.a

.PHONY: all clean test

all: querier fuzzquery

querier: querier.o $(LIBS)
	$(CC) $(CFLAGS) -o $@ $^

fuzzquery: fuzzquery.o $(LIBS)
	$(CC) $(CFLAGS) -o $@ $^

querier.o: querier.c
fuzzquery.o: fuzzquery.c

test: querier fuzzquery
	bash -v testing.sh &> testing.out

clean:
	rm -f querier fuzzquery *.o testing.out