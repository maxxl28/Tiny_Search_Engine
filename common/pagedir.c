/*
 * pagedir.c Max Peter Ale May 2026
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../libcs50/webpage.h"
#include "pagedir.h"
#include "../libcs50/file.h"

/* see pagedir.h for description */
bool
pagedir_init(const char* pageDirectory)
{
  char filepath[strlen(pageDirectory) + strlen("/.crawler") + 1];
  sprintf(filepath, "%s/.crawler", pageDirectory);

  FILE* fp = fopen(filepath, "w");
  if (fp == NULL) {
    return false;
  }

  fclose(fp);
  return true;
}

/* see pagedir.h for description */
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

  fprintf(fp, "%s\n%d\n%s",
          webpage_getURL(page),
          webpage_getDepth(page),
          webpage_getHTML(page));
  fclose(fp);
}

/* see pagedir.h for description */
bool
pagedir_validate(const char* pageDirectory)
{
  char filepath[strlen(pageDirectory) + strlen("/.crawler") + 1];
  sprintf(filepath, "%s/.crawler", pageDirectory);

  FILE* fp = fopen(filepath, "r");
  if (fp == NULL) {
    return false;
  }

  fclose(fp);
  return true;
}

/* see pagedir.h for description */
webpage_t*
pagedir_load(const char* pageDirectory, const int docID)
{
  char filepath[strlen(pageDirectory) + 20];
  sprintf(filepath, "%s/%d", pageDirectory, docID);

  FILE* fp = fopen(filepath, "r");
  if (fp == NULL) {
    return NULL;
  }

  char* url = file_readLine(fp);
  char depthStr[20];
  fgets(depthStr, 20, fp);
  int depth = atoi(depthStr);
  char* html = file_readFile(fp);

  fclose(fp);
  return webpage_new(url, depth, html);
}