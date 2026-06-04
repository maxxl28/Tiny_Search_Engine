# **CS50 TSE Querier**

## **Design Spec**

In this document we reference the Requirements Specification and focus on the design-level decisions for the Querier. We describe the abstract data structures, module decomposition, and pseudocode for the querier's logic.

## **User interface**

The querier's only interface with the user is on the command line and via stdin:

./querier pageDirectory indexFilename

For example:

$ ./querier ../data/letters ../data/letters.index

After startup, the querier reads queries from stdin one line at a time, printing results to stdout, until EOF.

## **Inputs and outputs**

**Input:**

* Two command-line arguments: a Crawler-produced pageDirectory and an Indexer-produced indexFilename.  
* Queries read from stdin, one per line.

**Output:**

* For each valid query: the clean query, then a ranked list of matching documents (score, docID, URL), or No documents match.  
* Error messages to stderr for invalid arguments or malformed queries.  
* Nothing else to stdout.

## **Functional decomposition into modules**

We anticipate the following modules or functions:

1. **main**: parses arguments, loads index, drives the query loop  
2. **parseArgs**: validates command-line arguments  
3. **tokenizeQuery**: reads a line, validates characters, splits into an array of normalized words  
4. **validateQuery**: checks that operators are not first, last, or adjacent  
5. **runQuery**: walks the token array following the BNF grammar, computing the result counters  
6. **andSequence**: computes the intersection (running minimum) for one andsequence  
7. **rankAndPrint**: ranks documents by score (descending) and prints each with its URL

Helper modules as well:

1. **index**: loads the inverted index from file; provides index\_find  
2. **pagedir**: provides pagedir\_validate and URL lookup from a document file  
3. **counters**: used for per-document scores; intersection and union operations implemented via counters\_iterate

## **Major data structures**

**index\_t**: a hashtable mapping word (string) to counters\_t\*. Each counters object maps docID (int) to count (int). This is loaded once at startup from indexFilename and is read-only during query processing.

**counters\_t (result)**: a temporary counters object built during query processing, mapping docID to score. Two operations on counters are central:

* **Intersection** (for AND): iterate over one counters, look up each docID in the other, keep the minimum count. Documents absent from either side get score zero (removed).  
* **Union** (for OR): iterate over one counters, add each docID's count into the other. Think of AND as a set intersection (narrowing) and OR as a set union (broadening).

## **Pseudo code for logic/algorithmic flow**

The querier runs as follows:

parse and validate command-line arguments  
load the index from indexFilename  
loop:  
    prompt the user (if stdin is a tty)  
    read one line from stdin; if EOF, break  
    tokenize and normalize the line into a word array  
    if no words, continue  
    validate query structure (no leading/trailing/adjacent operators)  
    if invalid, print error and continue  
    print the clean query  
    call runQuery(wordArray, index) to get result counters  
    call rankAndPrint(result, pageDirectory) to display matches  
    free result counters  
delete the index  
exit zero

where **runQuery**:

initialize a running 'total' counters (for OR accumulation) to NULL  
initialize index i \= 0  
outer loop (over andsequences separated by 'or'):  
    call andSequence(wordArray, \&i, index) to get a 'product' counters  
    union 'product' into 'total' (sum counts for each docID)  
    free 'product'  
    if wordArray\[i\] \== "or", advance i and continue outer loop  
    else break  
return 'total'

where **andSequence**:

get counters for wordArray\[i\] from index; copy into 'result'  
advance i  
while wordArray\[i\] is not NULL and not "or":  
    if wordArray\[i\] \== "and", advance i and continue  
    get counters for wordArray\[i\] from index  
    intersect 'result' with those counters (keep minimum per docID)  
    advance i  
return 'result'

where **rankAndPrint**:

loop:  
    use counters\_iterate to find the docID with the highest score  
    if highest score \== 0, break  
    read the URL from pageDirectory/docID  
    print score, docID, URL  
    set that docID's count to 0 in result (mark as printed as well)

This is effectively a selection sort which will have a runtime O(n squared) but fine for typical query result sizes.

## **Error handling and recovery**

* Invalid command-line arguments:print to stderr, exit non-zero.  
* Query lines with non-letter, non-space characters: print error, skip query.  
* Operator as first or last token: print error, skip query.  
* Two adjacent operators: print error, skip query.  
* Words not found in index: treated as empty counters (zero matches); intersection produces empty result.  
* Out-of-memory: mem\_assert prints to stderr and exits non-zero.

## **Testing plan**

**Unit testing:**

* tokenizeQuery tested with edge cases: leading/trailing spaces, multiple spaces between words, all-uppercase input, mixed case.  
* andSequence / runQuery tested with known index files and manually verified expected scores.

**Integration testing:**

* Test with invalid arguments (wrong count, bad pageDirectory, unreadable indexFile).  
* Test with malformed queries (operators first/last/adjacent, bad characters).  
* Test with valid single-word, multi-word, AND-only, OR-only, and mixed queries against /cs50/shared/tse/output/ directories.  
* Compare output against known-good results.  
* Run valgrind to verify no memory leaks or errors.

