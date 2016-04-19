
#include "value.h"
#include "utility.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/****************************************************************************/

static const char *NONE_S =  "INVALID";
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

