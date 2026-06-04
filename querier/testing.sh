#!/bin/bash

PAGEDIR=../crawler/testdata/letters-10
INDEXFILE=letters-10.index

../indexer/indexer $PAGEDIR $INDEXFILE

# arg stuff
echo "Argument tests"

./querier

./querier $PAGEDIR

# three arguments
./querier $PAGEDIR $INDEXFILE extra

# directory
./querier /no/such/dir $INDEXFILE

./querier /tmp $INDEXFILE

./querier $PAGEDIR /no/such/file


# making sure syntaz is checked
echo "Syntax error tests"
./querier $PAGEDIR $INDEXFILE <<EOF
and
or
and earth
or earth
planet earth or
planet earth and
planet earth and or science
planet earth and and science
planet earth or and science
Warning!
hello (world)
EOF


# correctness
echo "Test: words"
./querier $PAGEDIR $INDEXFILE <<EOF
home
tse
for
nonexistentword
EOF

echo "Test: and in the quierie"
./querier $PAGEDIR $INDEXFILE <<EOF
home and page
tse and home
search and first
EOF

echo "Test: or queries"
./querier $PAGEDIR $INDEXFILE <<EOF
home or page
tse or algorithm
EOF

echo "Test: and/or "
./querier $PAGEDIR $INDEXFILE <<EOF
home and page or tse
coding or search and first
EOF

echo "Test: not there and"
./querier $PAGEDIR $INDEXFILE <<EOF
home page
tse home
EOF

echo "Test: empty/whitespace"
./querier $PAGEDIR $INDEXFILE <<EOF

   
EOF


#prof provided fuzz test
echo "Fuzz test"
./fuzzquery $INDEXFILE 20 42 | ./querier $PAGEDIR $INDEXFILE


# valgrind
echo "Valgrind"
./fuzzquery $INDEXFILE 5 7 | valgrind --leak-check=full --show-leak-kinds=all ./querier $PAGEDIR $INDEXFILE