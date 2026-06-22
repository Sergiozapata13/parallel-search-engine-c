#include "search.h"

#include <stdint.h>

uint64_t search_space_size(size_t charset_size, size_t length, int *overflow)
{
    uint64_t result;
    size_t i;

    if (overflow != 0) {
        *overflow = 0;
    }

    if (charset_size == 0U || length == 0U) {
        return 0U;
    }

    result = 1U;

    for (i = 0U; i < length; i++) {
        if (result > UINT64_MAX / (uint64_t)charset_size) {
            if (overflow != 0) {
                *overflow = 1;
            }
            return 0U;
        }

        result *= (uint64_t)charset_size;
    }

    return result;
}
