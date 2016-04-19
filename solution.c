#ifndef CALC_ABSTRACT_SYNTAX_TREE_H
#define CALC_ABSTRACT_SYNTAX_TREE_H 

#include "value.h"
#include "operator.h"

#include "env.h"
#include "utility.h"

#include "value.h"

#include <stdbool.h>

typedef struct AST_Node {
    struct AST_Node *left;
    struct AST_Node *right;
    Value v;
} *AST_Node;

AST_Node AST_new();
AST_Node AST_newv(Value v);

AST_Node AST_copy(AST_Node root);

void AST_free(AST_Node *ast);

AST_Node AST_insert(Value v, AST_Node root);

AST_Node AST_insertoperator(AST_Node n, AST_Node root);
AST_Node AST_insertleaf(AST_Node n, AST_Node root);

bool bindsTighterThan(AST_Node lhs, AST_Node rhs);

void AST_print(AST_Node root);
void AST_print_verbose(AST_Node root);

void  AST_replace_vars(AST_Node root, Env e);
// Will print runtime errors for any variables it sees.
// Check integrity of AST structure
bool  AST_validate(AST_Node root);
Type  AST_typeof(AST_Node root, Env e);
Value AST_eval(AST_Node root);

AST_Node AST_rightmost(AST_Node root);

#endif

#ifndef CALC_ENVIRONMENT_BINDING_H
#define CALC_ENVIRONMENT_BINDING_H 

#include "value.h"

#include <stdlib.h>

#define T Binding
typedef struct T *T;

struct T {
    char *name;
    Value value;
    Binding rest;
};

static const T EMPTY_BINDING = NULL;

T Binding_new(char *name, Value v);

/* Free the entire chain of bindings */
void Binding_free(T *b);

T Binding_prepend(T new_b, T rest);
Value Binding_find(T list, char *name);

void Binding_print(T b);

#undef T
#endif

#ifndef CALC_LOOKUP_ENVIRONMENT_H
#define CALC_LOOKUP_ENVIRONMENT_H 

#include "value.h"
#include "binding.h"

#define T Env
typedef struct T *T;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                   *
 * Lookup environments binding names to values. Environments may be  * 
 * chained in a linked list style.                                   *
 *                                                                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

T    Env_new();
T    Env_new_extension(T e);
/* Free this environment and leave the rest */
void Env_free(T *e);
/* Recursively free all environments */
void Env_free_r(T *e);

Value Env_find(T e, char *name);
T     Env_bind(T e, char *name, Value val);

void Env_print(T e);

#undef T
#endif

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

#ifndef CALC_PARSE_H
#define CALC_PARSE_H 

#include "subexp.h"

// Client must free the SubExp returned here
SubExp parse(char *line, Env e);

#endif

#ifndef CALC_SCOPE_H
#define CALC_SCOPE_H 

#include <stdbool.h>

#include "ast.h"
#include "value.h"

typedef struct SubExp *SubExp;

SubExp SubExp_new();
void SubExp_free(SubExp *s);

void SubExp_print(SubExp s);

SubExp SubExp_new_layer(SubExp s);
SubExp SubExp_pop(SubExp s);

SubExp SubExp_add(SubExp s, Value v);

SubExp SubExp_collapse(SubExp s);
AST_Node SubExp_toAST(SubExp s);

bool SubExp_is_singleton(SubExp s);

#endif

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

char *drop_leading_whitespace(char *str);
char *find_next_whitespace(char *str);

char *find_char(char *domain, char target);
char *find_next(char *str, condition predicate);
char *find_next_word(char *str, word_condition predicate, const char *target);

bool leads_with(const char *first, const char *second);

char *isKeyword(char *token);
char *isLeadingKeyword(char *token);
char *isNonLeadingKeyword(char *token);
bool  isNumber(char *token);

/* Expanding tabs is controlled by the MY_GETLINE_TABWIDTH define */
/* If MY_GETLINE_TABWIDTH is defined, my_getline() will replace   */
/* \t with however many spaces MY_GETLINE_TABWIDTH evaluates to   */
size_t my_getline(char **buf, size_t *size, FILE *fd);

#endif

#ifndef CALC_VALUE_H
#define CALC_VALUE_H 

#include "operator.h"

