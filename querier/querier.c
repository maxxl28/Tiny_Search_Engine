/*
 * querier.c    Max, Ale    June 2026
 *
 * This file implements the TSE Querier. It reads an index file produced by the Indexer, then answers
 * search queries from stdin.
 * Queries support AND and OR operators with AND taking precedence.
 * Results are printed in decreasing order by score.
 * Usage: ./querier pageDirectory indexFilename
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "../libcs50/counters.h"
#include "../libcs50/file.h"
#include "../libcs50/mem.h"
#include "../common/index.h"
#include "../common/pagedir.h"
#include "../common/word.h"

// helper structs
typedef struct two_counters {
  counters_t* result;
  counters_t* other;
} two_counters_t;

typedef struct max_doc {
  int maxKey;
  int maxCount;
} max_doc_t;


static void parseArgs(const int argc, char* argv[],
                      char** pageDirectory, char** indexFilename);
static bool tokenizeQuery(const char* line, char*** wordArray, int* numWords);
static bool validateQuery(char** wordArray, const int numWords);
static counters_t* runQuery(char** wordArray, const int numWords, index_t* index);
static counters_t* andSequence(char** wordArray, int* i, index_t* index);
static void intersectCounters(counters_t* result, counters_t* other);
static void intersectHelper(void* arg, const int key, const int count);
static void unionCounters(counters_t* dest, counters_t* src);
static void unionHelper(void* arg, const int key, const int count);
static void rankAndPrint(counters_t* result, const char* pageDirectory);
static void rankHelper(void* arg, const int key, const int count);
static void countHelper(void* arg, const int key, const int count);
static void prompt(void);
int fileno(FILE* stream);

//runs the whole process
int
main(const int argc, char* argv[])
{
  // this needs to 
  char* pageDirectory = NULL;
  char* indexFilename = NULL;
  parseArgs(argc, argv, &pageDirectory, &indexFilename);

  index_t* index = index_load(indexFilename);
  if (index == NULL){
    fprintf(stderr, "index is null");
    exit(1);
  }

  // process queries one line at a time
  char* line;
  prompt();
  while ((line = file_readLine(stdin)) != NULL) {
    char** wordArray = NULL;
    int numWords = 0;
    if (!tokenizeQuery(line, &wordArray, &numWords)) {
      free(line);
      prompt();
      continue;
    }
    if (numWords == 0) {
      free(wordArray);
      free(line);
      prompt();
      continue;
    }
    if (!validateQuery(wordArray, numWords)) {
      for (int i = 0; i < numWords; i++) {
        free(wordArray[i]);
      }
      free(wordArray);
      free(line);
      prompt();
      continue;
    }
    printf("Query:");
    for (int i = 0; i < numWords; i++) {
      printf(" %s", wordArray[i]);
    }
    printf("\n");
    counters_t* result = runQuery(wordArray, numWords, index);
    rankAndPrint(result, pageDirectory);

    // cleanup free memmory
    counters_delete(result);
    for (int i = 0; i < numWords; i++) {
      free(wordArray[i]);
    }
    free(wordArray);
    free(line);
    prompt();
  }
  index_delete(index);
  return 0;
}

/* parseArgs: validate and extract command-line arguments
 * Caller provides: argc, argv, pointers to pageDirectory and indexFilename
 * We do: verify pageDirectory is a crawler directory, verify indexFilename is readable
 * We return: only if all arguments are valid; exit non-zero otherwise
 */
static void
parseArgs(const int argc, char* argv[],
          char** pageDirectory, char** indexFilename)
{
  if (argc != 3) {
    fprintf(stderr, "Usage: ./querier pageDirectory indexFilename\n");
    exit(1);
  }

  *pageDirectory = argv[1];
  if (!pagedir_validate(*pageDirectory)) {
    fprintf(stderr, "Error: invalid pageDirectory: %s\n", *pageDirectory);
    exit(1);
  }
   *indexFilename = argv[2];
  FILE* fp = fopen(*indexFilename, "r");
  if (fp == NULL) {
    fprintf(stderr, "Error: cannot read indexFilename: %s\n", *indexFilename);
    exit(1);
  }
  fclose(fp);
}


