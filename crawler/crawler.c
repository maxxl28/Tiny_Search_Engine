
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../libcs50/bag.h"
#include "../libcs50/hashtable.h"
#include "../libcs50/webpage.h"
#include "../libcs50/mem.h"
#include "../common/pagedir.h"

static void parseArgs(const int argc, char* argv[],
                      char** seedURL, char** pageDirectory, int* maxDepth);
static void crawl(char* seedURL, char* pageDirectory, const int maxDepth);
static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen);

int
main(const int argc, char* argv[])
{
  char* seedURL = NULL;
  char* pageDirectory = NULL;
  int maxDepth = 0;

  parseArgs(argc, argv, &seedURL, &pageDirectory, &maxDepth);
  crawl(seedURL, pageDirectory, maxDepth);

  return 0;
}

/* Parse and validate command-line arguments.
 * Normalize seedURL and confirm it is internal.
 * Initialize pageDirectory. Validate maxDepth range.
 * Exit non-zero on any error.
 */
static 
void parseArgs(const int argc, char* argv[],
                      char** seedURL, char** pageDirectory, int* maxDepth)
{
    if (argc != 4) {
        fprintf(stderr, "Usage: ./crawler seedURL pageDirectory maxDepth\n");
        exit(1);
    }

    // validate and normalize seedURL
    *seedURL = normalizeURL(argv[1]);
    if (*seedURL == NULL) {
        fprintf(stderr, "Error: seedURL could not be normalized\n");
        exit(1);
    }
    if (!isInternalURL(*seedURL)) {
        fprintf(stderr, "Error: seedURL is not internal\n");
        free(*seedURL);
        exit(1);
    }
    // validate pageDirectory
    *pageDirectory = argv[2];
    if (!pagedir_init(*pageDirectory)) {
        fprintf(stderr, "Error: cannot initialize pageDirectory: %s\n", *pageDirectory);
        exit(1);
    }


    // validate maxDepth
    char excess;
    if (sscanf(argv[3], "%d%c", maxDepth, &excess) != 1) {
        fprintf(stderr, "Error: maxDepth must be an integer\n");
        exit(1);
    }
    if (*maxDepth < 0 || *maxDepth > 10) {
        fprintf(stderr, "Error: maxDepth must be between 0 and 10\n");
        exit(1);
    }
}

/* Crawl from seedURL to maxDepth, saving pages in pageDirectory.
 * Uses a bag of pages to crawl and a hashtable of seen URLs.
 */
static 
void crawl(char* seedURL, char* pageDirectory, const int maxDepth)
{
  hashtable_t* pagesSeen = hashtable_new(200);
  hashtable_insert(pagesSeen, seedURL, "");
  bag_t* webPages = bag_new();
  webpage_t* seedPage = webpage_new(seedURL, 0, NULL);
  bag_insert(webPages, seedPage);
  int docID = 1;
  webpage_t* page;

  
  while ((page = bag_extract(webPages)) != NULL) {
    if (webpage_fetch(page)) {
      pagedir_save(page, pageDirectory, docID);
      docID++;
      if (webpage_getDepth(page) < maxDepth) {
          pageScan(page, webPages, pagesSeen);
      }
    }
    webpage_delete(page);
  }
  hashtable_delete(pagesSeen, NULL);
  bag_delete(webPages, webpage_delete); 
}

/* Scan the given page for URLs. For each internal URL not yet seen,
 * add it to both the hashtable and the bag.
 */
static 
void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen)
{
  int pos = 0;
  char* url;  
  while ((url = webpage_getNextURL(page, &pos)) != NULL) {
    if (isInternalURL(url)) {
      if (hashtable_insert(pagesSeen, url, "")) {
        webpage_t* newPage = webpage_new(url, webpage_getDepth(page) + 1, NULL);
        bag_insert(pagesToCrawl, newPage);
      } else {
        free(url);
      }
    }
    else {
      free(url);
    }
  }
}