typedef enum Type {NONE = -1, NUMBER, STRING, VAR, OP} Type;
const char *typestring(Type t);

typedef struct Value {
    Type type;
    union {
        double d;
        char  *s;
        char  *name;
        OPERATOR op;
    } u;
} Value;

static const Value NOTHING = {NONE, {0}};

Value Value_new_number(double d);
Value Value_new_string(const char *s);
Value Value_new_op(OPERATOR op);
Value Value_new_var(char *name);

Value Value_copy(Value v);

void Value_free(Value *v);

Value Value_combine(Value lhs, OPERATOR op, Value rhs);

void Value_print(Value v);

#endif


#include "ast.h"
#include "value.h"

#include <stdio.h>
#include <stdlib.h>

/****************************************************************************/

void AST_print_verbose_r_(AST_Node root);
void AST_print_r(AST_Node root);

/****************************************************************************/

AST_Node AST_new()
{
    AST_Node n = malloc(sizeof(*n));
    if (n == NULL) {
        perror("AST_new");
        exit(EXIT_FAILURE);
    }
    n->left = NULL;
    n->right = NULL;
    n->v = NOTHING;

    return n;
}

AST_Node AST_newv(Value v)
{
    AST_Node n = malloc(sizeof(*n));
    if (n == NULL) {
        perror("AST_newv");
        exit(EXIT_FAILURE);
    }

    n->left = NULL;
    n->right = NULL;
    n->v = v;

    return n;
}

AST_Node AST_copy(AST_Node root)
{
    if (root == NULL) return NULL;
    
    AST_Node n = AST_newv(Value_copy(root->v));
    n->left = AST_copy(root->left);
    n->right = AST_copy(root->right);

    return n;
}

void AST_free(AST_Node *root)
{
    if (root == NULL || *root == NULL) return;
    AST_free(&(*root)->left);
    AST_free(&(*root)->right);
    Value_free(&(*root)->v);
    free(*root);
}

AST_Node AST_insert(Value v, AST_Node root)
{
    if (v.type == NONE) return root;
    if (root == NULL) return AST_newv(v);

    AST_Node new_n = AST_newv(v);

    switch (v.type) {
        case VAR:
        case STRING:
        case NUMBER: return AST_insertleaf(new_n, root);
        case OP:
            if (v.u.op == PAREN) return AST_insertleaf(new_n, root);
            else return AST_insertoperator(new_n, root);
        case NONE: return root;
    }

    // Compiler dummy
    return root;
}

AST_Node AST_insertleaf(AST_Node n, AST_Node root)
{
    if (n == NULL || n->v.type == NONE) return root;
    if (root == NULL) return n;

    if (root->right != NULL) {
        AST_insertleaf(n, root->right);
    } else {
        root->right = n;
    }

    return root;
}

AST_Node AST_insertoperator(AST_Node n, AST_Node root)
{
    if (n == NULL || n->v.type == NONE) return root;
    if (root == NULL) return n;

    if (bindsTighterThan(n, root)) {
        if (!bindsTighterThan(n, root->right)) {
            n->left = root->right;
            root->right = n;
        } else {
            return AST_insertoperator(n, root->right);
        }
    } else {
        n->left = root;
        return n;
    }

    return root;
}

void AST_print(AST_Node root)
{
    AST_print_r(root);
    fputc('\n', stdout);
}

void AST_print_r(AST_Node root)
{
    if (root == NULL) { 
        return;
    }

    AST_print_r(root->left);

    switch (root->v.type) {
        case NONE: return;
        case NUMBER:
            fprintf(stdout, "%.15g ", root->v.u.d);
            break;
        case STRING:
            fprintf(stdout, "\"%s\" ", root->v.u.s);
            break;
        case OP:
            if (root->v.u.op != PAREN) {
                fprintf(stdout, "%c ", OPERATORtochar(root->v.u.op));
            } else {
                fprintf(stdout, "( ");
                AST_print_r(root->right);
                fprintf(stdout, ") ");
                return;
            }
            break;
        case VAR: 
            fprintf(stderr, "%s [Line %d]: "
                            "(Argh! You've found an interpreter bug!)",
                            FILENAME, LINE_NUMBER);
            break;
    }

    AST_print_r(root->right);
}

void AST_print_verbose(AST_Node root)
{
    AST_print_verbose_r_(root);
    fputc('\n', stdout);
}

