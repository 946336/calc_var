#include "relop.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *EQUAL_S = "=";
static const char *NOT_EQUAL_S = "!=";
static const char *LESS_THAN_S = "<";
static const char *GREATER_THAN_S = ">";
static const char *LESS_THAN_OR_EQUAL_S = "<=";
static const char *GREATER_THAN_OR_EQUAL_S = ">=";

const char *RELOPtostring(RELOP r)
{
    switch (r) {
        case EQUAL:                 return EQUAL_S;
        case NOT_EQUAL:             return NOT_EQUAL_S;
        case LESS_THAN:             return LESS_THAN_S;
        case GREATER_THAN:          return GREATER_THAN_S;
        case LESS_THAN_OR_EQUAL:    return LESS_THAN_OR_EQUAL_S;
        case GREATER_THAN_OR_EQUAL: return GREATER_THAN_OR_EQUAL_S;
    }
    // Compiler dummy
    return NULL;
}

RELOP stringtoRELOP(char *str)
{
    if (!strcmp(str, EQUAL_S))
        return EQUAL;
    else if (!strcmp(str, NOT_EQUAL_S))
        return NOT_EQUAL;
    else if (!strcmp(str, LESS_THAN_S))
        return LESS_THAN;
    else if (!strcmp(str, GREATER_THAN_S))
        return GREATER_THAN;
    else if (!strcmp(str, LESS_THAN_OR_EQUAL_S))
        return LESS_THAN_OR_EQUAL;
    else if (!strcmp(str, GREATER_THAN_OR_EQUAL_S))
        return GREATER_THAN_OR_EQUAL;
    else {
        fprintf(stderr, "%s: Cannot convert invalid string [%s] to RELOP\n",
                        "stringtoRELOP", str);
        exit(EXIT_FAILURE);
    }
}

