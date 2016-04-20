#ifndef CALC_TOKENIZE_H
#define CALC_TOKENIZE_H 

#include "value.h"
#include "operator.h"

#include <stdbool.h>

static const char DELIMS[] = " =\"!<>";
static const char ASSIGN = '=';
static const char QUOTE = '"';
static const char ESCAPE = '\\';

static const char LET[] = "let";
static const char WHERE[] = "where";
static const char AND[] = "and";

static const char *LEADING_KEYWORDS[] = 
{
    LET
};
static unsigned int NUM_LEADING_KEYWORDS =
    (sizeof(LEADING_KEYWORDS) / sizeof(char *));

static const char *NONLEADING_KEYWORDS[] = 
{
    WHERE, AND
};

static unsigned int NUM_NONLEADING_KEYWORDS = 
    (sizeof(NONLEADING_KEYWORDS) / sizeof(char *));

static const char NOT[] = "!";
static const char IS_EQUAL[] = "=";
static const char IS_NOT_EQUAL[] = "!=";
static const char IS_LESS_THAN[] = "<";
static const char IS_GREATER_THAN[] = ">";
static const char IS_LESS_THAN_OR_EQUAL[] = "<=";
static const char IS_GREATER_THAN_OR_EQUAL[] = ">=";

static const char *RELOPS[] = 
{
    NOT, IS_EQUAL, IS_NOT_EQUAL, IS_LESS_THAN, IS_GREATER_THAN,
    IS_LESS_THAN_OR_EQUAL, IS_GREATER_THAN_OR_EQUAL
};
static const unsigned int NUM_RELOPS = sizeof(RELOPS) / sizeof(char*);

char *next_token(char **str);

char *isDelim(char *character);

#endif

