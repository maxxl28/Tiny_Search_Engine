#ifndef __INDEX_H
#define __INDEX_H

#include <stdio.h>
#include <stdbool.h>
#include "../libcs50/hashtable.h"
#include "../libcs50/counters.h"

typedef struct index index_t;

/**************** index_new ****************/
/* Create a new index with the given number of hashtable slots.
 * Returns a new index, or NULL on error.
 */
index_t* index_new(const int num_slots);

/**************** index_delete ****************/
/* Delete the index and free all memory. */
void index_delete(index_t* index);

/**************** index_insert ****************/
/* Insert a counters object for the given word.
 * Returns true if successful, false otherwise.
 */
bool index_insert(index_t* index, const char* word, counters_t* ctrs);

/**************** index_find ****************/
/* Return the counters object for the given word, or NULL if not found. */
counters_t* index_find(index_t* index, const char* word);

/**************** index_save ****************/
/* Write the index to a file in the format:
 *   word docID count [docID count]...
 */
void index_save(index_t* index, const char* indexFilename);

/**************** index_load ****************/
/* Load an index from a file produced by index_save.
 * Returns a new index, or NULL on error.
 */
index_t* index_load(const char* indexFilename);

#endif // __INDEX_H
