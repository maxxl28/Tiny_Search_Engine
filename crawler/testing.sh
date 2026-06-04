#!/bin/bash

# testing for crawler

CRAWLER=./crawler
LETTERS=http://cs50tse.cs.dartmouth.edu/tse/letters/
TOSCRAPE=http://cs50tse.cs.dartmouth.edu/tse/toscrape/
WIKIPEDIA=http://cs50tse.cs.dartmouth.edu/tse/wikipedia/
DATA=./testdata

mkdir -p $DATA

# Error cases

echo "Test: no arguments"
$CRAWLER
echo "Exit: $?"

echo "Test: too few arguments"
$CRAWLER $LETTERS $DATA/tmp
echo "Exit: $?"

echo "Test: too many arguments"
$CRAWLER $LETTERS $DATA/tmp 0 extra
echo "Exit: $?"

echo "Test: non-internal seedURL"
$CRAWLER http://www.google.com $DATA/tmp 0
echo "Exit: $?"

echo " Test: invalid seedURL (not a URL)"
$CRAWLER notaURL $DATA/tmp 0
echo "Exit: $?"

echo "Test: non-existent pageDirectory"
$CRAWLER $LETTERS $DATA/doesNotExist 0
echo "Exit: $?"

echo "Test: maxDepth negative "
mkdir -p $DATA/tmp
$CRAWLER $LETTERS $DATA/tmp -1
echo "Exit: $?"

echo "Test: maxDepth too large "
$CRAWLER $LETTERS $DATA/tmp 11
echo "Exit: $?"

echo "Test: maxDepth not an integer "
$CRAWLER $LETTERS $DATA/tmp foo
echo "Exit: $?"

# Valgrind memory check

echo "Test: valgrind on toscrape depth 1"
mkdir -p $DATA/valgrind
valgrind --leak-check=full --error-exitcode=1 \
    $CRAWLER $TOSCRAPE $DATA/valgrind 1
echo "Exit: $?"

# Valid crawls

echo "Test: letters at depth 0"
mkdir -p $DATA/letters-0
$CRAWLER $LETTERS $DATA/letters-0 0
echo "Exit: $?"
ls $DATA/letters-0

echo "Test: letters at depth 10"
mkdir -p $DATA/letters-10
$CRAWLER $LETTERS $DATA/letters-10 10
echo "Exit: $?"
ls $DATA/letters-10

echo "Test: toscrape at depth 0"
mkdir -p $DATA/toscrape-0
$CRAWLER $TOSCRAPE $DATA/toscrape-0 0
echo "Exit: $?"
ls $DATA/toscrape-0

echo "Test: toscrape at depth 1"
mkdir -p $DATA/toscrape-1
$CRAWLER $TOSCRAPE $DATA/toscrape-1 1
echo "Exit: $?"
ls $DATA/toscrape-1

echo "Test: wikipedia at depth 0"
mkdir -p $DATA/wikipedia-0
$CRAWLER $WIKIPEDIA $DATA/wikipedia-0 0
echo "Exit: $?"
ls $DATA/wikipedia-0

echo "Test: wikipedia at depth 1"
mkdir -p $DATA/wikipedia-1
$CRAWLER $WIKIPEDIA $DATA/wikipedia-1 1
echo "Exit: $?"
ls $DATA/wikipedia-1
