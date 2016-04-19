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

void SubExp_replace_vars(SubExp s, Env e)
{
    if ((s == NULL) || (e == NULL)) return;
    AST_replace_vars(s->head, e);
}

bool SubExp_is_singleton(SubExp s)
{
    return ((s == NULL) || (s->rest == NULL)) ? true : false;
}