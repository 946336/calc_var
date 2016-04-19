
#include "utility.h"
#include "tokenize.h"

#include <stdlib.h>
#include <stdio.h>

#include <ctype.h>
#include <string.h>

/****************************************************************************/

unsigned int LINE_NUMBER = 0;
char *FILENAME = "Standard Input";

/****************************************************************************/

char *copy_string(const char *str)
{
    if (str == NULL) return NULL;

    size_t size = 20;
    size_t i = 0;

    char *buf = malloc(size * sizeof(char));
    if (buf == NULL) {
        perror("copy_name");
        exit(EXIT_FAILURE);
    }

    for (i = 0; str[i] != '\0'; ++i) {
        if (i == size) {
            buf = realloc(buf, 2 * size);
            size *= 2;
            if (buf == NULL) {
                perror("copy_name");
                exit(EXIT_FAILURE);
            }
        }
        buf[i] = str[i];
    }
    buf[i] = '\0';

    return buf;
}

char *copy_nstring(const char *str, size_t len)
{
    if (str == NULL) return NULL;
    size_t i = 0;

    /* Extra space for '\0' */
    char *buf = malloc(len * sizeof(char) + 1);
    if (buf == NULL) {
        perror("copy_nstring");
        exit(EXIT_FAILURE);
    }

    for (i = 0; (i < len) && (str[i] != '\0'); ++i) {
        buf[i] = str[i];
    }
    for (; i <= len; ++i) {
        buf[i] = '\0';
    }

    return buf;
}

char *combine_string(char *start, char *second)
{
    if (start == NULL) return second;
    if (second == NULL) return start;

    size_t l1 = strlen(start);
    size_t l2 = strlen(second);

    char *buf = malloc(((l1 * l2) * sizeof(*buf)) + 1);
    strncpy(buf, start, l1);
    strncpy(buf + l1, second, l2 + 1);

    return buf;
}

char *drop_leading_whitespace(char *str)
{
    if (str == NULL) return NULL;
    while (isspace((int) *str)) ++str;
    return str;
}

char *find_next_whitespace(char *str)
{
     if (str == NULL) return NULL;
     while (!isspace((int) *str)) ++str;
     return str;
}

char *find_char(char *domain, char target)
{
    if (domain == NULL) return domain;
    for (char *tmp = domain; *tmp != '\0'; ++tmp) {
        if (*tmp == target) return tmp;
    }
    return domain;
}

char *find_next(char *str, condition predicate)
{
    if (str == NULL) return str;
    char *tmp = str;
    for (; *tmp != '\0'; ++tmp) {
        if (predicate(tmp) != NULL) return tmp;
    }
    return tmp;
}

char *find_next_word(char *str, word_condition predicate, const char *target)
{
    if (str == NULL) return str;
    char *tmp = str;
    for (; *tmp != '\0'; ++tmp) {
        if (predicate(tmp, target)) {
            return tmp;
        }
    }
    return str;
}

bool leads_with(const char *first, const char *second)
{
    if (first == second) return true;
    if ((first == NULL) || (second == NULL)) return true;

    while ((*first != '\0') && (*second != '\0') && (*first == *second)) {
        // Inlining these increments to the condition causes problems
        ++first;
        ++second;
        // fprintf(stdout, "[%c] matches [%c]\n", *first++, *second++);
    }
    if ((*first == '\0') || (*second == '\0')) {
        // fprintf(stdout, "[%s] and [%s] share heads\n", first, second);
        return true;
    }
    else return false;
}

char *hasKeyword(char *token)
{
    if (token == NULL) return NULL;

    // fprintf(stdout, "Matching token [%s] across [%u] keywords\n",
                    // token, NUM_KEYWORDS);
    for (unsigned i = 0; i < NUM_LEADING_KEYWORDS; ++i) {
        if (leads_with(LEADING_KEYWORDS[i], token)) {
            // fprintf(stdout, "Matched token [%s] to keyword [%s]\n",
            //                 token, KEYWORDS[i]);
            return token;
        }
    }
    for (unsigned i = 0; i < NUM_NONLEADING_KEYWORDS; ++i) {
        if (leads_with(NONLEADING_KEYWORDS[i], token)) {
            // fprintf(stdout, "Matched token [%s] to keyword [%s]\n",
            //                 token, KEYWORDS[i]);
            return token;
        }
    }
    return NULL;
}

char *isKeyword(char *token)
{
    if (token == NULL) return NULL;

    // fprintf(stdout, "Matching token [%s] across [%u] keywords\n",
                    // token, NUM_KEYWORDS);
    for (unsigned i = 0; i < NUM_LEADING_KEYWORDS; ++i) {
        if (strcmp(LEADING_KEYWORDS[i], token) == 0) {
            // fprintf(stdout, "Matched token [%s] to keyword [%s]\n",
            //                 token, KEYWORDS[i]);
            return token;
        }
    }
    for (unsigned i = 0; i < NUM_NONLEADING_KEYWORDS; ++i) {
        if (strcmp(NONLEADING_KEYWORDS[i], token) == 0) {
            // fprintf(stdout, "Matched token [%s] to keyword [%s]\n",
            //                 token, KEYWORDS[i]);
            return token;
        }
    }
    return NULL;
}

