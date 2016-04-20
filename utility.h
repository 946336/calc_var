#ifndef CALC_UTILITY_H
#define CALC_UTILITY_H 

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

extern unsigned int LINE_NUMBER;
extern char *FILENAME;

/****************************************************************************/

typedef char *condition(char *c);
typedef bool word_condition(const char *fst, const char *snd);

/****************************************************************************/

// These will null-terminate strings
char *copy_string(const char *str);
char *copy_nstring(const char *str, size_t len);
// This one requires that first and second be null terminated
char *combine_string(char *first, char *second);

void print_string(char *str, FILE *fp);

char *drop_leading_whitespace(char *str);
char *find_next_whitespace(char *str);

char *find_char(char *domain, char target);
char *find_next(char *str, condition predicate);
char *find_next_word(char *str, word_condition predicate, const char *target);

bool leads_with(const char *first, const char *second);

char *isKeyword(char *token);
char *hasKeyword(char *token);
char *isLeadingKeyword(char *token);
char *isNonLeadingKeyword(char *token);
bool  isNumber(char *token);
char *isRelOp(char *str);
char *hasRelOp(char *str);

/* Expanding tabs is controlled by the MY_GETLINE_TABWIDTH define */
/* If MY_GETLINE_TABWIDTH is defined, my_getline() will replace   */
/* \t with however many spaces MY_GETLINE_TABWIDTH evaluates to   */
size_t my_getline(char **buf, size_t *size, FILE *fd);

#endif

