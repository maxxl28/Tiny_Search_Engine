# **CS50 TSE Indexer**

## **Implementation Spec**

In this document we reference the Requirements Specification and Design Specification and focus on the implementation-specific decisions. Here we focus on the core subset:

* Data structures  
* Control flow: pseudo code for overall flow, and for each of the functions  
* Detailed function prototypes and their parameters  
* Error handling and recovery  
* Testing plan

## **Data structures**

We use one major data structure: an **index**, which is a hashtable mapping from words (strings) to counters objects.

* The **hashtable** is keyed by word (string) and stores a counters\_t\* as its item.  
* Each **counters** object is keyed by docID (int) and stores the count of occurrences of that word in that document.

Think of it like a library card catalog: the hashtable is the cabinet of drawers (one drawer per word), and each drawer holds a set of cards (one card per document, stamped with how many times the word appears).

The hashtable is initialized with 500 slots: a reasonable middle ground given we cannot know in advance how many unique words we will find. When loading from a file, we count lines first and size accordingly.

## **Control flow**

The Indexer is implemented in indexer.c, with three functions: main, indexBuild, and indexPage.

### **main**

The main function calls parseArgs, then indexBuild, then saves and deletes the index.

Pseudocode:

call parseArgs to validate and extract pageDirectory and indexFilename  
call indexBuild(pageDirectory) to produce an index  
call index\_save(index, indexFilename) to write index to file  
call index\_delete(index) to free memory  
exit zero

### **parseArgs**

Given arguments from the command line, extract them into the function parameters; return only if successful.

if argc \!= 3, print error to stderr and exit non-zero  
assign pageDirectory \= argv\[1\]  
call pagedir\_validate(pageDirectory); if false, print error and exit non-zero  
assign indexFilename \= argv\[2\]  
attempt to open indexFilename for writing; if fail, print error and exit non-zero  
close the file

### **indexBuild**

Builds an in-memory index by reading all page files from pageDirectory.

Pseudocode:

create a new index object  
for docID \= 1, 2, 3, ...:  
    call pagedir\_load(pageDirectory, docID) to get a webpage\_t\*  
    if webpage is NULL, break (no more documents)  
    call indexPage(index, webpage, docID)  
    call webpage\_delete(webpage)  
return the index

### **indexPage**

Scans a single webpage and adds all valid words to the index.

Pseudocode:

pos \= 0  
while (word \= webpage\_getNextWord(webpage, \&pos)) is not NULL:  
    if strlen(word) \< 3:  
        free(word)  
        continue  
    call normalizeWord(word)  
    look up word in index to get its counters object  
    if counters is NULL:  
        create a new counters object  
        insert it into the index under this word  
    increment the counter for docID in that counters object  
    free(word)

## **Other modules**

### **pagedir (extensions)**

We extend pagedir.c in ../common with two new functions needed by the Indexer.

Pseudocode for pagedir\_validate:

construct the pathname pageDirectory/.crawler  
attempt to open that file for reading  
if open fails, return false  
close the file and return true

Pseudocode for pagedir\_load:

construct the pathname pageDirectory/docID (e.g. "../data/letters/1")  
attempt to open that file for reading  
if open fails, return NULL  
read the first line as the URL  
read the second line as the depth (integer)  
read the rest of the file as the HTML content  
close the file  
create and return a webpage\_t\* using webpage\_new(url, depth, html)

### **index**

We create a new module index.c in ../common. The index\_t type is a thin wrapper around a hashtable\_t.

Function prototypes:

index\_t\* index\_new(const int num\_slots);  
void     index\_delete(index\_t\* index);  
void     index\_save(index\_t\* index, const char\* indexFilename);  
index\_t\* index\_load(const char\* indexFilename);  
bool     index\_insert(index\_t\* index, const char\* word, counters\_t\* ctrs);  
counters\_t\* index\_find(index\_t\* index, const char\* word);

Pseudocode for index\_save:

open indexFilename for writing; on error, print to stderr and return  
iterate over all entries in the hashtable:  
    for each (word, counters) pair:  
        print the word  
        iterate over the counters:  
            for each (docID, count) pair:  
                print " docID count"  
        print a newline  
