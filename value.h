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

