#ifndef SEARCH_H
#define SEARCH_H

#include <stddef.h>
#include <stdint.h>

uint64_t search_space_size(size_t charset_size, size_t length, int *overflow);

#endif
