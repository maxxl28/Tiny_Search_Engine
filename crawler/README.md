# CS50 Lab 4
## CS50 Spring 2026

### crawler

Crawls web pages starting from a seed URL, saving each fetched page to a numbered file in a given directory, up to a specified maximum depth.

#### Usage

```
./crawler seedURL pageDirectory maxDepth
```

- `seedURL` — the starting URL; must be internal to `http://cs50tse.cs.dartmouth.edu/tse/`
- `pageDirectory` — an existing directory where fetched pages will be saved
- `maxDepth` — crawl depth (0–10); depth 0 means only the seed page is fetched

#### Assumptions

- `pageDirectory` must already exist before the crawler is run.
- Only URLs internal to the CS50 TSE server are used

#### Known issues

None known.