void AST_print_verbose_r_(AST_Node root)
{
    if (root == NULL) return;

    fprintf(stdout, "(");

    AST_print_verbose_r_(root->left);
    switch (root->v.type) {
        case NONE: return;
        case NUMBER:
            fprintf(stdout, "%.15g", root->v.u.d);
            break;
        case STRING:
            fprintf(stdout, "\"%s\"", root->v.u.s);
            break;
        case OP:
            if (root->v.u.op != PAREN) {
                fprintf(stdout, " %c ", OPERATORtochar(root->v.u.op));
            } else {
                AST_print_verbose_r_(root->right);
                fprintf(stdout, ")");
                return;
            }
            break;
        case VAR:
            fprintf(stderr, "%s [Line %d]: "
                            "(Argh! You've found an interpreter bug!)",
                            FILENAME, LINE_NUMBER);
            break;
    }
    AST_print_verbose_r_(root->right);

    fprintf(stdout, ")");
}

void AST_replace_vars(AST_Node root, Env e)
{
    if (root == NULL) return;

    if (root->v.type == VAR) {
        Value v = Env_find(e, root->v.u.name);
        if (v.type != NONE) {
            Value_free(&(root->v));
            root->v = Value_copy(v);
        }
    }

    AST_replace_vars(root->left, e);
    AST_replace_vars(root->right, e);
}

bool AST_validate(AST_Node root)
{
    if (root == NULL) return false;

    switch (root->v.type) {
        case NONE: return false;
        case VAR:
            fprintf(stderr, "%s [Line %d]: Runtime error: Name [%s] not "
                            "bound\n", FILENAME, LINE_NUMBER,
                            root->v.u.name);
            if (root->left != NULL) {
                fprintf(stderr, "%s [LINE %d]: Argh! You've found an "
                                "interpreter bug! (AST_validate)\n",
                                FILENAME, LINE_NUMBER);
            }
            if (root->right != NULL) {
                fprintf(stderr, "%s [LINE %d]: Argh! You've found an "
                                "interpreter bug! (AST_validate)\n",
                                FILENAME, LINE_NUMBER);
            }
            return true;
        case NUMBER:
            if (root->left != NULL) {
                fprintf(stderr, "%s [LINE %d]: Argh! You've found an "
                                "interpreter bug! (AST_validate)\n",
                                FILENAME, LINE_NUMBER);
            }
            if (root->right != NULL) {
                fprintf(stderr, "%s [LINE %d]: Argh! You've found an "
                                "interpreter bug! (AST_validate)\n",
                                FILENAME, LINE_NUMBER);
            }
            return (root->left == NULL) && (root->right == NULL);            
        case STRING:
            if (root->left != NULL) {
                fprintf(stderr, "%s [LINE %d]: Argh! You've found an "
                                "interpreter bug! (AST_validate)\n",
                                FILENAME, LINE_NUMBER);
            }
            if (root->right != NULL) {
                fprintf(stderr, "%s [LINE %d]: Argh! You've found an "
                                "interpreter bug! (AST_validate)\n",
                                FILENAME, LINE_NUMBER);
            }
            return (root->left == NULL) && (root->right == NULL);            
        case OP:
            if (root->v.u.op != PAREN) {
                if ((root->left == NULL) || (root->right == NULL)) {
                    fprintf(stderr, "%s [Line %d]: Runtime error: Operator "
                                    "[%c] expects two arguments\n",
                                    FILENAME, LINE_NUMBER,
                                    OPERATORtochar(root->v.u.op));
                }
                return AST_validate(root->left) && AST_validate(root->right);
            } else {
                if (root->right == NULL) {
                    fprintf(stderr, "%s [Line %d]: Runtime error: Parentheses "
                                    "must not be empty\n",
                                    FILENAME, LINE_NUMBER);
                }
                return AST_validate(root->right);
            }
    }
    // Compiler dummy
    return true;
}

