/* indextest.c - CS50 TSE index tester
 *
 * Loads an index from oldIndexFilename, saves it to newIndexFilename.
 * Verifies that index_load and index_save work correctly.
 *
 * Usage: ./indextest oldIndexFilename newIndexFilename
 *
 * Peter Owolabi, Spring 2026
 */

#include <stdio.h>
#include <stdlib.h>
#include "../common/index.h"

int main(const int argc, char* argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s oldIndexFilename newIndexFilename\n", argv[0]);
        exit(1);
    }

    const char* oldIndexFilename = argv[1];
    const char* newIndexFilename = argv[2];

    index_t* index = index_load(oldIndexFilename);
    if (index == NULL) {
        fprintf(stderr, "Error: could not load index from '%s'\n", oldIndexFilename);
        exit(1);
    }

    index_save(index, newIndexFilename);
    index_delete(index);

    return 0;
}
