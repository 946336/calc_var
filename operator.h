/****************************************************************************/
/*                                operator.h                                */
/****************************************************************************/

#ifndef CALC_OPERATOR_H
#define CALC_OPERATOR_H 

#include <stdbool.h>

typedef enum OPERATOR { LITERAL = 0,
                    PAREN,
                    EXP, LOG,
                    MOD, INT,
                    PROD, QUOT,
                    SUM, DIFF
                  } OPERATOR;
static const char operators[] = "()^|%\\*/+-";

/****************************************************************************/

#define LIT_CHAR    '$'
#define PAREN_CHAR  '&'
#define EXP_CHAR    '^'
#define LOG_CHAR    '|'
#define MOD_CHAR    '%'
#define INT_CHAR    '\\'
#define PROD_CHAR   '*'
#define QUOT_CHAR   '/'
#define SUM_CHAR    '+'
#define DIFF_CHAR   '-'

#define LPAREN '('
#define RPAREN ')'

/****************************************************************************/

bool isOperator(char *str);

OPERATOR  stringtoOPERATOR(char *str);
const char *OPERATORtostring(OPERATOR op);

OPERATOR chartoOPERATOR(char c);
char OPERATORtochar(OPERATOR op);

/* Exponentiation is right-associative */
/* Everything else if left-associative */
bool hasHigherPriorityThan(OPERATOR lhs, OPERATOR rhs);

#endif

