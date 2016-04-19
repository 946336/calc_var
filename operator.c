/****************************************************************************/
/*                                operator.c                                */
/****************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include <string.h>

#include "operator.h"

/****************************************************************************/

static const char *LIT_STR =       "[LITERAL]";
static const char *PAREN_STR =     "[PARENTHESIZED GROUP]";
static const char *EXP_STR =       "[EXPONENTIATION]";
static const char *LOG_STR =       "[LOGARITHM]";
static const char *MOD_STR =       "[MODULUS]";
static const char *INT_STR =       "[INTEGER DIVISION]";
static const char *PROD_STR =      "[PRODUCT]";
static const char *QUOT_STR =      "[QUOTIENT]";
static const char *SUM_STR =       "[SUM]";
static const char *DIFF_STR =      "[DIFFERENCE]";

/****************************************************************************/

bool isOperator(char *str)
{
    if (str == NULL) return false;
    if (str[1] != '\0') return false;

    switch (*str) {
        case LPAREN:     return    true;
        case RPAREN:     return    true;
        case EXP_CHAR:   return    true;
        case LOG_CHAR:   return    true;
        case MOD_CHAR:   return    true;
        case INT_CHAR:   return    true;
        case PROD_CHAR:  return    true;
        case QUOT_CHAR:  return    true;
        case SUM_CHAR:   return    true;
        case DIFF_CHAR:  return    true;
        default:         return    false;
    }
}

const char *OPERATORtostring(OPERATOR op)
{
    switch (op) {
        case LITERAL:   return LIT_STR;
        case PAREN:     return PAREN_STR;
        case EXP:       return EXP_STR;
        case LOG:       return LOG_STR;
        case MOD:       return MOD_STR;
        case INT:       return INT_STR;
        case PROD:      return PROD_STR;
        case QUOT:      return QUOT_STR;
        case SUM:       return SUM_STR;
        case DIFF:      return DIFF_STR;
        default:        return NULL;
    }
}


OPERATOR stringtoOPERATOR(char *str)
{
    if (!strcmp(str, LIT_STR))           return LITERAL;
    else if (!strcmp(str, PAREN_STR))    return PAREN;
    else if (!strcmp(str, EXP_STR))      return EXP;
    else if (!strcmp(str, LOG_STR))      return LOG;
    else if (!strcmp(str, MOD_STR))      return MOD;
    else if (!strcmp(str, INT_STR))      return INT;
    else if (!strcmp(str, PROD_STR))     return PROD;
    else if (!strcmp(str, QUOT_STR))     return QUOT;
    else if (!strcmp(str, SUM_STR))      return SUM;
    else if (!strcmp(str, DIFF_STR))     return DIFF;
    else {
        fprintf(stderr, "%s [%s] %s\n", "stringtoOPERATOR: cannot convert "
                                        "invalid string", str, "to OPERATOR");
        exit(EXIT_FAILURE);
    }
}

OPERATOR chartoOPERATOR(char c)
{
    switch (c) {
        case LPAREN:     return    PAREN;
        case RPAREN:     return    PAREN;
        case EXP_CHAR:   return    EXP;
        case LOG_CHAR:   return    LOG;
        case MOD_CHAR:   return    MOD;
        case INT_CHAR:   return    INT;
        case PROD_CHAR:  return    PROD;
        case QUOT_CHAR:  return    QUOT;
        case SUM_CHAR:   return    SUM;
        case DIFF_CHAR:  return    DIFF;
        default:
            fprintf(stderr, "%s [%c] %s\n", "chartoOPERATOR: cannot convert "
                                            "invalid character", c,
                                            "to OPERATOR");
            exit(EXIT_FAILURE);
    }
}

char OPERATORtochar(OPERATOR op)
{
    switch (op) {
        case LITERAL:   return LIT_CHAR;
        case PAREN:     return PAREN_CHAR;
        case EXP:       return EXP_CHAR;
        case LOG:       return LOG_CHAR;
        case MOD:       return MOD_CHAR;
        case INT:       return INT_CHAR;
        case PROD:      return PROD_CHAR;
        case QUOT:      return QUOT_CHAR;
        case SUM:       return SUM_CHAR;
        case DIFF:      return DIFF_CHAR;
        default:
            fprintf(stderr, "%s\n", "OPERATORtochar: cannot convert invalid "
                                    "OPERATOR to character");
            exit(EXIT_FAILURE);
    }
}

bool hasHigherPriorityThan(OPERATOR lhs, OPERATOR rhs)
{
    if (lhs == rhs) return false;

    switch (lhs) {
        case LITERAL:   return true;
        case PAREN:     return lhs < rhs;
        case EXP:       return (lhs < rhs) && !(rhs == LOG) ;
        case LOG:       return lhs < rhs;
        case MOD:       return (lhs < rhs) && !(rhs == INT) ;
        case INT:       return lhs < rhs;
        case PROD:      return (lhs < rhs) && !(rhs == QUOT);
        case QUOT:      return (lhs < rhs);
        case SUM:       return (lhs < rhs) && !(rhs == DIFF);
        case DIFF:      return (lhs < rhs);
        default:        return false;
    }
}

