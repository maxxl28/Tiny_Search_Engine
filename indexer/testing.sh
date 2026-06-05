#!/bin/bash

# Ale Sprin26

CRAWLER_DATA=../crawler/testdata

#args
echo "Test: no arguments"
./indexer
echo "Exit: $?"

echo "Test: one argument"
./indexer $CRAWLER_DATA/letters-0
echo "Exit: $?"

echo "Test: extra arguments"
./indexer $CRAWLER_DATA/letters-0 myindex extra
echo "Exit: $?"

# pageDirectories stuff
echo "Test: bad pageDirectory"
./indexer /nonexistent/path myindex
echo "Exit: $?"

echo "Test: not crawler dir"
./indexer /tmp myindex
echo "Exit: $?"

echo "Test: bad indexFile path"
./indexer $CRAWLER_DATA/letters-0 /nonexistent/dir/myindex
echo "Exit: $?"

echo "Test: read-only directory"
mkdir -p readonlydir
chmod -w readonlydir
./indexer $CRAWLER_DATA/letters-0 readonlydir/myindex
echo "Exit: $?"
chmod +w readonlydir
rm -rf readonlydir

echo "Test: read-only file"
touch readonlyfile
chmod -w readonlyfile
./indexer $CRAWLER_DATA/letters-0 readonlyfile
echo "Exit: $?"
chmod +w readonlyfile
rm -f readonlyfile

# letters depth and test with indextest
echo "Test: letters depth 0"
./indexer $CRAWLER_DATA/letters-0 myindex_letters0
echo "Exit: $?"
./indextest myindex_letters0 myindex_letters0_copy
echo "Exit: $?"

echo "Test: letters depth 10"
./indexer $CRAWLER_DATA/letters-10 myindex_letters10
echo "Exit: $?"
./indextest myindex_letters10 myindex_letters10_copy
echo "Exit: $?"

echo "Test: toscrape depth 1"
./indexer $CRAWLER_DATA/toscrape-1 myindex_toscrape1
echo "Exit: $?"
./indextest myindex_toscrape1 myindex_toscrape1_copy
echo "Exit: $?"

echo "Test: wikipedia depth 1"
./indexer $CRAWLER_DATA/wikipedia-1 myindex_wikipedia1
echo "Exit: $?"
./indextest myindex_wikipedia1 myindex_wikipedia1_copy
echo "Exit: $?"

# valgrind 
echo "Test: valgrind indexer"
valgrind --leak-check=full --show-leak-kinds=all ./indexer $CRAWLER_DATA/letters-10 myindex_valgrind 2>&1
echo "Exit: $?"

echo "Test: valgrind indextest"
valgrind --leak-check=full --show-leak-kinds=all ./indextest myindex_letters10 myindex_valgrind_copy 2>&1
echo "Exit: $?"

# cleanup
rm -f myindex_*