
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
        case BOOL:
        case STRING:
        case NUMBER: return AST_insertleaf(new_n, root);
        case OP:
            if (v.u.op == PAREN) return AST_insertleaf(new_n, root);
            else return AST_insertoperator(new_n, root);
        case RELAT_OP:
            return AST_insertoperator(new_n, root);
        case NONE:
        case INVALID: return root;
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
            AST_insertoperator(n, root->right);
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
        case NONE:      return;
        case INVALID:   return;
        case NUMBER:
            fprintf(stdout, "%.15g ", root->v.u.d);
            break;
        case STRING:
            fputc('\"', stdout);
            print_string(root->v.u.s, stdout);
            fputc('\"', stdout);
            break;
        case BOOL:
            fprintf(stdout, "%s ", root->v.u.b ? "<True>" : "<False>");
            break;
        case RELAT_OP:
            fprintf(stdout, "%s ", RELOPtostring(root->v.u.rop));
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
        case NONE:      return;
        case INVALID:   return;
        case NUMBER:
            fprintf(stdout, "%.15g", root->v.u.d);
            break;
        case STRING:
            fputc('\"', stdout);
            print_string(root->v.u.s, stdout);
            fputc('\"', stdout);
            break;
        case BOOL:
            fprintf(stdout, "%s", root->v.u.b ? "<True>" : "<False>");
            break;
        case RELAT_OP:
            fprintf(stdout, " %s ", RELOPtostring(root->v.u.rop));
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
        case NONE:      return false;
        case INVALID:   return false;
        case VAR:
            fprintf(stderr, "%s [Line %d]: Runtime error: Name [%s] not "
                            "bound\n", FILENAME, LINE_NUMBER,
                            root->v.u.name);
            return true;
        case BOOL:
            return (root->left == NULL) && (root->right == NULL);            
        case NUMBER:
            return (root->left == NULL) && (root->right == NULL);            
        case STRING:
            return (root->left == NULL) && (root->right == NULL);            
        case RELAT_OP:
            return AST_validate(root->left) && AST_validate(root->right);
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

Type AST_typeof(AST_Node root, Env e, bool show_errors)
{
    if (root == NULL) {
        return NONE;
    }

    Value lhs = NOTHING;
    Value rhs = NOTHING;

    switch (root->v.type) {
        case NONE:
        case INVALID:
            return root->v.type;
        case VAR:
            lhs = Env_find(e, root->v.u.name);
            return ((root->left != NULL) || (root->right != NULL)) ?
                INVALID : lhs.type;
        case NUMBER:
        case BOOL:
        case STRING:
            if ((root->left != NULL) || (root->right != NULL)) {
                return INVALID;
            } else return root->v.type;
            break;
        case RELAT_OP:
            lhs.type = AST_typeof(root->left, e, show_errors);
            rhs.type = AST_typeof(root->right, e, show_errors);
            if (lhs.type != rhs.type) {
                fprintf(stderr, "%s [Line %d]: Type mismatch: Relational "
                                "operator [%s] cannot operate on arguments "
                                "of type [%s] and [%s]\n", FILENAME,
                                LINE_NUMBER, RELOPtostring(root->v.u.rop),
                                typestring(lhs.type), typestring(rhs.type));
                return INVALID;
            } else return ((rhs.type == NUMBER) || (rhs.type == STRING)
                            || (rhs.type == BOOL)) ?
                            BOOL : INVALID;
        case OP:
            if (root->v.u.op != PAREN) {
                lhs.type = AST_typeof(root->left, e, show_errors);
                rhs.type = AST_typeof(root->right, e, show_errors);

                if (lhs.type != rhs.type) {
                    if (show_errors) {
                        fprintf(stderr, "%s [Line %d]: "
                                        "Type mismatch: Operator [%c] cannot "
                                        "operate on arguments of type [%s] and "
                                        "[%s]\n", FILENAME, LINE_NUMBER,
                                        OPERATORtochar(root->v.u.op),
                                        typestring(lhs.type),
                                        typestring(rhs.type));
                    }
                    return INVALID;
                }
                if (root->v.u.op != SUM) {
                    return (lhs.type == NUMBER) ? NUMBER : INVALID;
                } else return lhs.type;
            } else {
                return AST_typeof(root->right, e, show_errors);
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
    } else if (root->v.type == RELAT_OP) {
        vl = AST_eval(root->left);
        vr = AST_eval(root->right);
        result = Value_relate(vl, root->v.u.rop, vr);
        Value_free(&vl);
        Value_free(&vr);
        return result;
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

    if (lhs->v.type == RELAT_OP) return false;
    if (rhs->v.type == RELAT_OP) return true;

    return hasHigherPriorityThan(lhs->v.u.op, rhs->v.u.op);
}