/* tokenizeQuery: split a query line into an array of normalized words
 * Caller provides: valid line, pointer to wordArray, pointer to numWords
 * We return: true on success, false if any non-alpha non-space character found
 * Caller should free memory
 */
static bool
tokenizeQuery(const char* line, char*** wordArray, int* numWords)
{
  //check everything is there
  if (line == NULL || wordArray == NULL || numWords == NULL){
    return false;
  }
  // upper bound is max possible amount of words but just to be safe we make it larger
  int maxWords = strlen(line) +1;
  *wordArray = mem_malloc(maxWords * sizeof(char*));
  int i = 0;
  const char* p = line;
  while (*p != '\0') {
    // skip spaces
    if (isspace(*p)) {
      p++;
      continue;
    }
    // check invalid character in query
    if (!isalpha(*p)) {
      fprintf(stderr, "Error: invalid character in query\n");
      // free everything allocated so far
      for (int j = 0; j < i; j++) {
        free((*wordArray)[j]);
      }
      free(*wordArray);
      *wordArray = NULL;
      *numWords = 0;
      return false;
    }
    const char* start = p;
    while (isalpha(*p)) { 
      p++;
    }
    // copy word into new string
    int wordLen = p - start;
    char* word = mem_malloc(wordLen + 1);
    strncpy(word, start, wordLen);
    word[wordLen] = '\0';
    // normalize and store
    normalizeWord(word);
    (*wordArray)[i++] = word;
  }
  (*wordArray)[i] = NULL;
  *numWords = i;
  return true;
}


/* validateQuery: check that operators are not first, last, or adjacent
 * Caller provides: valid wordArray, numWords >= 1
 * We return: true if query structure is valid, false with error message otherwise
 */
static bool
validateQuery(char** wordArray, const int numWords)
{
  //check everything is there
  if (wordArray == NULL || numWords <= 0){
    return false;
  }
  // check first word
  if (strcmp(wordArray[0], "and") == 0 || strcmp(wordArray[0], "or") == 0) {
    fprintf(stderr, "Error: '%s' cannot be first\n", wordArray[0]);
    return false;
  }
  // check last word
  if (strcmp(wordArray[numWords-1], "and") == 0 || 
      strcmp(wordArray[numWords-1], "or") == 0) {
    fprintf(stderr, "Error: '%s' cannot be last\n", wordArray[numWords-1]);
    return false;
  }
  // check adjacent operators: logic: check if previous and curr 
  // are operators then return error if true
  for (int i = 1; i < numWords; i++) {
    bool currIsOp = (strcmp(wordArray[i], "and") == 0 || strcmp(wordArray[i], "or") == 0);
    bool prevIsOp = (strcmp(wordArray[i-1], "and") == 0 || strcmp(wordArray[i-1], "or") == 0);
    if (currIsOp && prevIsOp) {
      fprintf(stderr, "Error: '%s' and '%s' cannot be adjacent\n", wordArray[i-1], wordArray[i]);
      return false;
    }
  }
  return true;
}

/* runQuery: execute a query against the index
 * Caller provides: valid wordArray, numWords, loaded index
 * We return: heap-allocated counters mapping docID to score
 * Caller must free memory
 */
static counters_t*
runQuery(char** wordArray, const int numWords, index_t* index)
{
  counters_t* total = NULL;
  int i = 0;
  while (i < numWords) {
    counters_t* product = andSequence(wordArray, &i, index);
    if (total == NULL) {
      total = product;
    } else {
      unionCounters(total, product);
      counters_delete(product);
    }
    if (i < numWords && strcmp(wordArray[i], "or") == 0) {
      i++;
    } else {
      break;
    }
  }
  return total;
}

/* andSequence: process one AND sequence from the word array
 * Caller provides: valid wordArray, pointer to current index i, loaded index
 * We return: heap-allocated counters for this andsequence
 */