Type AST_typeof(AST_Node root, Env e)
{
    if (root == NULL) {
        return NONE;
    }

    Value lhs = NOTHING;
    Value rhs = NOTHING;

    switch (root->v.type) {
        case NONE:
            return root->v.type;
        case VAR:
            lhs = Env_find(e, root->v.u.name);
            return ((root->left != NULL) || (root->right != NULL)) ?
                NONE : lhs.type;
        case NUMBER: 
        case STRING:
            if ((root->left != NULL) || (root->right != NULL)) {
                return NONE;
            }
            break;
        case OP:
            if (root->v.u.op != PAREN) {
                lhs.type = AST_typeof(root->left, e);
                rhs.type = AST_typeof(root->right, e);

                if (lhs.type != rhs.type) {
                    fprintf(stderr, "%s [Line %d]: "
                                    "Type mismatch: Operator [%c] cannot "
                                    "operate on arguments of type [%s] and "
                                    "[%s]\n", FILENAME, LINE_NUMBER,
                                    OPERATORtochar(root->v.u.op),
                                    typestring(lhs.type),
                                    typestring(rhs.type));
                    return NONE;
                }
                if (root->v.u.op != SUM) {
                    return (lhs.type == NUMBER) ? NUMBER : NONE;
                } else return lhs.type;
            } else {
                return AST_typeof(root->right, e);
            }
    }
    return root->v.type;
}

Value AST_eval(AST_Node root)
{
    if (root == NULL) return NOTHING;

    Value vl;
    Value vr;
    Value result;

    if ((root->v.type == OP)) {
        if (root->v.u.op != PAREN) {
            vl = AST_eval(root->left);
            vr = AST_eval(root->right);
            result = Value_combine(vl, root->v.u.op, vr);
            Value_free(&vl);
            Value_free(&vr);
            return result;
        } else {
            return AST_eval(root->right);
        }
    } else {
        return Value_copy(root->v);
    }

}

AST_Node AST_rightmost(AST_Node root)
{
    if (root == NULL) return NULL;
    if (root->right == NULL) return root;
    return AST_rightmost(root->right);
}

/****************************************************************************/

bool bindsTighterThan(AST_Node lhs, AST_Node rhs)
{
    if (lhs == NULL) return true;
    if (rhs == NULL) return false;

    if (lhs->v.type == VAR) return true;
    if (rhs->v.type == VAR) return false;

    if (lhs->v.type == STRING) return true;
    if (rhs->v.type == STRING) return false;

    return hasHigherPriorityThan(lhs->v.u.op, rhs->v.u.op);
}


#include "binding.h"
#include "utility.h"

#include <stdlib.h>
#include <stdio.h>

#include <string.h>


/****************************************************************************/

Binding Binding_new(char *name, Value v)
{
    if (name == NULL) return EMPTY_BINDING;

    Binding nb = malloc(sizeof(*nb));
    if (nb == NULL) {
        perror("Binding_new");
        exit(EXIT_FAILURE);
    }
    nb->value = Value_copy(v);
    nb->name = copy_string(name);
    nb->rest = EMPTY_BINDING;

    return nb;
}

void Binding_free(Binding *b)
{
    if ((b == NULL) || (*b == EMPTY_BINDING)) return;
    Binding_free(&((*b)->rest));
    free((*b)->name);
    Value_free(&(*b)->value);
    free(*b);
    *b = EMPTY_BINDING;
}

Binding Binding_prepend(Binding new_b, Binding rest)
{
    if (new_b == EMPTY_BINDING) return rest;
    new_b->rest = rest;
    return new_b;
}

Value Binding_find(Binding list, char *name)
{
    if (list == EMPTY_BINDING) return NOTHING;
    if (strcmp(name, list->name) == 0) return list->value;
    else return Binding_find(list->rest, name);
}

void Binding_print(Binding b)
{
    if (b == NULL) return;
    
    switch (b->value.type) {
        case NUMBER:
            fprintf(stdout, "[%s] --> [%.15g]\n", b->name, b->value.u.d);
            break;
        case STRING:
            fprintf(stdout, "[%s] --> [%s]\n", b->name, b->value.u.s);
            break;
        default: break;
    }
    Binding_print(b->rest);
}

/****************************************************************************/


#include "env.h"

#include <stdlib.h>
#include <stdio.h>

#include <string.h>

struct Env {
    Binding bindings;
    Env rest;
};

Env Env_new()
{
    Env e = malloc(sizeof(*e));
    if (e == NULL) {
        perror("Env_new");
        exit(EXIT_FAILURE);
    }
    e->bindings = NULL;
    e->rest = NULL;

    return e;
}

