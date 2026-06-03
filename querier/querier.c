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


int
main(const int argc, char* argv[])
{
  char* pageDirectory = NULL;
  char* indexFilename = NULL;
  parseArgs(argc, argv, &pageDirectory, &indexFilename);
  index_t* index = index_load(indexFilename);

  // process queries one line at a time
  char* line;
  while (prompt(), (line = file_readLine(stdin)) != NULL) {
    char** wordArray = NULL;
    int numWords = 0;
    if (!tokenizeQuery(line, &wordArray, &numWords)) {
      free(line);
      continue;
    }
    if (numWords == 0) {
      free(wordArray);
      free(line);
      continue;
    }
    if (!validateQuery(wordArray, numWords)) {
      for (int i = 0; i < numWords; i++) {
        free(wordArray[i]);
      }
      free(wordArray);
      free(line);
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
  }
  index_delete(index);
  return 0;
}


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




static bool
tokenizeQuery(const char* line, char*** wordArray, int* numWords)
{
  // upper bound is max possible amount of words
  int maxWords = strlen(line) / 2 + 1;
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


static bool
validateQuery(char** wordArray, const int numWords)
{
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


static counters_t*
andSequence(char** wordArray, int* i, index_t* index)
{
  counters_t* result = counters_new();
  counters_t* first = index_find(index, wordArray[*i]);
  if (first != NULL) {
    unionCounters(result, first);
  }
  (*i)++;
  // keep intersecting while we see more words 
  while (wordArray[*i] != NULL && strcmp(wordArray[*i], "or") != 0) {
    // skip and
    if (strcmp(wordArray[*i], "and") == 0) {
      (*i)++;
      continue;
    }

    // intersect with next word's counters
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

static void
intersectCounters(counters_t* result, counters_t* other)
{
}


static void
intersectHelper(void* arg, const int key, const int count)
{
}


static void
unionCounters(counters_t* dest, counters_t* src)
{
}


static void
unionHelper(void* arg, const int key, const int count)
{
}


static void
rankAndPrint(counters_t* result, const char* pageDirectory)
{
}


static void
rankHelper(void* arg, const int key, const int count)
{
}


static void
countHelper(void* arg, const int key, const int count)
{
}


static void
prompt(void)
{
  if (isatty(fileno(stdin))) {
    printf("What is your TSE query? ");
  }
}