static counters_t*
andSequence(char** wordArray, int* i, index_t* index)
{
  counters_t* result = counters_new();
  counters_t* first = index_find(index, wordArray[*i]);
  if (first != NULL) {
    unionCounters(result, first);
  }
  (*i)++;
  // keep intersecting while more words
  while (wordArray[*i] != NULL && strcmp(wordArray[*i], "or") != 0) {
    // skip and
    if (strcmp(wordArray[*i], "and") == 0) {
      (*i)++;
      continue;
    }
    // then we intersect with next word's counters
    counters_t* ctrs = index_find(index, wordArray[*i]);
    if (ctrs != NULL) {
      intersectCounters(result, ctrs);
    } else {
      counters_delete(result);
      result = counters_new();
    }
    (*i)++;
  }
  return result;
}

// modifies result in place: for each docID currently in result,
// set its score to min(result's score, other's score).
static void
intersectCounters(counters_t* result, counters_t* other)
{
  two_counters_t two = {result, other};
  counters_iterate(result, &two, intersectHelper);
}

// called once per (docID, count) in result.
// looks up the same docID in other, keeps the smaller score.
// This is the AND logic
static void
intersectHelper(void* arg, const int key, const int count)
{
  two_counters_t* two = arg;
  int otherCount = counters_get((*two).other, key);
  if (count < otherCount) {
    counters_set(two->result, key, count);
  } else {
    counters_set(two->result, key, otherCount);
  }
}

// modifies dest in place: for each docID in src,
// add src's count to whatever dest already has for that docID.
//this is the OR logic
static void
unionCounters(counters_t* dest, counters_t* src)
{
  counters_iterate(src, dest, unionHelper);
}

// called once per (docID, count) in src
// adds this count to dest's existing count for the same docID.
static void
unionHelper(void* arg, const int key, const int count)
{
  counters_t* dest = arg;
  int existing = counters_get(dest, key);
  counters_set(dest, key, existing + count);
}

// prints all matching documents in decreasing score order.
// we repeadtedly find the max, print it and then set it to 0
static void
rankAndPrint(counters_t* result, const char* pageDirectory)
{
  if (result == NULL) {
    printf("No documents match.\n");
    return;
  }
  // if all have count 0 then no docs match
  int matchCount = 0;
  counters_iterate(result, &matchCount, countHelper);
  if (matchCount == 0) {
    printf("All counts are zero so no docs match.\n");
    return;
  }
  printf("Matches %d documents (ranked):\n", matchCount);
  //each iteration finds the max, prints it, sets it to zero
  for (int n = 0; n < matchCount; n++) {
    max_doc_t max = {0, 0};
    counters_iterate(result, &max, rankHelper);
    if (max.maxCount == 0) break;
    char* filepath = mem_malloc(strlen(pageDirectory) + 20);
    sprintf(filepath, "%s/%d", pageDirectory, max.maxKey);
    // read the URL 
    FILE* fp = fopen(filepath, "r");
    char* url = NULL;
    if (fp != NULL) {
      url = file_readLine(fp);
      fclose(fp);
    }
    if (url != NULL) {
      printf("score %4d doc %4d: %s\n", max.maxCount, max.maxKey, url);
    } 
    else {
      printf("score %4d doc %4d: (unknown)\n", max.maxCount, max.maxKey);
    }
    free(url);
    free(filepath);
    counters_set(result, max.maxKey, 0);
  }
}

// called once per (docID, count) in result.
// updates max if this doc's score is higher than the best so far.
static void
rankHelper(void* arg, const int key, const int count)
{
  max_doc_t* max = arg;
  if (count > (*max).maxCount) {
    (*max).maxKey = key;
    (*max).maxCount = count;
  }
}
// called once per (docID, count) in result.
// increments the counter if this doc has a nonzero score.
static void
countHelper(void* arg, const int key, const int count)
{
  int* n = arg;
  if (count > 0) 
  {
    (*n)++;
  }
}
// prints Query? regardless of whether it is from stdin or from a file
static void
prompt(void)
{
  if (isatty(fileno(stdin))) {
    printf("Query? ");
  }
}
