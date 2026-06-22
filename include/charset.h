#ifndef CHARSET_H
#define CHARSET_H

#include <stddef.h>

const char *charset_for_mode(int mode);
size_t charset_len(const char *charset);
int charset_contains_char(const char *charset, char c);
int charset_validate_string(const char *charset, const char *text);

#endif
