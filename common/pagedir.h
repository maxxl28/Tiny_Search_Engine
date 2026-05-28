#ifndef __PAGEDIR_H
#define __PAGEDIR_H

#include <stdbool.h>
#include "../libcs50/webpage.h"

/**************** pagedir_init ****************/
/* Mark a directory as a Crawler-produced directory by creating a file
 * named .crawler inside it.
 * Returns true if successful, false otherwise.
 */
bool pagedir_init(const char* pageDirectory);

/**************** pagedir_save ****************/
/* Save a webpage to a file in pageDirectory.
 * Caller provides: valid pointer to a webpage, valid pageDirectory, valid docID.
 * We do: write URL, depth, and HTML to pageDirectory/docID.
 */
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);

/**************** pagedir_validate ****************/
/* Verify that pageDirectory is a Crawler-produced directory.
 * Returns true if pageDirectory/.crawler exists, false otherwise.
 */
bool pagedir_validate(const char* pageDirectory);

/**************** pagedir_load ****************/
/* Load a webpage from a file in pageDirectory.
 * Returns a heap-allocated webpage_t*, or NULL if file doesn't exist.
 * Caller must webpage_delete the result.
 */
webpage_t* pagedir_load(const char* pageDirectory, const int docID);

#endif // __PAGEDIR_H