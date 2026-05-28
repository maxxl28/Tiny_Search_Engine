/* word.c - CS50 TSE word module
 * Implements normalizeWord for the Indexer and Querier.
 * Peter Owolabi, Spring 2026
 */

#include <ctype.h>
#include <stddef.h>
#include "word.h"

void
normalizeWord(char* word)
{
    if (word == NULL) return;
    for (int i = 0; word[i] != '\0'; i++) {
        word[i] = tolower(word[i]);
    }
}
