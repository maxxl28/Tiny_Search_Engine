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

bool
pagedir_validate(const char* pageDirectory)
{
    if (pageDirectory == NULL) return false;

    char filepath[strlen(pageDirectory) + strlen("/.crawler") + 1];
    sprintf(filepath, "%s/.crawler", pageDirectory);

    FILE* fp = fopen(filepath, "r");
    if (fp == NULL) {
        return false;
    }
    fclose(fp);
    return true;
}

webpage_t*
pagedir_load(const char* pageDirectory, const int docID)
{
    if (pageDirectory == NULL) return NULL;

    // build path like pageDirectory/1
    char filepath[strlen(pageDirectory) + 20];
    sprintf(filepath, "%s/%d", pageDirectory, docID);

    FILE* fp = fopen(filepath, "r");
    if (fp == NULL) {
        return NULL;  // no more documents — normal loop termination
    }

    // read URL (first line)
    char* url = file_readLine(fp);
    if (url == NULL) { fclose(fp); return NULL; }

    // read depth (second line)
    char* depthStr = file_readLine(fp);
    if (depthStr == NULL) { free(url); fclose(fp); return NULL; }
    int depth = atoi(depthStr);
    free(depthStr);

    // read HTML (rest of file)
    char* html = file_readFile(fp);
    fclose(fp);

    webpage_t* page = webpage_new(url, depth, html);
    return page;
}