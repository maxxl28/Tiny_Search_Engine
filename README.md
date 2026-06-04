# Tiny Search Engine (TSE)

## Overview

The Tiny Search Engine consists of three subsystems:

1. Crawler — crawls a website starting from a seed URL, downloading pages up to a given depth and saving them to a directory.
2. Indexer — reads the pages produced by the crawler, builds a mapping words to (docID, count) pairs, and writes it to a file.
3. Querier — reads the index file and answers search queries from stdin,also supports both And and Or

## Building
make:
Builds all three subsystems and their shared libraries (`libcs50` and `common`).

make clear: Removes all compiled files.

## Testing

Each subsystem has its own `testing.sh`. From any subdirectory just type make test
