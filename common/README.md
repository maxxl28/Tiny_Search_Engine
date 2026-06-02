# CS50 Lab 4
## CS50 Spring 2026

### common

This directory just has files that are used across the other three (crawler, indexer, querier).
It compiles into a library `common.a` .

#### pagedir

The pagedir module handles reading and writing the page directory used by the crawler.

`pagedir_init(pageDirectory)` marks a directory as crawler-produced by creating a `.crawler` file inside it. Returns true on success, false if the directory is not writable.

`pagedir_save(page, pageDirectory, docID)` saves a fetched webpage to `pageDirectory/docID`, writing the URL, depth, and raw HTML on successive lines.