char *isLeadingKeyword(char *token)
{
    if (token == NULL) return NULL;

    // fprintf(stdout, "Matching token [%s] across [%u] keywords\n",
                    // token, NUM_KEYWORDS);
    for (unsigned i = 0; i < NUM_LEADING_KEYWORDS; ++i) {
        if (strcmp(LEADING_KEYWORDS[i], token) == 0) {
            // fprintf(stdout, "Matched token [%s] to keyword [%s]\n",
            //                 token, KEYWORDS[i]);
            return token;
        }
    }
    return NULL;
}

char *isNonLeadingKeyword(char *token)
{
    if (token == NULL) return NULL;

    // fprintf(stdout, "Matching token [%s] across [%u] keywords\n",
                    // token, NUM_KEYWORDS);
    for (unsigned i = 0; i < NUM_NONLEADING_KEYWORDS; ++i) {
        if (strcmp(NONLEADING_KEYWORDS[i], token) == 0) {
            // fprintf(stdout, "Matched token [%s] to keyword [%s]\n",
            //                 token, KEYWORDS[i]);
            return token;
        }
    }
    return NULL;
}


bool isNumber(char *token)
{
    if (token == NULL) return false;
    char *end = NULL;
    strtod(token, &end);
    return *end == '\0';
}

/* Expanding tabs is controlled by the MY_GETLINE_TABWIDTH define */
/* If MY_GETLINE_TABWIDTH is defined, my_getline() will replace   */
/* \t with however many spaces MY_GETLINE_TABWIDTH evaluates to   */
size_t my_getline(char **buf, size_t *size, FILE *fd)
{
        static const int  INIT_SIZE = 256;
        static const char DELIM1 = '\n';
        static const char DELIM2 = '\r';
        static const char NULLCHAR = '\0';

        if (size == NULL) return (size_t) -1;

        if (*buf == NULL) {
                *buf = malloc(INIT_SIZE * sizeof(**buf));
                if (*buf == NULL) return (size_t) -1;
                *size = INIT_SIZE;
        }

        int c;
        size_t i = 0;

        char peeking;
        do {
                if ((i + 2) >= *size){
                        *buf = realloc(*buf, 2 * (*size) + 1);
                        *size *= 2;
                        if (*buf == NULL) {
                                return (size_t) -1;
                        }
                }

                c = fgetc(fd);
                if ((i + 2) >= *size){
                        *buf = realloc(*buf, 2 * (*size) + 1);
                        *size *= 2;
                        if (*buf == NULL) {
                                return (size_t) -1;
                        }
                }
                /* Lines that are actually blank (no newline at all) are */
                /* reported as invalid. Should only happen in an empty   */
                /* file.                                                 */
                if (c < 0){
                        if (i == 0){
                                *size = -1;
                                return (size_t) -1;
                        }
                        else ++i;
                }
                else {
                        (*buf)[i] = c;
                        /* UNIX line endings:       \n   */
                        /* OSX line endings:        \r   */
                        /* Windows line endings     \r\n */
                        #if defined(MY_GETLINE_TABWIDTH)
                                if ((*buf)[i] == TAB) {
                                        if (*size <=
                                            i + MY_GETLINE_TABWIDTH + 1) {
                                                *buf = realloc(*buf,
                                                              2 * (*size) + 1);
                                                *size *= 2;
                                                if (*buf == NULL)
                                                        return (size_t) -1;
                                        }
                                        #if defined(MY_GETLINE_TABSTOPS)
                                        int dst_to_tabstop =
                                                ((i % MY_GETLINE_TABWIDTH) ? 
                                                (i % MY_GETLINE_TABWIDTH) :
                                                MY_GETLINE_TABWIDTH);
                                        for (size_t q = i;
                                             q < i + dst_to_tabstop;
                                             ++q) {
                                                (*buf)[q] = ' ';
                                        }
                                        i += dst_to_tabstop - 1;
                                        #else
                                        for (size_t q = i;
                                             q < i + MY_GETLINE_TABWIDTH;
                                             ++q) {
                                                (*buf)[q] = ' ';
                                        }
                                        i += MY_GETLINE_TABWIDTH - 1;
                                        #endif
                                }
                        #endif
                        /* UNIX or OS9 */
                        if (((*buf)[i] == DELIM1) || (*buf)[i] == DELIM2) {
                                /* Windows */
                                if ((c == DELIM2) &&
                                        (peeking = fgetc(fd)) != DELIM1)
                                                ungetc(peeking, fd);
                                (*buf)[++i] = NULLCHAR;
                                return i;
                        }
                        ++i;
                }
        } while(c > 0); /* EOF returns a negative value */

        (*buf)[i] = NULLCHAR;
        return i;
}

