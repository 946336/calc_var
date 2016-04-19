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
void     SubExp_replace_vars(SubExp s, Env e);

bool SubExp_is_singleton(SubExp s);

#endif