close the file

Note: this requires a nested iterator: the outer iterates over hashtable slots (words), the inner iterates over counters (docID/count pairs). We pass the FILE\* through a helper struct to both iterators.

Pseudocode for index\_load:

count the number of lines in indexFilename to size the hashtable  
open indexFilename for reading  
create a new index with num\_slots \= (line\_count \* 1.3) rounded to integer  
while fscanf reads a word successfully:  
    create a new counters object  
    while fscanf reads a docID and count pair:  
        set counter\[docID\] \= count  
    insert (word, counters) into the index  
close the file  
return the index

### **word**

We create a new module word.c in ../common with one function.

void normalizeWord(char\* word);

Pseudocode:

for each character in word:  
    word\[i\] \= tolower(word\[i\])

## **Function prototypes**

### **indexer**

int main(const int argc, char\* argv\[\]);  
static void parseArgs(const int argc, char\* argv\[\],  
                      char\*\* pageDirectory, char\*\* indexFilename);  
static index\_t\* indexBuild(const char\* pageDirectory);  
static void indexPage(index\_t\* index, webpage\_t\* page, const int docID);

### **pagedir (new functions)**

bool       pagedir\_validate(const char\* pageDirectory);  
webpage\_t\* pagedir\_load(const char\* pageDirectory, const int docID);

### **index**

index\_t\*    index\_new(const int num\_slots);  
void        index\_delete(index\_t\* index);  
bool        index\_insert(index\_t\* index, const char\* word, counters\_t\* ctrs);  
counters\_t\* index\_find(index\_t\* index, const char\* word);  
void        index\_save(index\_t\* index, const char\* indexFilename);  
index\_t\*    index\_load(const char\* indexFilename);

### **word**

void normalizeWord(char\* word);

## **Error handling and recovery**

All command-line parameters are rigorously checked before any data structures are allocated or work begins; problems result in a message printed to stderr and a non-zero exit status.

Out-of-memory errors are handled by mem\_assert variants, which print to stderr and exit non-zero. We anticipate these to be rare and allow the program to exit cleanly.

All functions check pointer parameters for NULL and take safe action (return NULL, return false, or log to stderr).

Specific recoverable errors:

* pagedir\_validate returns false (rather than exiting) so parseArgs can decide to exit with a message.  
* pagedir\_load returns NULL when a document file cannot be opened, which signals indexBuild to stop looping: this is the normal loop-termination condition.  
* index\_save prints an error and returns (rather than crashing) if the output file cannot be opened.

## **Testing plan**

### **Unit testing**

**index module:** The program indextest.c serves as a unit test for index\_load and index\_save. It loads an index file into memory and writes it back out to a new file. Running indexcmp on the two files verifies they are equivalent.

**word module:** Tested implicitly via the indexer; normalizeWord is called on every word processed.

**pagedir extensions:** Tested implicitly via the indexer; pagedir\_validate is called during argument parsing, and pagedir\_load is called in every iteration of indexBuild.

### **Integration/system testing**

We write a script testing.sh that invokes the indexer several times with a variety of arguments.

**Error-case tests** (each should exit non-zero with a message to stderr):

1. No arguments  
2. One argument  
3. Three or more arguments  
4. Invalid pageDirectory (non-existent path)  
5. Invalid pageDirectory (exists but not a Crawler directory: no .crawler file)  
6. Invalid indexFilename (non-existent directory path)  
7. Invalid indexFilename (read-only directory)

**Correctness tests:**

1. Run indexer on /cs50/shared/tse/output/letters-depth-1 and write to a local index file.  
2. Run indextest on that index file to produce a second index file.  
3. Run /cs50/shared/tse/indexcmp on the two index files to verify equivalence.  
4. Compare our index file against /cs50/shared/tse/output/letters-index-1 using indexcmp.  
5. Repeat steps 1–4 on toscrape-depth-1 and wikipedia-depth-1.

**Memory testing:**

Run valgrind on both indexer and indextest on a moderate test case (e.g., letters-depth-2) and confirm no memory errors or leaks are reported.

Run the script with bash \-v testing.sh so commands and output are interleaved in testing.out.

