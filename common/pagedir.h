#ifndef __PAGEDIR_H
#define __PAGEDIR_H

#include <stdbool.h>
#include "../libcs50/webpage.h"

/**************** pagedir_init ****************/
/* Mark a directory as a Crawler-produced directory by creating a file named .crawler inside it.
 * Caller provides: a valid pathname string for pageDirectory
 * We return: true if successful and false otherwise
 */
bool pagedir_init(const char* pageDirectory);

/**************** pagedir_save ****************/
/* Save a webpage to a file in pageDirectory.
 * 
 * Caller provides: valid pointer to a webpage, valid pageDirectory, valid docID 
 * We do: write URL, depth, and HTML to pageDirectory/docID
 * We guarantee: nothing if any error opening the file
 */
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);

#endif // __PAGEDIR_H
