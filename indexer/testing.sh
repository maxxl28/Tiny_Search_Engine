#!/bin/bash
# testing.sh - Tests for the CS50 TSE Indexer
# Peter Owolabi, Spring 2026

CRAWLER_DATA=../crawler/testdata
INDEXCMP=/cs50/shared/tse/indexcmp

### ERROR CASES ###

echo "=== Test 1: No arguments ==="
./indexer
echo "Exit: $?"

echo "=== Test 2: One argument ==="
./indexer $CRAWLER_DATA/letters-1
echo "Exit: $?"

echo "=== Test 3: Too many arguments ==="
./indexer $CRAWLER_DATA/letters-1 myindex extra
echo "Exit: $?"

echo "=== Test 4: Non-existent pageDirectory ==="
./indexer /nonexistent/path myindex
echo "Exit: $?"

echo "=== Test 5: Not a crawler directory ==="
./indexer /tmp myindex
echo "Exit: $?"

echo "=== Test 6: Bad indexFilename path ==="
./indexer $CRAWLER_DATA/letters-1 /nonexistent/dir/myindex
echo "Exit: $?"

### CORRECTNESS TESTS ###

echo "=== Test 7: letters depth 0 ==="
./indexer $CRAWLER_DATA/letters-0 myindex_letters0
echo "Exit: $?"
./indextest myindex_letters0 myindex_letters0_copy
echo "Exit: $?"

echo "=== Test 8: letters depth 10 ==="
./indexer $CRAWLER_DATA/letters-10 myindex_letters10
echo "Exit: $?"
./indextest myindex_letters10 myindex_letters10_copy
echo "Exit: $?"

echo "=== Test 9: toscrape depth 1 ==="
./indexer $CRAWLER_DATA/toscrape-1 myindex_toscrape1
echo "Exit: $?"
./indextest myindex_toscrape1 myindex_toscrape1_copy
echo "Exit: $?"

echo "=== Test 10: wikipedia depth 1 ==="
./indexer $CRAWLER_DATA/wikipedia-1 myindex_wikipedia1
echo "Exit: $?"
./indextest myindex_wikipedia1 myindex_wikipedia1_copy
echo "Exit: $?"

### MEMORY TESTS ###

echo "=== Test 11: Valgrind on indexer ==="
valgrind --leak-check=full ./indexer $CRAWLER_DATA/letters-10 myindex_valgrind 2>&1
echo "Exit: $?"

echo "=== Test 12: Valgrind on indextest ==="
valgrind --leak-check=full ./indextest myindex_letters10 myindex_valgrind_copy 2>&1
echo "Exit: $?"

# Cleanup
rm -f myindex_*