Env Env_new_extension(Env e)
{
    Env new_env = Env_new();
    new_env->bindings = NULL;
    new_env->rest = e;

    return new_env;
}

void Env_free(Env *e)
{
    if (e == NULL || *e == NULL) return;
    Env rest = (*e)->rest;
    Binding_free(&(*e)->bindings);
    free(*e);
    *e = rest;
}

void Env_free_r(Env *e)
{
    if (e == NULL || *e == NULL) return;
    Env_free_r(&(*e)->rest);
    Env_free(e);
    *e = NULL;
}

Value Env_find(Env e, char *name)
{
    if (e == NULL) {
        fprintf(stderr, "%s: %s\n", "Env_find", "Expected non-NULL "
                        "environment");
        return NOTHING;
    }

    Value v = Binding_find(e->bindings, name);
    if ((v.type != NONE) || (e->rest == NULL)) {
        return v;
    } else {
        return Env_find(e->rest, name);
    }

}

Env Env_bind(Env e, char *name, Value val)
{
    if (name == NULL) return e;
    if (e == NULL) return e;

    Binding tmp;

    switch (val.type) {
        case NONE: return e;
        case NUMBER:
        case STRING:
            tmp  = Binding_new(name, val);
            e->bindings = Binding_prepend(tmp, e->bindings);
            break;
        default:
            return Env_bind(e, val.u.name, Value_copy(Env_find(e, val.u.name)));
            // fprintf(stderr, "Value type not supported\n");
    }
    return e;
}

void Env_print(Env e)
{
    if (e == NULL) return;
    Binding_print(e->bindings);
    Env_print(e->rest);
}

#include "env.h"
#include "binding.h"
#include "value.h"
#include "ast.h"
#include "utility.h"
#include "subexp.h"

#include "tokenize.h"
#include "parse.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

typedef enum VERBOSITY {QUIET, NORMAL, VERBOSE} VERBOSITY;
static VERBOSITY verbosity = NORMAL;

typedef enum ECHO {YES, NO} ECHO;
static ECHO echo = YES;

static const char *HELPME = "-q: Quiet - Suppress most output\n"
                            "-v: Verbose - Produce extra output\n"
                            "--no-echo: No echo - Don't echo parsed expression";
const char *INTERACTIVE_PROMPT = ">>> ";
const char *NONINTERACTIVE_PROMPT = "";

const char *PROMPT;

