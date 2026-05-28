/* index.c - CS50 TSE index module
 *
 * Implements an inverted index mapping words to (docID, count) counters.
 * Peter Owolabi, Spring 2026
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../libcs50/hashtable.h"
#include "../libcs50/counters.h"
#include "../libcs50/file.h"
#include "../libcs50/mem.h"
#include "index.h"

/**************** local types ****************/
struct index {
    hashtable_t* ht;
};

// helper struct to pass FILE* through iterators
typedef struct {
    FILE* fp;
} file_arg_t;

/**************** helper functions ****************/
static void counters_save_helper(void* arg, const int key, const int count)
{
    FILE* fp = arg;
    fprintf(fp, " %d %d", key, count);
}

static void index_save_helper(void* arg, const char* key, void* item)
{
    FILE* fp = arg;
    counters_t* ctrs = item;
    fprintf(fp, "%s", key);
    counters_iterate(ctrs, fp, counters_save_helper);
    fprintf(fp, "\n");
}

static void index_delete_helper(void* item)
{
    counters_delete((counters_t*)item);
}

/**************** index_new ****************/
index_t*
index_new(const int num_slots)
{
    index_t* index = mem_malloc(sizeof(index_t));
    if (index == NULL) return NULL;
    index->ht = hashtable_new(num_slots);
    if (index->ht == NULL) {
        mem_free(index);
        return NULL;
    }
    return index;
}

/**************** index_delete ****************/
void
index_delete(index_t* index)
{
    if (index == NULL) return;
    hashtable_delete(index->ht, index_delete_helper);
    mem_free(index);
}

/**************** index_insert ****************/
bool
index_insert(index_t* index, const char* word, counters_t* ctrs)
{
    if (index == NULL || word == NULL || ctrs == NULL) return false;
    return hashtable_insert(index->ht, word, ctrs);
}

/**************** index_find ****************/
counters_t*
index_find(index_t* index, const char* word)
{
    if (index == NULL || word == NULL) return NULL;
    return hashtable_find(index->ht, word);
}

/**************** index_save ****************/
void
index_save(index_t* index, const char* indexFilename)
{
    if (index == NULL || indexFilename == NULL) return;

    FILE* fp = fopen(indexFilename, "w");
    if (fp == NULL) {
        fprintf(stderr, "Error: cannot open '%s' for writing\n", indexFilename);
        return;
    }
    hashtable_iterate(index->ht, fp, index_save_helper);
    fclose(fp);
}

/**************** index_load ****************/
index_t*
index_load(const char* indexFilename)
{
    if (indexFilename == NULL) return NULL;

    FILE* fp = fopen(indexFilename, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error: cannot open '%s' for reading\n", indexFilename);
        return NULL;
    }

    // count lines to size the hashtable
    int num_lines = file_numLines(fp);
    rewind(fp);

    index_t* index = index_new(num_lines * 1.3 + 1);
    if (index == NULL) { fclose(fp); return NULL; }

    char* word;
    while ((word = file_readWord(fp)) != NULL) {
        counters_t* ctrs = counters_new();
        int docID, count;
        while (fscanf(fp, " %d %d", &docID, &count) == 2) {
            counters_set(ctrs, docID, count);
        }
        index_insert(index, word, ctrs);
        free(word);
    }

    fclose(fp);
    return index;
}
