#ifndef CALC_TOKENIZE_H
#define CALC_TOKENIZE_H 

#include "value.h"
#include "operator.h"

#include <stdbool.h>

static const char DELIMS[] = " =\"";
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

char *next_token(char **str);

char *isDelim(char *character);

#endif

