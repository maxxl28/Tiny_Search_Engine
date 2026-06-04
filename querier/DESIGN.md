# Design spec

## data structures

1) index_t is a hastable that connects words to counters_t
2) counters_t is a set of (docID, count) pairs. 
3) two_counters_t is a struct we define holding two counters_t pointers. We need this because intersectHelper function needs to be able to access both the result counters and the other counters
4) max_doc is another struct holding a docID and a score. We use this one to rank the highest-scoring document

## Pseudocode

1) Main loop:
parse and validate command-line arguments
load index from file
while there is input:
read a line
tokenize line into array of lowercase words
if tokenization fails (bad characters), skip
if no words, skip
validate query syntax (no leading/trailing/adjacent operators)
if invalid, skip
print the cleaned query
evaluate the query against the index
rank and print the results
free everything for this query
delete the index

2) tokenizeQuery
allocate array of char* pointers
walk a pointer through the line character by character:
skip whitespace
if non-alphabet character found, return error
mark start of word
advance pointer while alphabet
copy characters into a new string and lowercase it
store in array
return the word count

3) validateQuery:
check first word is not "and" or "or"
check last word is not "and" or "or"
for each adjacent pair of words:
    if both are operators, return error
return true

4) runQuery

total = NULL
while words remain:
product = andSequence(words, position, index)
if total is NULL:
total = product
else:
union product into total (add scores)
free product
if next word is "or":
skip it, continue loop
else:
break
return total

5) addSequence:
result = new empty counters
copy first word's counters into result via union
advance position
while more words and next word is not "or":
if word is "and", skip it
look up word in index
if found:
intersect result with word's counters keep the min scores
else:
reset result to empty (AND with nothing = nothing)
advance position
return result

6) intersectCounters - AND
for each (docID, count) in result:
look up docID in other
set result[docID] = min(count, otherCount)

7) unionCounters - OR
for each (docID, count) in src:
set dest[docID] = dest[docID] + count

8) rankAndPrint
count documents with nonzero scores
if none, print "No documents match."
for each rank position:
scan all entries, find the one with highest score
read URL from pageDirectory/docID file
print score, docID, URL
set that entry's score to 0


