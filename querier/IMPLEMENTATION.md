# Implementation Spec

## Functions

`main` — parses args, loads index, runs the query loop reading lines from stdin, and frees everything on EOF.

`parseArgs` — validates argc == 3, checks pageDirectory is a crawler directory via `pagedir_validate`, checks indexFilename is readable by opening and closing it.

`tokenizeQuery` — takes a raw input line and produces an array of lowercase word strings. Returns false if any non-alphabet non-space character is found.

`validateQuery` — checks that "and"/"or" are not the first or last word, and that no two operators are adjacent. Returns false and prints an error if so

`runQuery` — implements the outer OR loop of the query grammar. 

`andSequence` — implements the inner AND loop. 

`intersectCounters` — iterates over result, and for each docID sets the score to the minimum of result's score and other's score.

`intersectHelper` — heler for `intersectCounters`.

`unionCounters` — iterates over src, and for each docID adds src's count to dest's existing count. Passes dest directly as the `void*` argument since only one counters is needed.

`unionHelper` — helper for `unionCounters`

`rankAndPrint` — prints results in decreasing score order using selection sort. 

`rankHelper` — helper that tracks the document with the highest count seen so far.

`countHelper` — helper that counts entries with count > 0.

`prompt` — prints "Query? " only when stdin is a terminal 

## Testing Plan

Testing is done via `testing.sh`, run by `make test`.

1. Arg errors

2. Syntax errors

3. Correcteness 
4. Fuzz test provided by prof

5. Valgrind