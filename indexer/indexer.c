/* indexer.c - CS50 TSE Indexer
 *
 * Reads pages saved by the crawler, builds an inverted index,
 * and writes it to a file.
 *
 * Usage: ./indexer pageDirectory indexFilename
 *
 * Peter Owolabi, Spring 2026
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../libcs50/webpage.h"
#include "../libcs50/mem.h"
#include "../libcs50/counters.h"
#include "../common/index.h"
#include "../common/pagedir.h"
#include "../common/word.h"

/**************** function prototypes ****************/
static void parseArgs(const int argc, char* argv[],
                      char** pageDirectory, char** indexFilename);
static index_t* indexBuild(const char* pageDirectory);
static void indexPage(index_t* index, webpage_t* page, const int docID);

/**************** main ****************/
int main(const int argc, char* argv[])
{
    char* pageDirectory = NULL;
    char* indexFilename = NULL;

    parseArgs(argc, argv, &pageDirectory, &indexFilename);

    index_t* index = indexBuild(pageDirectory);
    index_save(index, indexFilename);
    index_delete(index);

    return 0;
}

/**************** parseArgs ****************/
/* Validate command-line arguments.
 * Exits non-zero on any error.
 */
static void parseArgs(const int argc, char* argv[],
                      char** pageDirectory, char** indexFilename)
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s pageDirectory indexFilename\n", argv[0]);
        exit(1);
    }

    *pageDirectory = argv[1];
    *indexFilename = argv[2];

    // Verify pageDirectory is a crawler-produced directory
    if (!pagedir_validate(*pageDirectory)) {
        fprintf(stderr, "Error: '%s' is not a valid crawler directory\n",
                *pageDirectory);
        exit(1);
    }

    // Verify we can write to indexFilename
    FILE* fp = fopen(*indexFilename, "w");
    if (fp == NULL) {
        fprintf(stderr, "Error: cannot open '%s' for writing\n", *indexFilename);
        exit(1);
    }
    fclose(fp);
}

/**************** indexBuild ****************/
/* Build an index by reading all crawler-output pages from pageDirectory.
 * Returns a newly-allocated index; caller must index_delete it.
 */
static index_t* indexBuild(const char* pageDirectory)
{
    index_t* index = index_new(500);
    int docID = 1;
    webpage_t* page;

    while ((page = pagedir_load(pageDirectory, docID)) != NULL) {
        indexPage(index, page, docID);
        webpage_delete(page);
        docID++;
    }

    return index;
}

/**************** indexPage ****************/
/* Scan one webpage and add each word (length >= 3) to the index. */
static void indexPage(index_t* index, webpage_t* page, const int docID)
{
    int pos = 0;
    char* word;

    while ((word = webpage_getNextWord(page, &pos)) != NULL) {
        if (strlen(word) >= 3) {
            normalizeWord(word);

            counters_t* ctrs = index_find(index, word);
            if (ctrs == NULL) {
                ctrs = counters_new();
                index_insert(index, word, ctrs);
            }
            counters_add(ctrs, docID);
        }
        free(word);
    }
}