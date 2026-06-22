#include "search.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint64_t search_space_size(size_t charset_size, size_t length, int *overflow)
{
    uint64_t result;
    size_t i;

    if (overflow != NULL) {
        *overflow = 0;
    }

    if (charset_size == 0U || length == 0U) {
        return 0U;
    }

    result = 1U;

    for (i = 0U; i < length; i++) {
        if (result > UINT64_MAX / (uint64_t)charset_size) {
            if (overflow != NULL) {
                *overflow = 1;
            }
            return 0U;
        }

        result *= (uint64_t)charset_size;
    }

    return result;
}

int index_to_candidate(uint64_t index,
                       const char *charset,
                       size_t charset_size,
                       size_t length,
                       char *out,
                       size_t out_size)
{
    size_t pos;

    if (charset == NULL || out == NULL || charset_size == 0U || length == 0U) {
        return -1;
    }

    if (out_size < length + 1U) {
        return -1;
    }

    out[length] = '\0';

    for (pos = length; pos > 0U; pos--) {
        uint64_t digit;

        digit = index % (uint64_t)charset_size;
        out[pos - 1U] = charset[digit];
        index /= (uint64_t)charset_size;
    }

    if (index != 0U) {
        return -1;
    }

    return 0;
}

int search_sequential(const char *target,
                      const char *charset,
                      size_t charset_size,
                      size_t length,
                      uint64_t total_space,
                      int verbose,
                      uint64_t *found_index,
                      uint64_t *attempts)
{
    char *candidate;
    uint64_t i;

    if (target == NULL || charset == NULL || found_index == NULL || attempts == NULL) {
        return -1;
    }

    if (charset_size == 0U || length == 0U || total_space == 0U) {
        return -1;
    }

    candidate = malloc(length + 1U);
    if (candidate == NULL) {
        return -1;
    }

    *attempts = 0U;
    *found_index = 0U;

    for (i = 0U; i < total_space; i++) {
        if (index_to_candidate(i, charset, charset_size, length, candidate, length + 1U) != 0) {
            free(candidate);
            return -1;
        }

        (*attempts)++;

        if (verbose) {
            printf("[single-thread] attempt=%" PRIu64 " index=%" PRIu64 " candidate=%s\n",
                   *attempts,
                   i,
                   candidate);
        }

        if (strcmp(candidate, target) == 0) {
            *found_index = i;
            free(candidate);
            return 1;
        }
    }

    free(candidate);
    return 0;
}
