#ifndef CALC_VALUE_H
#define CALC_VALUE_H 

#include "operator.h"
#include "relop.h"

#include <stdbool.h>

typedef enum Type {
    INVALID = -2, NONE = -1, NUMBER, STRING, VAR, BOOL,
    OP, RELAT_OP
} Type;

const char *typestring(Type t);
bool is_literal_type(Type t);

typedef struct Value {
    Type type;
    union {
        double d;
        char  *s;
        char  *name;
        OPERATOR op;
        RELOP rop;
        bool b;
    } u;
} Value;

static const Value NOTHING = {NONE, {0}};

Value Value_new_number(double d);
Value Value_new_string(const char *s);
Value Value_new_op(OPERATOR op);
Value Value_new_var(char *name);
Value Value_new_bool(bool b);
Value Value_new_relop(RELOP r);

Value Value_copy(Value v);

void Value_free(Value *v);

Value Value_combine(Value lhs, OPERATOR op, Value rhs);
Value Value_relate(Value lhs, RELOP op, Value rhs);

void Value_print(Value v);

#endif

