#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../libcs50/webpage.h"
#include "pagedir.h"
#include "../libcs50/file.h"

bool 
pagedir_init(const char* pageDirectory)
{
    // construct the pathname for the crawler
    char filepath[strlen(pageDirectory) + strlen("/.crawler") + 1];
    sprintf(filepath, "%s/.crawler", pageDirectory);

    FILE* fp = fopen(filepath, "w");
    if (fp == NULL) {
        return false;
    }

    fclose(fp);
    return true;
}

void 
pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID)
{
    char filepath[strlen(pageDirectory) + 100];
    sprintf(filepath, "%s/%d", pageDirectory, docID);
    FILE* fp = fopen(filepath, "w");
    if (fp == NULL) {
        fprintf(stderr, "Cannot save page: %s\n", filepath);
        exit(1);
    }
    fprintf(fp, "%s\n%d\n%s", webpage_getURL(page), webpage_getDepth(page), webpage_getHTML(page));
    fclose(fp);
}
