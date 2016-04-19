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

