# Makefile for CS50 Tiny Search Engine
#
# David Kotz - April 2016, 2017, 2021
L = libcs50
.PHONY: all clean

############## default: make all libs and programs ##########
all:
	(cd $L && rm -f $L.a && if [ -r set.c ]; then make $L.a; else cp $L-given.a $L.a; fi)
	make -C common
	make -C crawler
	make -C indexer
	make -C querier

############### TAGS for emacs users ##########
TAGS:  Makefile */Makefile */*.c */*.h */*.md */*.sh
	etags $^

############## clean  ##########
clean:
	rm -f *~
	rm -f TAGS
	make -C libcs50 clean
	make -C common clean
	make -C crawler clean
	make -C indexer clean
	make -C querier clean