int main(int argc, char **argv)
{
    FILE *fp = stdin;
    PROMPT = INTERACTIVE_PROMPT;

    int i = 1;
    if (argc > 1) {
        for (; i < argc; ++i) {
            if (strcmp(argv[i], "-q") == 0) verbosity = QUIET;
            else if (strcmp(argv[i], "-v") == 0) verbosity = VERBOSE;
            else if (strcmp(argv[i], "--no-echo") == 0) echo = NO;
            else if (strcmp(argv[i], "-h") == 0) {
                fprintf(stdout, "%s", HELPME);
                exit(EXIT_SUCCESS);
            }
            else break;
        }
    }

    if (argv[i] != '\0') {
        fp = fopen(argv[i], "r");
        FILENAME = argv[i];
        PROMPT = NONINTERACTIVE_PROMPT;
        if (fp == NULL) {
            perror(argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (++i < argc) {
        fprintf(stderr, "Ignoring options specified after filename: "
                        " The first is: [%s]\n", argv[i]);
    }

/****************************************************************************/

    Env e = Env_new();

    char *line = NULL;
    size_t size = 0;
    size_t len = 0;

    if (verbosity != QUIET) fprintf(stdout, "%s", PROMPT);
    fflush(stdout);

    while ((len = my_getline(&line, &size, fp)) != (size_t) -1) {
        ++LINE_NUMBER;
        line[--len] = '\0';
        if (len == 0) {
            if (verbosity != QUIET) fprintf(stdout, "%s", PROMPT);
            fflush(stdout);
            continue;
        }

        SubExp s = parse(line, e);
        AST_Node root = SubExp_toAST(s);
        AST_replace_vars(root, e);
    
        if ((root != NULL) && !AST_validate(root)) {
            if (verbosity == VERBOSE)
                fprintf(stderr, "%s [Line %d]: "
                                "Incomplete expression!\n",
                                FILENAME, LINE_NUMBER);
        }
    
        Type t = AST_typeof(root, e);
        if (t != NONE) {
            if ((echo == YES) &&(verbosity == NORMAL)) {
                AST_print(root);
            } else if ((echo == YES) &&(verbosity == VERBOSE)) {
                AST_print_verbose(root);
            }
    
            Value result = AST_eval(root);
            switch (result.type) {
                case NUMBER:
                    fprintf(stdout, "= %.15g\n", result.u.d);
                    break;
                case STRING:
                    fprintf(stdout, "= \"%s\"\n", result.u.s);
                    break;
                default:
                    break;
                    fprintf(stderr, "%s [Line %d]: %s\n", FILENAME, LINE_NUMBER,
                                    "Argh! You've found an interpreter "
                                    "bug: Impossible value");
            }
            Value_free(&result);
        } else {
            if (verbosity == VERBOSE)
                fprintf(stderr, "%s [Line %d]: Expression is not well-typed/"
                                "well-formed\n", FILENAME, LINE_NUMBER);
        }
    
        AST_free(&root);
        SubExp_free(&s);

        if (verbosity != QUIET) fprintf(stdout, "%s", PROMPT);
        fflush(stdout);
    }
    Env_free(&e);
    free(line);

    if (fp != stdin) fclose(fp);
    fputc('\n', stdout);

    return 0;
}
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

#include "parse.h"
#include "env.h"
#include "value.h"
#include "operator.h"
#include "tokenize.h"

#include <stdio.h>
#include <stdlib.h>

#include <string.h>

/****************************************************************************/

Value let_binding(char **line, Env e);
Env where_binding(char **line, char *token, Env e);

SubExp expression(char **line, char *token);
Value string(char **line, char *token);

/****************************************************************************/

SubExp parse(char *line, Env e)
{
    // fprintf(stdout, "parse: [%s]\n", line);
    SubExp l = SubExp_new();
    if (line == NULL) return l;

    char *token = next_token(&line);

    if ((isLeadingKeyword(token) != NULL)) {
        if (strcmp(token, LET) == 0) {
            free(token);
            return SubExp_add(l, let_binding(&line, e));
        } else {
            fprintf(stderr, "%s [Line %d]: Argh! You've found an interpreter "
                            "bug! (parse)\n", FILENAME, LINE_NUMBER);
        }
    } else {
        SubExp_free(&l);
        // General expression must follow
        return expression(&line, token);
    }

    // Compiler dummy
    return l;
}

/****************************************************************************/

Value let_binding(char **line, Env e)
{
    (void) line;
    (void) e;

    char *name = next_token(line);
    char *assign = next_token(line);

    bool has_additional_bindings = false;

    SubExp s = parse(*line, e);

    char *token;
    char *tmp = *line;
    if ((*line = find_next_word(*line, leads_with, WHERE)) != tmp) {
        if ((token = next_token(line)) != NULL) {
            e = where_binding(line, token, Env_new_extension(e));
            free(token);
            has_additional_bindings = true;
        }
    }

    AST_Node root = SubExp_toAST(s);
    AST_replace_vars(root, e);

    if (has_additional_bindings) Env_free(&e);

    Value final = AST_eval(root);
    AST_free(&root);
    e = Env_bind(e, name, final);
    SubExp_free(&s);
    free(name);
    free(assign);

    return final;
}

Env where_binding(char **line, char *token, Env e)
{
    char *name = next_token(line);
    char *assign = next_token(line);

    if ((name == NULL) || (assign == NULL) ||
            (isNonLeadingKeyword(token)) == NULL) {
        fprintf(stderr, "%s [Line %d]: Syntax error: Expected additional "
                        "bindings\n", FILENAME, LINE_NUMBER);
        free(assign);
        free(name);
        return e;
    }

    token = next_token(line);
    SubExp s = expression(line, token);
    AST_Node root = SubExp_toAST(s);
    Type isComplete = AST_typeof(root, e);

    AST_replace_vars(root, e);
    if (isComplete != NONE) {
        Value v = AST_eval(root);
        e = Env_bind(e, name, v);
        Value_free(&v);
    }

    token = next_token(line);
    if (isNonLeadingKeyword(token)) {
        e = where_binding(line, token, e);
    }

    if (root != NULL) {
        AST_replace_vars(root, e);
        if (isComplete == NONE) {
            Value v = AST_eval(root);
            e = Env_bind(e, name, v);
            Value_free(&v);
        }
    } else {
        fprintf(stderr, "%s [Line %d]: Syntax error: expected additional "
                        "bindings\n", FILENAME, LINE_NUMBER);
    }

    free(name);
    free(assign);
    free(token);
    AST_free(&root);
    SubExp_free(&s);

    return e;
}

SubExp expression(char **line, char *token)
{
    // fprintf(stdout, "expression: [%s][%s]\n", token, *line);
    SubExp l = SubExp_new();

    char *last = NULL;

    do {
        // fprintf(stdout, "token: [%s]\n", token);
        if (isNumber(token)) {
            if ((last != NULL) && (*last == RPAREN)) {
                l = SubExp_add(l, Value_new_op(PROD)); 
            }
            l = SubExp_add(l, Value_new_number(strtod(token, NULL)));
        } else if (isOperator(token)) {
            if ((*token == LPAREN)) {
                if ((last != NULL) && ((*last == RPAREN) || isNumber(last) ||
                        (!isOperator(last) && !isNumber(last) &&
                         !isKeyword(last)))) {
                    l = SubExp_add(l, Value_new_op(PROD));
                }
                l = SubExp_add(l, Value_new_op(chartoOPERATOR(*token)));
                l = SubExp_new_layer(l);
            }
            else if (*token == RPAREN) {
                l = SubExp_collapse(l);
            } else l = SubExp_add(l, Value_new_op(chartoOPERATOR(*token)));
        } else if (*token == QUOTE) {
            // fprintf(stdout, "Starting string literal\n");
            l = SubExp_add(l, string(line, token));
            // if (**line == QUOTE) fprintf(stdout, "Ending string literal\n");
            // else fprintf(stdout, "Ended on [%s]\n", *line);
        } else {
            if ((last != NULL) && (*last == RPAREN)) {
                l = SubExp_add(l, Value_new_op(PROD)); 
            }
            l = SubExp_add(l, Value_new_var(token));
        }
        free(last);
        last = token;
    } while (((token = next_token(line)) != NULL) && 
             (isNonLeadingKeyword(token) == NULL));

    // Hacky and bad
    char *keyword = isNonLeadingKeyword(token);
    if (keyword != NULL) {
        *line -= strlen(isNonLeadingKeyword(token));
    }

    free(last);
    free(token);
    return l;
}

Value string(char **line, char *token)
{
    (void) token;
    char *walk = *line;
    while (*walk != QUOTE) {
        if (*walk == ESCAPE) ++walk;
        else if (*walk == '\0') return NOTHING;
        ++walk;
    }
    ++walk;
    Value v = {STRING, {.s = copy_nstring(*line, walk - *line - 1)}};
    *line = walk;
    return v;
}

#include "subexp.h"
#include "env.h"

#include <stdlib.h>
#include <stdio.h>

struct SubExp {
    AST_Node head;
    SubExp rest;
};

SubExp SubExp_new()
{
    SubExp s = malloc(sizeof(*s));
    if (s == NULL) {
        perror("SubExp_new");
        exit(EXIT_FAILURE);
    }

    s->head = NULL;
    s->rest = NULL;
    return s;
}

void SubExp_free(SubExp *s)
{
    if (s == NULL || *s == NULL) return;

    AST_free(&((*s)->head));
    SubExp_free(&((*s)->rest));
    free(*s);
    *s = NULL;
}

static void SubExp_print_r(SubExp s);
void SubExp_print(SubExp s)
{
    SubExp_print_r(s);
    fprintf(stdout, "\n");
}

void SubExp_print_r(SubExp s)
{
    if (s == NULL) return;
    AST_print(s->head);
    SubExp_print(s->rest);
}

SubExp SubExp_new_layer(SubExp s)
{
    SubExp n = SubExp_new();
    n->rest = s;
    return n;
}

SubExp SubExp_pop(SubExp s)
{
    if (s == NULL) return NULL;

    SubExp tmp = s->rest;
    free(s);
    return tmp;
}

SubExp SubExp_add(SubExp s, Value v)
{
    if (s == NULL) s = SubExp_new();
    s->head = AST_insert(v, s->head);

    return s;
}

SubExp SubExp_collapse(SubExp s)
{
    if (s == NULL) return NULL;
    if (s->rest == NULL) return s;

    SubExp first = s;
    SubExp second = s->rest;

    AST_Node right = AST_rightmost(second->head);
    if (right == NULL) second->head = s->head;
    else right->right = s->head;

    if (right->v.type != OP) fprintf(stderr, "Chained onto non-operator!\n");

    free(first);
    return second;
}

AST_Node SubExp_toAST(SubExp s)
{
    if (s == NULL) return NULL;

    if (SubExp_is_singleton(s)) {
        return AST_copy(s->head);
    } else {
        return SubExp_toAST(SubExp_collapse(s));
    }
}

bool SubExp_is_singleton(SubExp s)
{
    return ((s == NULL) || (s->rest == NULL)) ? true : false;
}
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
    while (isspace(*str)) ++str;
    return str;
}

char *find_next_whitespace(char *str)
{
     if (str == NULL) return NULL;
     while (!isspace(str)) ++str;
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


#include "value.h"
#include "utility.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/****************************************************************************/

static const char *NONE_S =  "NONE";
static const char *NUMBER_S = "NUMBER";
static const char *STRING_S = "STRING";
static const char *VAR_S = "VARIABLE";
static const char *OP_S = "OPERATOR";

const char *typestring(Type t)
{
    switch (t) {
        case NONE: return NONE_S;
        case NUMBER: return NUMBER_S;
        case STRING: return STRING_S;
        case VAR: return VAR_S;
        case OP: return OP_S;
    }
    // Compiler dummy
    return NONE_S;
}

/****************************************************************************/

static double do_math(double lhs, OPERATOR op, double rhs);

/****************************************************************************/

Value Value_new_number(double d)
{
    Value v = {NUMBER, {.d = d}};
    return v;
}

Value Value_new_string(const char *s)
{
    if (s == NULL) return NOTHING;
    Value v = {STRING, {.s = copy_string(s)}};
    return v;
}

Value Value_new_op(OPERATOR op)
{
    Value v = {OP, {.op = op}};
    return v;
}

Value Value_new_var(char *name)
{
    if (name == NULL) return NOTHING;
    Value v = {VAR, {.name = copy_string(name)}};
    return v;
}

Value Value_copy(Value v)
{
    Value n = v;
    switch (v.type) {
        case STRING:
            n.u.s = copy_string(v.u.s);
            break;
        case VAR:
            n.u.name = copy_string(v.u.name);
            break;
        default: return n;
    }
    return n;
}

void Value_free(Value *v)
{
    if (v == NULL) return;
    switch (v->type) {
        case STRING:    free(v->u.s); break;
        case VAR:       free(v->u.name); break;
        case OP:
        case NUMBER:
        default:        return;
    }
}

Value Value_combine(Value lhs, OPERATOR op, Value rhs)
{
    if (lhs.type != rhs.type) return NOTHING;

    Value v;

    switch (rhs.type) {
        case NUMBER:
            return Value_new_number(do_math(lhs.u.d, op, rhs.u.d));
        case STRING:
            v.type = STRING;
            v.u.s = combine_string(lhs.u.s, rhs.u.s);
            return v;
        case OP:
        default: return NOTHING;
    }
}

void Value_print(Value v)
{
    switch (v.type) {
        case NUMBER: fprintf(stdout, "[%g]", v.u.d); break;
        case STRING: fprintf(stdout, "[%s]", v.u.s); break;
        case VAR: fprintf(stdout, "[%s]", v.u.name); break;
        case OP: fprintf(stdout, "[%c]", OPERATORtochar(v.u.op)); break;
        case NONE: fprintf(stdout, "[NONE]");
    }
}

/****************************************************************************/

double do_math(double lhs, OPERATOR op, double rhs)
{
    switch (op) {
        case EXP:   return pow(lhs, rhs);
        case LOG:   return log(lhs) / log(rhs);
        case MOD:   return fmod(lhs, rhs);
        case INT:   return (int) lhs / (int) rhs;
        case PROD:  return lhs * rhs;
        case QUOT:  return lhs / rhs;
        case SUM:   return lhs + rhs;
        case DIFF:  return lhs - rhs;
        default: return 0;
    }
    return 0;
}

