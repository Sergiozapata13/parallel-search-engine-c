#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    int found;
    int winning_thread;
    uint64_t found_index;
    uint64_t attempts;
} ParallelSearchResult;

long scheduler_detect_logical_processors(void);

int scheduler_parallel_search(const char *target,
                              const char *charset,
                              size_t charset_size,
                              size_t length,
                              uint64_t total_space,
                              uint64_t chunk_size,
                              int verbose,
                              long thread_count,
                              ParallelSearchResult *result);

#endif
