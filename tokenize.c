
#include "tokenize.h"
#include "utility.h"

#include <stdlib.h>
#include <stdio.h>

#include <string.h>
#include <ctype.h>

char *next_token(char **str)
{
    if (str == NULL || *str == NULL) return NULL;
    char *start = drop_leading_whitespace(*str);
    if (*start == '\0') return NULL;
    if (isDelim(start)) {
        *str = start + 1;
        return copy_nstring(start, 1);
    } else {
        *str = find_next(start, isDelim);
        return copy_nstring(start, (*str - start));
    }
}

char *isDelim(char *c)
{
    if (c == NULL) return NULL;
    for (const char *tmp = DELIMS; *tmp != '\0'; ++tmp) {
        if (*tmp == *c) return c;
    }
    for (const char *tmp = operators; *tmp != '\0'; ++tmp) {
        if (*tmp == *c) return c;
    }
    return false;
}

