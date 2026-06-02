#ifndef __PAGEDIR_H
#define __PAGEDIR_H

#include <stdbool.h>
#include "../libcs50/webpage.h"

/* Mark a directory as a Crawler-produced directory.
 * Caller provides: a valid pathname string for pageDirectory
 * We return: true if successful, false otherwise
 */
bool pagedir_init(const char* pageDirectory);

/* Save a webpage to a file in pageDirectory.
 * Caller provides: valid pointer to a webpage, valid pageDirectory, valid docID
 * We do: write URL, depth, and HTML to pageDirectory/docID
 * We guarantee: nothing if any error opening the file
 */
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);

/* Verify that directory is a Crawler-produced directory.
 * Caller provides: valid pathname string for pageDirectory
 * We return: true if .crawler file exists, false otherwise
 */
bool pagedir_validate(const char* pageDirectory);

/* Load a webpage from a file in pageDirectory.
 * Caller provides: valid pageDirectory, valid docID >= 1
 * We return: pointer to a webpage_t, or NULL if file can't be opened
 */
webpage_t* pagedir_load(const char* pageDirectory, const int docID);

#endif // __PAGEDIR_H