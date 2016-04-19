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

