#ifndef SEARCH_H
#define SEARCH_H

#include <stddef.h>
#include <stdint.h>

uint64_t search_space_size(size_t charset_size, size_t length, int *overflow);

int index_to_candidate(uint64_t index,
                       const char *charset,
                       size_t charset_size,
                       size_t length,
                       char *out,
                       size_t out_size);

int search_sequential(const char *target,
                      const char *charset,
                      size_t charset_size,
                      size_t length,
                      uint64_t total_space,
                      int verbose,
                      uint64_t *found_index,
                      uint64_t *attempts);

#endif
