#include "charset.h"

#include <string.h>

const char *charset_for_mode(int mode)
{
    static const char numeric[] =
        "0123456789";

    static const char numeric_lower[] =
        "0123456789abcdefghijklmnopqrstuvwxyz";

    static const char numeric_lower_upper[] =
        "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

    static const char numeric_lower_upper_special[] =
        "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "!@#$%^&*()-_=+[]{};:,.<>/";

    switch (mode) {
    case 1:
        return numeric;
    case 2:
        return numeric_lower;
    case 3:
        return numeric_lower_upper;
    case 4:
        return numeric_lower_upper_special;
    default:
        return NULL;
    }
}

size_t charset_len(const char *charset)
{
    if (charset == NULL) {
        return 0U;
    }

    return strlen(charset);
}

int charset_contains_char(const char *charset, char c)
{
    if (charset == NULL) {
        return 0;
    }

    return strchr(charset, c) != NULL;
}

int charset_validate_string(const char *charset, const char *text)
{
    size_t i;

    if (charset == NULL || text == NULL) {
        return 0;
    }

    for (i = 0U; text[i] != '\0'; i++) {
        if (!charset_contains_char(charset, text[i])) {
            return 0;
        }
    }

    return 1;
}
